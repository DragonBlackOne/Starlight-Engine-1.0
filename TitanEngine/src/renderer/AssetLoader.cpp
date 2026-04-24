#include "AssetLoader.hpp"
#include "Log.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"

// --- Open Source Power (MIT License) ---
#include <meshoptimizer.h>

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

#include <vector>
#include <fstream>
#include <glad/glad.h>

namespace titan {
    // =========================================================================
    //  MESH OPTIMIZER (meshoptimizer by zeux - MIT License)
    //  Reorders triangles for GPU vertex cache locality, reduces overdraw,
    //  and optimizes vertex fetch order for maximum memory bandwidth.
    // =========================================================================
    void AssetLoader::OptimizeMesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
        if (indices.empty() || vertices.empty()) return;

        size_t indexCount = indices.size();
        size_t vertexCount = vertices.size();

        // 1. Vertex Cache Optimization (Forsyth/Tipsify algorithm)
        // Reorders triangles to maximize post-transform vertex cache hits on the GPU.
        std::vector<uint32_t> optimizedIndices(indexCount);
        meshopt_optimizeVertexCache(optimizedIndices.data(), indices.data(), indexCount, vertexCount);

        // 2. Overdraw Optimization
        // Reorders triangles to reduce pixel overdraw (pixels shaded but later occluded).
        // Uses the optimized cache order as input, threshold 1.05 = max 5% degradation from cache optimal.
        meshopt_optimizeOverdraw(optimizedIndices.data(), optimizedIndices.data(), indexCount,
            &vertices[0].position.x, vertexCount, sizeof(Vertex), 1.05f);

        // 3. Vertex Fetch Optimization
        // Reorders the vertex buffer to match the access pattern of the optimized index buffer,
        // improving spatial locality and GPU memory prefetching.
        std::vector<uint32_t> remap(vertexCount);
        meshopt_optimizeVertexFetchRemap(remap.data(), optimizedIndices.data(), indexCount, vertexCount);
        meshopt_remapIndexBuffer(optimizedIndices.data(), optimizedIndices.data(), indexCount, remap.data());

        std::vector<Vertex> optimizedVertices(vertexCount);
        meshopt_remapVertexBuffer(optimizedVertices.data(), vertices.data(), vertexCount, sizeof(Vertex), remap.data());

        indices = std::move(optimizedIndices);
        vertices = std::move(optimizedVertices);

        Log::Info("meshoptimizer: Optimized " + std::to_string(vertexCount) + " vertices, " + std::to_string(indexCount / 3) + " triangles.");
    }

    // =========================================================================
    //  OBJ LOADER (tinyobj_loader_c) + meshoptimizer post-process
    // =========================================================================
    static void file_reader(void* ctx, const char* path, int is_mtl, const char* obj_path, char** buf, size_t* size) {
        (void)ctx;
        (void)is_mtl;
        (void)obj_path;
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open()) return;
        *size = file.tellg();
        *buf = (char*)malloc(*size);
        file.seekg(0);
        file.read(*buf, *size);
    }

    std::shared_ptr<Mesh> AssetLoader::LoadOBJ(const std::string& path) {
        tinyobj_attrib_t attrib;
        tinyobj_shape_t* shapes = NULL;
        size_t num_shapes;
        tinyobj_material_t* materials = NULL;
        size_t num_materials;

        int result = tinyobj_parse_obj(&attrib, &shapes, &num_shapes, &materials, &num_materials, path.c_str(), file_reader, NULL, TINYOBJ_FLAG_TRIANGULATE);
        
        if (result != TINYOBJ_SUCCESS) {
            Log::Error("Failed to load OBJ: " + path);
            return nullptr;
        }

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        for (size_t i = 0; i < attrib.num_face_num_verts; i++) {
            for (size_t f = 0; f < attrib.face_num_verts[i]; f++) {
                tinyobj_vertex_index_t idx = attrib.faces[i * 3 + f];
                
                Vertex v;
                v.position = {
                    attrib.vertices[idx.v_idx * 3 + 0],
                    attrib.vertices[idx.v_idx * 3 + 1],
                    attrib.vertices[idx.v_idx * 3 + 2]
                };

                if (idx.vn_idx >= 0) {
                    v.normal = {
                        attrib.normals[idx.vn_idx * 3 + 0],
                        attrib.normals[idx.vn_idx * 3 + 1],
                        attrib.normals[idx.vn_idx * 3 + 2]
                    };
                } else {
                    v.normal = {0, 1, 0};
                }

                if (idx.vt_idx >= 0) {
                    v.texCoords = {
                        attrib.texcoords[idx.vt_idx * 2 + 0],
                        attrib.texcoords[idx.vt_idx * 2 + 1]
                    };
                } else {
                    v.texCoords = {0, 0};
                }
                
                vertices.push_back(v);
                indices.push_back((uint32_t)indices.size());
            }
        }

        // meshoptimizer post-process: cache + overdraw + fetch optimization
        OptimizeMesh(vertices, indices);

        auto mesh = std::make_shared<Mesh>(vertices, indices);

        tinyobj_attrib_free(&attrib);
        if (shapes) tinyobj_shapes_free(shapes, num_shapes);
        if (materials) tinyobj_materials_free(materials, num_materials);

        return mesh;
    }

    // =========================================================================
    //  GLTF/GLB LOADER (cgltf by jkuhlmann - MIT License)
    //  Loads the industry-standard PBR format used by Blender, Godot, Unity, Unreal.
    // =========================================================================
    std::shared_ptr<Mesh> AssetLoader::LoadGLTF(const std::string& path) {
        cgltf_options options = {};
        cgltf_data* data = nullptr;

        cgltf_result result = cgltf_parse_file(&options, path.c_str(), &data);
        if (result != cgltf_result_success) {
            Log::Error("cgltf: Failed to parse: " + path);
            return nullptr;
        }

        result = cgltf_load_buffers(&options, data, path.c_str());
        if (result != cgltf_result_success) {
            Log::Error("cgltf: Failed to load buffers: " + path);
            cgltf_free(data);
            return nullptr;
        }

        if (data->meshes_count == 0) {
            Log::Error("cgltf: No meshes found in: " + path);
            cgltf_free(data);
            return nullptr;
        }

        // Load the first mesh, first primitive (standard convention for single-mesh assets)
        cgltf_mesh& gltfMesh = data->meshes[0];
        if (gltfMesh.primitives_count == 0) {
            Log::Error("cgltf: No primitives in mesh: " + path);
            cgltf_free(data);
            return nullptr;
        }

        cgltf_primitive& prim = gltfMesh.primitives[0];

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        // --- Extract vertex attributes ---
        const float* positionData = nullptr;
        const float* normalData = nullptr;
        const float* texcoordData = nullptr;
        size_t vertexCount = 0;

        for (cgltf_size a = 0; a < prim.attributes_count; a++) {
            cgltf_attribute& attr = prim.attributes[a];
            cgltf_accessor* accessor = attr.data;

            if (attr.type == cgltf_attribute_type_position) {
                vertexCount = accessor->count;
                positionData = (const float*)((const char*)accessor->buffer_view->buffer->data + accessor->buffer_view->offset + accessor->offset);
            }
            else if (attr.type == cgltf_attribute_type_normal) {
                normalData = (const float*)((const char*)accessor->buffer_view->buffer->data + accessor->buffer_view->offset + accessor->offset);
            }
            else if (attr.type == cgltf_attribute_type_texcoord) {
                texcoordData = (const float*)((const char*)accessor->buffer_view->buffer->data + accessor->buffer_view->offset + accessor->offset);
            }
        }

        if (!positionData || vertexCount == 0) {
            Log::Error("cgltf: No position data in: " + path);
            cgltf_free(data);
            return nullptr;
        }

        vertices.resize(vertexCount);
        for (size_t i = 0; i < vertexCount; i++) {
            vertices[i].position = { positionData[i * 3], positionData[i * 3 + 1], positionData[i * 3 + 2] };
            if (normalData) {
                vertices[i].normal = { normalData[i * 3], normalData[i * 3 + 1], normalData[i * 3 + 2] };
            } else {
                vertices[i].normal = { 0, 1, 0 };
            }
            if (texcoordData) {
                vertices[i].texCoords = { texcoordData[i * 2], texcoordData[i * 2 + 1] };
            } else {
                vertices[i].texCoords = { 0, 0 };
            }
            vertices[i].jointIndices = glm::ivec4(0);
            vertices[i].weights = glm::vec4(0.0f);
        }

        // --- Extract indices ---
        if (prim.indices) {
            cgltf_accessor* idxAccessor = prim.indices;
            indices.resize(idxAccessor->count);
            for (cgltf_size i = 0; i < idxAccessor->count; i++) {
                indices[i] = (uint32_t)cgltf_accessor_read_index(idxAccessor, i);
            }
        } else {
            // No index buffer: generate sequential indices
            indices.resize(vertexCount);
            for (size_t i = 0; i < vertexCount; i++) {
                indices[i] = (uint32_t)i;
            }
        }

        // meshoptimizer post-process
        OptimizeMesh(vertices, indices);

        auto mesh = std::make_shared<Mesh>(vertices, indices);

        Log::Info("cgltf: Loaded '" + path + "' (" + std::to_string(vertexCount) + " verts, " + std::to_string(indices.size() / 3) + " tris)");

        cgltf_free(data);
        return mesh;
    }

    // =========================================================================
    //  TEXTURE LOADING (stb_image - MIT/Public Domain)
    // =========================================================================
    uint32_t AssetLoader::LoadTexture(const std::string& path) {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

        if (!data) {
            Log::Error("Failed to load texture: " + path);
            return 0;
        }

        uint32_t texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        return texture;
    }

    uint32_t AssetLoader::CreateCheckerboardTexture(int width, int height, int cellSize) {
        std::vector<unsigned char> data(width * height * 3);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                bool isWhite = ((x / cellSize) + (y / cellSize)) % 2 == 0;
                unsigned char c = isWhite ? 255 : 180;
                int idx = (y * width + x) * 3;
                data[idx] = c;
                data[idx + 1] = c;
                data[idx + 2] = c;
            }
        }

        uint32_t texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        return texture;
    }
}
