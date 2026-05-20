// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#include "AssetLoader.hpp"
#include "Log.hpp"
#pragma warning(push, 0)
#include <codeanalysis/warnings.h>
#pragma warning(disable: ALL_CODE_ANALYSIS_WARNINGS)

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"

// --- Open Source Power (MIT License) ---
#include <meshoptimizer.h>

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

#pragma warning(pop)

#include <vector>
#include <fstream>
#include <glad/glad.h>

namespace starlight {
    // =========================================================================
    //  MESH OPTIMIZER (meshoptimizer by zeux - MIT License)
    //  Reorders triangles for GPU vertex cache locality, reduces overdraw,
    //  and optimizes vertex fetch order for maximum memory bandwidth.
    // =========================================================================
    void AssetLoader::OptimizeMesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
        if (indices.empty() || vertices.empty()) return;

        size_t indexCount = indices.size();
        size_t vertexCount = vertices.size();

        // 1. Vertex Cache Optimization (Strip optimization for modern GPUs)
        std::vector<uint32_t> optimizedIndices(indexCount);
        meshopt_optimizeVertexCacheStrip(optimizedIndices.data(), indices.data(), indexCount, vertexCount);

        // 2. Overdraw Optimization
        meshopt_optimizeOverdraw(optimizedIndices.data(), optimizedIndices.data(), indexCount,
            &vertices[0].position.x, vertexCount, sizeof(Vertex), 1.05f);

        // 3. Vertex Fetch Optimization
        std::vector<uint32_t> remap(vertexCount);
        meshopt_optimizeVertexFetchRemap(remap.data(), optimizedIndices.data(), indexCount, vertexCount);
        meshopt_remapIndexBuffer(optimizedIndices.data(), optimizedIndices.data(), indexCount, remap.data());

        std::vector<Vertex> optimizedVertices(vertexCount);
        meshopt_remapVertexBuffer(optimizedVertices.data(), vertices.data(), vertexCount, sizeof(Vertex), remap.data());

        indices = std::move(optimizedIndices);
        vertices = std::move(optimizedVertices);

        Log::Info("meshoptimizer (v1.1): Highly optimized " + std::to_string(vertexCount) + " vertices, " + std::to_string(indexCount / 3) + " triangles.");
    }

    MeshData AssetLoader::SimplifyMesh(const MeshData& input, float targetRatio) {
        if (input.indices.empty() || input.vertices.empty()) return {};

        size_t indexCount = input.indices.size();
        size_t targetIndexCount = (size_t)(indexCount * targetRatio);
        float targetError = 0.01f;

        std::vector<uint32_t> simplifiedIndices(indexCount);
        size_t newIndexCount = meshopt_simplify(
            simplifiedIndices.data(), 
            input.indices.data(), 
            indexCount, 
            &input.vertices[0].position.x, 
            input.vertices.size(), 
            sizeof(Vertex), 
            targetIndexCount, 
            targetError
        );

        simplifiedIndices.resize(newIndexCount);

        MeshData output;
        output.vertices = input.vertices; // Reuse original vertices for simplicity in this helper
        output.indices = std::move(simplifiedIndices);
        output.valid = true;

        Log::Info("meshoptimizer: Simplified mesh from " + std::to_string(indexCount / 3) + " to " + std::to_string(newIndexCount / 3) + " triangles.");
        return output;
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

    MeshData AssetLoader::LoadOBJ(const std::string& path) {
        MeshData meshData;
        tinyobj_attrib_t attrib;
        tinyobj_shape_t* shapes = NULL;
        size_t num_shapes;
        tinyobj_material_t* materials = NULL;
        size_t num_materials;

        int result = tinyobj_parse_obj(&attrib, &shapes, &num_shapes, &materials, &num_materials, path.c_str(), file_reader, NULL, TINYOBJ_FLAG_TRIANGULATE);
        
        if (result != TINYOBJ_SUCCESS) {
            Log::Error("Failed to load OBJ: " + path);
            return meshData;
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

        meshData.vertices = std::move(vertices);
        meshData.indices = std::move(indices);
        meshData.valid = true;

        tinyobj_attrib_free(&attrib);
        if (shapes) tinyobj_shapes_free(shapes, num_shapes);
        if (materials) tinyobj_materials_free(materials, num_materials);

        return meshData;
    }

    // =========================================================================
    //  GLTF/GLB LOADER (cgltf by jkuhlmann - MIT License)
    //  Loads the industry-standard PBR format used by Blender, Godot, Unity, Unreal.
    // =========================================================================
    MeshData AssetLoader::LoadGLTF(const std::string& path) {
        MeshData meshData;
        cgltf_options options = {};
        cgltf_data* data = nullptr;

        cgltf_result result = cgltf_parse_file(&options, path.c_str(), &data);
        if (result != cgltf_result_success) {
            Log::Error("cgltf: Failed to parse: " + path);
            return meshData;
        }

        result = cgltf_load_buffers(&options, data, path.c_str());
        if (result != cgltf_result_success) {
            Log::Error("cgltf: Failed to load buffers: " + path);
            cgltf_free(data);
            return meshData;
        }

        if (data->meshes_count == 0) {
            Log::Error("cgltf: No meshes found in: " + path);
            cgltf_free(data);
            return meshData;
        }

        // Load the first mesh, first primitive (standard convention for single-mesh assets)
        cgltf_mesh& gltfMesh = data->meshes[0];
        if (gltfMesh.primitives_count == 0) {
            Log::Error("cgltf: No primitives in mesh: " + path);
            cgltf_free(data);
            return meshData;
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
            return meshData;
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

        meshData.vertices = std::move(vertices);
        meshData.indices = std::move(indices);
        meshData.valid = true;

        Log::Info("cgltf: Loaded '" + path + "' (" + std::to_string(vertexCount) + " verts, " + std::to_string(meshData.indices.size() / 3) + " tris)");

        cgltf_free(data);
        return meshData;
    }

    // =========================================================================
    //  TEXTURE LOADING (stb_image - MIT/Public Domain)
    // =========================================================================
    uint32_t AssetLoader::LoadTexture(const std::string& path, bool removeCheckered) {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(false);
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4); 

        if (!data) {
            Log::Error("Failed to load texture: " + path);
            return 0;
        }
        Log::Info("Texture Loaded: " + path + " (" + std::to_string(width) + "x" + std::to_string(height) + ")");

        if (removeCheckered) {
            for (int i = 0; i < width * height * 4; i += 4) {
                unsigned char r = data[i];
                unsigned char g = data[i + 1];
                unsigned char b = data[i + 2];
                
                // Discard near-white and near-gray checkerboard patterns
                bool isGray = (r > 200 && r < 210 && g > 200 && g < 210 && b > 200 && b < 210);
                bool isWhite = (r > 250 && g > 250 && b > 250);
                
                if (isGray || isWhite) {
                    data[i + 3] = 0; // Alpha = 0
                }
            }
        }

        uint32_t texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        return texture;
    }

    uint32_t AssetLoader::CreateProceduralIsometricTile(const glm::vec3& color) {
        const int size = 256;
        unsigned char* data = new unsigned char[size * size * 4];
        
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                int idx = (y * size + x) * 4;
                
                float uvx = (float)x / (float)size;
                float uvy = (float)y / (float)size;
                
                // Isometric Diamond Mask
                float dist = abs(uvx - 0.5f) + abs(uvy - 0.5f) * 2.0f;
                
                if (dist <= 0.5f) {
                    float factor = 1.0f - dist; // Lighter at center
                    data[idx + 0] = (unsigned char)(color.r * factor * 255);
                    data[idx + 1] = (unsigned char)(color.g * factor * 255);
                    data[idx + 2] = (unsigned char)(color.b * factor * 255);
                    data[idx + 3] = 255;
                    
                    // Add a small border highlight
                    if (dist > 0.48f) {
                        data[idx + 0] = 255; data[idx + 1] = 255; data[idx + 2] = 255;
                    }
                } else {
                    data[idx + 0] = 0; data[idx + 1] = 0; data[idx + 2] = 0; data[idx + 3] = 0;
                }
            }
        }

        uint32_t texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        delete[] data;
        return texture;
    }

    uint32_t AssetLoader::CreateProceduralBuilding(const glm::vec3& color) {
        // Mode 0: Corporate (Tall), Mode 1: Residential (Wide), Mode 2: Factory (Low)
        const int size = 256;
        unsigned char* data = new unsigned char[size * size * 4];
        memset(data, 0, size * size * 4);

        int type = (color.r > 0.4f) ? 0 : (color.g > 0.4f ? 1 : 2);

        int bx = 80, by = 40, bw = 96, bh = 180;
        if (type == 1) { bx = 60; by = 100; bw = 136; bh = 120; }
        if (type == 2) { bx = 50; by = 140; bw = 156; bh = 80; }

        for (int y = by; y < by + bh; y++) {
            for (int x = bx; x < bx + bw; x++) {
                int idx = (y * size + x) * 4;
                
                data[idx + 0] = (unsigned char)(color.r * 0.3f * 255);
                data[idx + 1] = (unsigned char)(color.g * 0.3f * 255);
                data[idx + 2] = (unsigned char)(color.b * 0.3f * 255);
                data[idx + 3] = 255;

                // Windows / Lights
                if (type == 0 && (y % 10 < 3) && (x % 20 < 10)) { data[idx + 1] = 255; data[idx + 2] = 255; }
                if (type == 1 && (y % 15 < 5) && (x % 30 < 15)) { data[idx + 0] = 255; data[idx + 1] = 200; }
                
                // Outline
                if (x == bx || x == bx+bw-1 || y == by || y == by+bh-1) {
                    data[idx + 0] = 100; data[idx + 1] = 100; data[idx + 2] = 100;
                }
            }
        }

        uint32_t texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        delete[] data;
        return texture;
    }

    uint32_t AssetLoader::LoadCubemap(const std::vector<std::string>& faces) {
        uint32_t textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < faces.size(); i++) {
            unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            } else {
                Log::Error("Cubemap texture failed to load at path: " + faces[i]);
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
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
