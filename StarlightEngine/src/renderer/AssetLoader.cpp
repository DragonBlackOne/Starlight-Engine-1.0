#include "AssetLoader.hpp"
#include "PathResolver.hpp"
#include "VFSSystem.hpp"
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
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <glad/glad.h>

namespace fs = std::filesystem;

class PathResolver {
public:
    static fs::path ResolveAssetPath(const std::string& relativePath) {
        // Lista de locais para procurar os assets
        std::vector<fs::path> searchPaths = {
            fs::current_path() / relativePath,
            fs::current_path() / "assets" / relativePath,
            fs::current_path() / ".." / "assets" / relativePath,
            fs::current_path() / "StarlightEngine" / "assets" / relativePath
        };

        for (const auto& path : searchPaths) {
            if (fs::exists(path)) {
                return fs::canonical(path);
            }
        }

        std::cerr << "[AssetLoader] Erro: Arquivo não encontrado: " << relativePath << "\n";
        return relativePath;
    }
};

namespace starlight {
    // =========================================================================
    //  MESH OPTIMIZER (meshoptimizer by zeux - MIT License)
    //  Reorders triangles for GPU vertex cache locality, reduces overdraw,
    //  and optimizes vertex fetch order for maximum memory bandwidth.
    // =========================================================================
    void AssetLoader::OptimizeMesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
        if (indices.empty() || vertices.empty() || vertices.size() < 16) return;

        size_t indexCount = indices.size();
        size_t vertexCount = vertices.size();

        // 1. Vertex Cache Optimization (Triangle cache locality)
        std::vector<uint32_t> optimizedIndices(indexCount);
        meshopt_optimizeVertexCache(optimizedIndices.data(), indices.data(), indexCount, vertexCount);

        // 2. Overdraw Optimization
        meshopt_optimizeOverdraw(optimizedIndices.data(), optimizedIndices.data(), indexCount,
            &vertices[0].position.x, vertexCount, sizeof(Vertex), 1.05f);

        // 3. Vertex Fetch Optimization
        std::vector<Vertex> optimizedVertices(vertexCount);
        size_t uniqueVertices = meshopt_optimizeVertexFetch(
            optimizedVertices.data(), optimizedIndices.data(), indexCount,
            vertices.data(), vertexCount, sizeof(Vertex)
        );
        if (uniqueVertices > 0 && uniqueVertices <= vertexCount) {
            optimizedVertices.resize(uniqueVertices);
            indices = std::move(optimizedIndices);
            vertices = std::move(optimizedVertices);
        }
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
        auto bytes = VFSSystem::Get().ReadFile(path);
        if (bytes.empty()) return;
        *size = bytes.size();
        *buf = (char*)malloc(*size);
        memcpy(*buf, bytes.data(), *size);
    }

    MeshData AssetLoader::LoadOBJ(const std::string& path) {
        MeshData meshData;
        tinyobj_attrib_t attrib;
        tinyobj_shape_t* shapes = NULL;
        size_t num_shapes;
        tinyobj_material_t* materials = NULL;
        size_t num_materials;

        std::string resolved = PathResolver::Resolve(path);
        int result = tinyobj_parse_obj(&attrib, &shapes, &num_shapes, &materials, &num_materials, resolved.c_str(), file_reader, NULL, TINYOBJ_FLAG_TRIANGULATE);
        
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
    static cgltf_result cgltf_read_vfs(const struct cgltf_memory_options* memory_options, const struct cgltf_file_options* file_options, const char* path, cgltf_size* size, void** data) {
        (void)memory_options;
        (void)file_options;
        auto bytes = VFSSystem::Get().ReadFile(path);
        if (bytes.empty()) {
            return cgltf_result_file_not_found;
        }
        *size = bytes.size();
        *data = malloc(*size);
        memcpy(*data, bytes.data(), *size);
        return cgltf_result_success;
    }

    static void cgltf_release_vfs(const struct cgltf_memory_options* memory_options, const struct cgltf_file_options* file_options, void* data) {
        (void)memory_options;
        (void)file_options;
        free(data);
    }

    MeshData AssetLoader::LoadGLTF(const std::string& path) {
        MeshData meshData;
        cgltf_options options = {};
        options.file.read = cgltf_read_vfs;
        options.file.release = cgltf_release_vfs;
        cgltf_data* data = nullptr;

        std::string resolved = PathResolver::Resolve(path);
        cgltf_result result = cgltf_parse_file(&options, resolved.c_str(), &data);
        if (result != cgltf_result_success) {
            Log::Error("cgltf: Failed to parse: " + resolved);
            return meshData;
        }

        result = cgltf_load_buffers(&options, data, resolved.c_str());
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

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        // Traverse all meshes and all primitives to concatenate submeshes
        for (cgltf_size m = 0; m < data->meshes_count; ++m) {
            cgltf_mesh& gltfMesh = data->meshes[m];
            for (cgltf_size p = 0; p < gltfMesh.primitives_count; ++p) {
                cgltf_primitive& prim = gltfMesh.primitives[p];

                // --- Extract vertex attributes ---
                cgltf_accessor* positionAccessor = nullptr;
                cgltf_accessor* normalAccessor = nullptr;
                cgltf_accessor* texcoordAccessor = nullptr;
                size_t vertexCount = 0;

                for (cgltf_size a = 0; a < prim.attributes_count; a++) {
                    cgltf_attribute& attr = prim.attributes[a];
                    cgltf_accessor* accessor = attr.data;

                    if (!accessor) continue;
                    if (!accessor->buffer_view && !accessor->is_sparse) continue;

                    if (attr.type == cgltf_attribute_type_position) {
                        vertexCount = accessor->count;
                        positionAccessor = accessor;
                    }
                    else if (attr.type == cgltf_attribute_type_normal) {
                        normalAccessor = accessor;
                    }
                    else if (attr.type == cgltf_attribute_type_texcoord) {
                        texcoordAccessor = accessor;
                    }
                }

                if (!positionAccessor || vertexCount == 0) {
                    continue;
                }

                size_t baseVertexIdx = vertices.size();
                size_t oldVertCount = vertices.size();
                vertices.resize(oldVertCount + vertexCount);

                float elem[4] = {0.0f, 0.0f, 0.0f, 0.0f};
                for (size_t i = 0; i < vertexCount; i++) {
                    Vertex& v = vertices[oldVertCount + i];
                    cgltf_accessor_read_float(positionAccessor, i, elem, 3);
                    v.position = { elem[0], elem[1], elem[2] };
                    if (normalAccessor) {
                        cgltf_accessor_read_float(normalAccessor, i, elem, 3);
                        v.normal = { elem[0], elem[1], elem[2] };
                    } else {
                        v.normal = { 0.0f, 1.0f, 0.0f };
                    }
                    if (texcoordAccessor) {
                        cgltf_accessor_read_float(texcoordAccessor, i, elem, 2);
                        v.texCoords = { elem[0], elem[1] };
                    } else {
                        v.texCoords = { 0.0f, 0.0f };
                    }
                    v.jointIndices = glm::ivec4(0);
                    v.weights = glm::vec4(0.0f);
                }

                // --- Extract indices ---
                if (prim.indices) {
                    cgltf_accessor* idxAccessor = prim.indices;
                    size_t oldIndexCount = indices.size();
                    indices.resize(oldIndexCount + idxAccessor->count);
                    for (cgltf_size i = 0; i < idxAccessor->count; i++) {
                        indices[oldIndexCount + i] = (uint32_t)(baseVertexIdx + cgltf_accessor_read_index(idxAccessor, i));
                    }
                } else {
                    size_t oldIndexCount = indices.size();
                    indices.resize(oldIndexCount + vertexCount);
                    for (size_t i = 0; i < vertexCount; i++) {
                        indices[oldIndexCount + i] = (uint32_t)(baseVertexIdx + i);
                    }
                }
            }
        }

        if (vertices.empty()) {
            Log::Error("cgltf: Loaded empty geometry from: " + path);
            cgltf_free(data);
            return meshData;
        }

        // meshoptimizer post-process
        OptimizeMesh(vertices, indices);

        meshData.vertices = std::move(vertices);
        meshData.indices = std::move(indices);
        meshData.valid = true;

        Log::Info("cgltf: Loaded '" + path + "' (" + std::to_string(meshData.vertices.size()) + " verts, " + std::to_string(meshData.indices.size() / 3) + " tris)");

        cgltf_free(data);
        return meshData;
    }

    // =========================================================================
    //  TEXTURE LOADING (stb_image - MIT/Public Domain)
    // =========================================================================
    uint32_t AssetLoader::LoadTexture(const std::string& path, bool removeCheckered) {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(false);
        std::string resolved = PathResolver::Resolve(path);
        unsigned char* data = stbi_load(resolved.c_str(), &width, &height, &channels, 4); 

        if (!data) {
            Log::Error("Failed to load texture: " + resolved);
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
            std::string resolved = PathResolver::Resolve(faces[i]);
            unsigned char* data = stbi_load(resolved.c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            } else {
                Log::Error("Cubemap texture failed to load at path: " + resolved);
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
        if (cellSize <= 0) cellSize = 1;
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

    // =========================================================================
    //  PROCEDURAL 3D PRIMITIVE GENERATORS
    // =========================================================================
    std::shared_ptr<Mesh> AssetLoader::CreatePlaneMesh(float width, float depth, float tileU, float tileV) {
        float hw = width * 0.5f;
        float hd = depth * 0.5f;

        std::vector<Vertex> vertices = {
            {{-hw, 0.0f, -hd}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}},
            {{ hw, 0.0f, -hd}, {0.0f, 1.0f, 0.0f}, {tileU, 0.0f}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}},
            {{ hw, 0.0f,  hd}, {0.0f, 1.0f, 0.0f}, {tileU, tileV}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}},
            {{-hw, 0.0f,  hd}, {0.0f, 1.0f, 0.0f}, {0.0f, tileV}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}}
        };

        std::vector<uint32_t> indices = {0, 3, 2, 0, 2, 1};
        return std::make_shared<Mesh>(vertices, indices);
    }

    std::shared_ptr<Mesh> AssetLoader::CreateBoxMesh(float width, float height, float depth) {
        float hw = width * 0.5f;
        float hh = height * 0.5f;
        float hd = depth * 0.5f;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        auto addFace = [&](glm::vec3 n, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
            uint32_t baseIdx = static_cast<uint32_t>(vertices.size());
            vertices.push_back({p0, n, {0.0f, 0.0f}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}});
            vertices.push_back({p1, n, {1.0f, 0.0f}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}});
            vertices.push_back({p2, n, {1.0f, 1.0f}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}});
            vertices.push_back({p3, n, {0.0f, 1.0f}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}});
            indices.push_back(baseIdx + 0);
            indices.push_back(baseIdx + 1);
            indices.push_back(baseIdx + 2);
            indices.push_back(baseIdx + 0);
            indices.push_back(baseIdx + 2);
            indices.push_back(baseIdx + 3);
        };

        // Front (+Z)
        addFace({0.0f, 0.0f, 1.0f}, {-hw, -hh,  hd}, { hw, -hh,  hd}, { hw,  hh,  hd}, {-hw,  hh,  hd});
        // Back (-Z)
        addFace({0.0f, 0.0f, -1.0f}, { hw, -hh, -hd}, {-hw, -hh, -hd}, {-hw,  hh, -hd}, { hw,  hh, -hd});
        // Top (+Y)
        addFace({0.0f, 1.0f, 0.0f}, {-hw,  hh,  hd}, { hw,  hh,  hd}, { hw,  hh, -hd}, {-hw,  hh, -hd});
        // Bottom (-Y)
        addFace({0.0f, -1.0f, 0.0f}, {-hw, -hh, -hd}, { hw, -hh, -hd}, { hw, -hh,  hd}, {-hw, -hh,  hd});
        // Right (+X)
        addFace({1.0f, 0.0f, 0.0f}, { hw, -hh,  hd}, { hw, -hh, -hd}, { hw,  hh, -hd}, { hw,  hh,  hd});
        // Left (-X)
        addFace({-1.0f, 0.0f, 0.0f}, {-hw, -hh, -hd}, {-hw, -hh,  hd}, {-hw,  hh,  hd}, {-hw,  hh, -hd});

        return std::make_shared<Mesh>(vertices, indices);
    }

    std::shared_ptr<Mesh> AssetLoader::CreateCylinderMesh(float radius, float height, int segments) {
        if (segments < 3) segments = 3;
        float hh = height * 0.5f;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        // Side vertices
        for (int i = 0; i <= segments; ++i) {
            float u = static_cast<float>(i) / segments;
            float angle = u * 2.0f * 3.1415926535f;
            float x = std::cos(angle) * radius;
            float z = std::sin(angle) * radius;
            glm::vec3 normal = glm::normalize(glm::vec3(x, 0.0f, z));

            vertices.push_back({{x, -hh, z}, normal, {u, 0.0f}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}});
            vertices.push_back({{x,  hh, z}, normal, {u, 1.0f}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}});
        }

        for (int i = 0; i < segments; ++i) {
            uint32_t idx = i * 2;
            indices.push_back(idx + 0);
            indices.push_back(idx + 1);
            indices.push_back(idx + 3);

            indices.push_back(idx + 0);
            indices.push_back(idx + 3);
            indices.push_back(idx + 2);
        }

        // Top Cap
        uint32_t topCenterIdx = static_cast<uint32_t>(vertices.size());
        vertices.push_back({{0.0f, hh, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, 0.5f}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}});
        for (int i = 0; i <= segments; ++i) {
            float u = static_cast<float>(i) / segments;
            float angle = u * 2.0f * 3.1415926535f;
            float x = std::cos(angle) * radius;
            float z = std::sin(angle) * radius;
            vertices.push_back({{x, hh, z}, {0.0f, 1.0f, 0.0f}, {0.5f + 0.5f * std::cos(angle), 0.5f + 0.5f * std::sin(angle)}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}});
        }
        for (int i = 0; i < segments; ++i) {
            indices.push_back(topCenterIdx);
            indices.push_back(topCenterIdx + 1 + i);
            indices.push_back(topCenterIdx + 2 + i);
        }

        // Bottom Cap
        uint32_t botCenterIdx = static_cast<uint32_t>(vertices.size());
        vertices.push_back({{0.0f, -hh, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.5f, 0.5f}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}});
        for (int i = 0; i <= segments; ++i) {
            float u = static_cast<float>(i) / segments;
            float angle = u * 2.0f * 3.1415926535f;
            float x = std::cos(angle) * radius;
            float z = std::sin(angle) * radius;
            vertices.push_back({{x, -hh, z}, {0.0f, -1.0f, 0.0f}, {0.5f + 0.5f * std::cos(angle), 0.5f + 0.5f * std::sin(angle)}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}});
        }
        for (int i = 0; i < segments; ++i) {
            indices.push_back(botCenterIdx);
            indices.push_back(botCenterIdx + 2 + i);
            indices.push_back(botCenterIdx + 1 + i);
        }

        return std::make_shared<Mesh>(vertices, indices);
    }

    std::shared_ptr<Mesh> AssetLoader::CreateSphereMesh(float radius, int rings, int segments) {
        if (rings < 3) rings = 3;
        if (segments < 3) segments = 3;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        for (int r = 0; r <= rings; ++r) {
            float v = static_cast<float>(r) / rings;
            float phi = v * 3.1415926535f;

            for (int s = 0; s <= segments; ++s) {
                float u = static_cast<float>(s) / segments;
                float theta = u * 2.0f * 3.1415926535f;

                float x = std::sin(phi) * std::cos(theta);
                float y = std::cos(phi);
                float z = std::sin(phi) * std::sin(theta);

                glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));
                glm::vec3 pos = normal * radius;

                vertices.push_back({pos, normal, {u, v}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}});
            }
        }

        for (int r = 0; r < rings; ++r) {
            for (int s = 0; s < segments; ++s) {
                uint32_t first = r * (segments + 1) + s;
                uint32_t second = first + segments + 1;

                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }

        return std::make_shared<Mesh>(vertices, indices);
    }

    std::shared_ptr<Mesh> AssetLoader::CreateCapsuleMesh(float radius, float height, int rings, int segments) {
        if (rings < 2) rings = 2;
        if (segments < 3) segments = 3;
        float halfCylinderH = std::max(0.0f, height * 0.5f - radius);

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        // Build a complete capsule: top hemisphere + cylinder body + bottom hemisphere.
        struct CapsuleRow {
            float phi;
            float y;
            float sideRad;
        };
        std::vector<CapsuleRow> rows;

        // Top hemisphere (phi from 0 at the pole to pi/2 at the equator)
        for (int i = 0; i <= rings; ++i) {
            float phi = static_cast<float>(i) / rings * (3.1415926535f * 0.5f);
            rows.push_back({phi, halfCylinderH + std::cos(phi) * radius, std::sin(phi) * radius});
        }
        // Cylinder body (constant radius, from top equator down to bottom equator)
        int cylRows = std::max(1, rings);
        for (int i = 1; i <= cylRows; ++i) {
            float t = static_cast<float>(i) / cylRows;
            rows.push_back({3.1415926535f * 0.5f, halfCylinderH - t * 2.0f * halfCylinderH, radius});
        }
        // Bottom hemisphere (phi from pi/2 down to pi), skipping the shared equator row
        for (int i = 1; i <= rings; ++i) {
            float phi = 3.1415926535f * 0.5f + static_cast<float>(i) / rings * (3.1415926535f * 0.5f);
            rows.push_back({phi, -halfCylinderH + std::cos(phi) * radius, std::sin(phi) * radius});
        }

        for (size_t r = 0; r < rows.size(); ++r) {
            const auto& row = rows[r];
            float uvV = static_cast<float>(r) / (rows.size() - 1);
            for (int s = 0; s <= segments; ++s) {
                float u = static_cast<float>(s) / segments;
                float theta = u * 2.0f * 3.1415926535f;

                float x = std::sin(row.phi) * std::cos(theta);
                float y = std::cos(row.phi);
                float z = std::sin(row.phi) * std::sin(theta);

                glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));
                glm::vec3 pos = normal * radius + glm::vec3(0.0f, row.y - std::cos(row.phi) * radius, 0.0f);

                vertices.push_back({pos, normal, {u, uvV}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}});
            }
        }

        int rowCount = static_cast<int>(rows.size());
        for (int r = 0; r < rowCount - 1; ++r) {
            for (int s = 0; s < segments; ++s) {
                uint32_t first = static_cast<uint32_t>(r * (segments + 1) + s);
                uint32_t second = first + segments + 1;

                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }

        return std::make_shared<Mesh>(vertices, indices);
    }

    std::shared_ptr<Mesh> AssetLoader::CreateTorusMesh(float majorRadius, float minorRadius, int radialSegments, int tubularSegments) {
        if (radialSegments < 3) radialSegments = 3;
        if (tubularSegments < 3) tubularSegments = 3;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        for (int j = 0; j <= radialSegments; ++j) {
            float u = static_cast<float>(j) / radialSegments;
            float uAngle = u * 2.0f * 3.141592653589793f;
            float cosU = std::cos(uAngle);
            float sinU = std::sin(uAngle);

            for (int i = 0; i <= tubularSegments; ++i) {
                float v = static_cast<float>(i) / tubularSegments;
                float vAngle = v * 2.0f * 3.141592653589793f;
                float cosV = std::cos(vAngle);
                float sinV = std::sin(vAngle);

                glm::vec3 pos;
                pos.x = (majorRadius + minorRadius * cosV) * cosU;
                pos.y = minorRadius * sinV;
                pos.z = (majorRadius + minorRadius * cosV) * sinU;

                glm::vec3 center(majorRadius * cosU, 0.0f, majorRadius * sinU);
                glm::vec3 normal = glm::normalize(pos - center);

                vertices.push_back({pos, normal, {u, v}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}});
            }
        }

        for (int j = 1; j <= radialSegments; ++j) {
            for (int i = 1; i <= tubularSegments; ++i) {
                uint32_t a = (tubularSegments + 1) * j + i - 1;
                uint32_t b = (tubularSegments + 1) * (j - 1) + i - 1;
                uint32_t c = (tubularSegments + 1) * (j - 1) + i;
                uint32_t d = (tubularSegments + 1) * j + i;

                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(d);

                indices.push_back(b);
                indices.push_back(c);
                indices.push_back(d);
            }
        }

        return std::make_shared<Mesh>(vertices, indices);
    }

    std::shared_ptr<Mesh> AssetLoader::CreateIcosphereMesh(float radius, int subdivisions) {
        if (subdivisions < 0) subdivisions = 0;
        if (subdivisions > 5) subdivisions = 5;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        const float t = (1.0f + std::sqrt(5.0f)) / 2.0f;

        auto addVertex = [&](glm::vec3 p) -> uint32_t {
            glm::vec3 norm = glm::normalize(p);
            glm::vec3 pos = norm * radius;
            float u = 0.5f + (std::atan2(norm.z, norm.x) / (2.0f * 3.141592653589793f));
            float v = 0.5f - (std::asin(std::clamp(norm.y, -1.0f, 1.0f)) / 3.141592653589793f);
            vertices.push_back({pos, norm, {u, v}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}});
            return static_cast<uint32_t>(vertices.size() - 1);
        };

        addVertex({-1,  t,  0});
        addVertex({ 1,  t,  0});
        addVertex({-1, -t,  0});
        addVertex({ 1, -t,  0});

        addVertex({ 0, -1,  t});
        addVertex({ 0,  1,  t});
        addVertex({ 0, -1, -t});
        addVertex({ 0,  1, -t});

        addVertex({ t,  0, -1});
        addVertex({ t,  0,  1});
        addVertex({-t,  0, -1});
        addVertex({-t,  0,  1});

        std::vector<glm::uvec3> faces = {
            {0, 11, 5}, {0, 5, 1}, {0, 1, 7}, {0, 7, 10}, {0, 10, 11},
            {1, 5, 9}, {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8},
            {3, 9, 4}, {3, 4, 2}, {3, 2, 6}, {3, 6, 8}, {3, 8, 9},
            {4, 9, 5}, {2, 4, 11}, {6, 2, 10}, {8, 6, 7}, {9, 8, 1}
        };

        struct EdgeKey {
            uint32_t a, b;
            bool operator==(const EdgeKey& o) const {
                return (a == o.a && b == o.b) || (a == o.b && b == o.a);
            }
        };
        struct EdgeKeyHash {
            std::size_t operator()(const EdgeKey& k) const {
                return std::hash<uint32_t>()(std::min(k.a, k.b)) ^ (std::hash<uint32_t>()(std::max(k.a, k.b)) << 1);
            }
        };

        for (int s = 0; s < subdivisions; ++s) {
            std::unordered_map<EdgeKey, uint32_t, EdgeKeyHash> midCache;
            std::vector<glm::uvec3> nextFaces;

            auto getMid = [&](uint32_t i1, uint32_t i2) -> uint32_t {
                EdgeKey key = {i1, i2};
                auto it = midCache.find(key);
                if (it != midCache.end()) return it->second;
                glm::vec3 p1 = vertices[i1].position;
                glm::vec3 p2 = vertices[i2].position;
                glm::vec3 mid = (p1 + p2) * 0.5f;
                uint32_t midIdx = addVertex(mid);
                midCache[key] = midIdx;
                return midIdx;
            };

            for (const auto& f : faces) {
                uint32_t a = getMid(f.x, f.y);
                uint32_t b = getMid(f.y, f.z);
                uint32_t c = getMid(f.z, f.x);

                nextFaces.push_back({f.x, a, c});
                nextFaces.push_back({f.y, b, a});
                nextFaces.push_back({f.z, c, b});
                nextFaces.push_back({a, b, c});
            }
            faces = std::move(nextFaces);
        }

        for (const auto& f : faces) {
            indices.push_back(f.x);
            indices.push_back(f.y);
            indices.push_back(f.z);
        }

        return std::make_shared<Mesh>(vertices, indices);
    }

    std::shared_ptr<Mesh> AssetLoader::CreateWedgeMesh(float width, float height, float depth) {
        float hw = width * 0.5f;
        float hh = height * 0.5f;
        float hd = depth * 0.5f;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        auto addTri = [&](glm::vec3 n, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2) {
            uint32_t b = static_cast<uint32_t>(vertices.size());
            vertices.push_back({p0, n, {0.0f, 0.0f}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}});
            vertices.push_back({p1, n, {1.0f, 0.0f}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}});
            vertices.push_back({p2, n, {0.5f, 1.0f}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}});
            indices.push_back(b);
            indices.push_back(b + 1);
            indices.push_back(b + 2);
        };

        auto addQuad = [&](glm::vec3 n, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
            uint32_t b = static_cast<uint32_t>(vertices.size());
            vertices.push_back({p0, n, {0.0f, 0.0f}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}});
            vertices.push_back({p1, n, {1.0f, 0.0f}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}});
            vertices.push_back({p2, n, {1.0f, 1.0f}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}});
            vertices.push_back({p3, n, {0.0f, 1.0f}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}});
            indices.push_back(b);
            indices.push_back(b + 1);
            indices.push_back(b + 2);
            indices.push_back(b);
            indices.push_back(b + 2);
            indices.push_back(b + 3);
        };

        // Bottom
        addQuad({0.0f, -1.0f, 0.0f}, {-hw, -hh, -hd}, {hw, -hh, -hd}, {hw, -hh, hd}, {-hw, -hh, hd});
        // Back
        addQuad({0.0f, 0.0f, -1.0f}, {hw, -hh, -hd}, {-hw, -hh, -hd}, {-hw, hh, -hd}, {hw, hh, -hd});
        // Sloped top
        glm::vec3 slopeNormal = glm::normalize(glm::vec3(0.0f, depth, height));
        addQuad(slopeNormal, {-hw, -hh, hd}, {hw, -hh, hd}, {hw, hh, -hd}, {-hw, hh, -hd});
        // Left side triangle
        addTri({-1.0f, 0.0f, 0.0f}, {-hw, -hh, -hd}, {-hw, -hh, hd}, {-hw, hh, -hd});
        // Right side triangle
        addTri({1.0f, 0.0f, 0.0f}, {hw, -hh, hd}, {hw, -hh, -hd}, {hw, hh, -hd});

        return std::make_shared<Mesh>(vertices, indices);
    }

    std::shared_ptr<Mesh> AssetLoader::CreateHumanoidMesh(float scale) {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        // Smooth Contoured Cylinder/Capsule Limb Helper
        auto addCapsuleLimb = [&](glm::vec3 p0, glm::vec3 p1, float r0, float r1, int boneId, glm::vec2 uvOffset, glm::vec2 uvScale, int radialSegs = 12) {
            glm::vec3 axis = p1 - p0;
            float len = glm::length(axis);
            glm::vec3 dir = (len > 0.0001f) ? glm::normalize(axis) : glm::vec3(0, 1, 0);
            
            glm::vec3 up = (std::abs(dir.y) < 0.99f) ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
            glm::vec3 side = glm::normalize(glm::cross(dir, up));
            glm::vec3 forward = glm::normalize(glm::cross(side, dir));

            int heightRings = 4;
            uint32_t startIdx = static_cast<uint32_t>(vertices.size());

            for (int r = 0; r <= heightRings; ++r) {
                float vFactor = static_cast<float>(r) / heightRings;
                glm::vec3 ringCenter = (p0 + dir * (len * vFactor)) * scale;
                float currentRadius = (r0 + (r1 - r0) * vFactor) * scale;

                for (int s = 0; s <= radialSegs; ++s) {
                    float uFactor = static_cast<float>(s) / radialSegs;
                    float theta = uFactor * 6.2831853f;
                    glm::vec3 radialDir = side * std::cos(theta) + forward * std::sin(theta);
                    glm::vec3 pos = ringCenter + radialDir * currentRadius;
                    glm::vec3 norm = glm::normalize(radialDir);

                    glm::vec2 uv = uvOffset + glm::vec2(uFactor, vFactor) * uvScale;
                    vertices.push_back({pos, norm, uv, {boneId, -1, -1, -1}, {1.0f, 0.0f, 0.0f, 0.0f}});
                }
            }

            int stride = radialSegs + 1;
            for (int r = 0; r < heightRings; ++r) {
                for (int s = 0; s < radialSegs; ++s) {
                    uint32_t i0 = startIdx + r * stride + s;
                    uint32_t i1 = startIdx + (r + 1) * stride + s;
                    uint32_t i2 = startIdx + (r + 1) * stride + (s + 1);
                    uint32_t i3 = startIdx + r * stride + (s + 1);

                    indices.push_back(i0);
                    indices.push_back(i1);
                    indices.push_back(i2);
                    indices.push_back(i0);
                    indices.push_back(i2);
                    indices.push_back(i3);
                }
            }
        };

        // Smooth Head Sphere
        auto addSmoothHead = [&](glm::vec3 center, float radius, int boneId, glm::vec2 uvOffset, glm::vec2 uvScale, int rings = 10, int segs = 14) {
            uint32_t startIdx = static_cast<uint32_t>(vertices.size());
            glm::vec3 c = center * scale;
            float r = radius * scale;

            for (int i = 0; i <= rings; ++i) {
                float phi = 3.14159265f * (static_cast<float>(i) / rings);
                float y = std::cos(phi);
                float sinPhi = std::sin(phi);

                for (int j = 0; j <= segs; ++j) {
                    float theta = 6.2831853f * (static_cast<float>(j) / segs);
                    float x = sinPhi * std::cos(theta);
                    float z = sinPhi * std::sin(theta);

                    glm::vec3 norm(x, y, z);
                    glm::vec3 pos = c + norm * r;
                    glm::vec2 uv = uvOffset + glm::vec2(static_cast<float>(j) / segs, static_cast<float>(i) / rings) * uvScale;

                    vertices.push_back({pos, norm, uv, {boneId, -1, -1, -1}, {1.0f, 0.0f, 0.0f, 0.0f}});
                }
            }

            int stride = segs + 1;
            for (int i = 0; i < rings; ++i) {
                for (int j = 0; j < segs; ++j) {
                    uint32_t i0 = startIdx + i * stride + j;
                    uint32_t i1 = startIdx + (i + 1) * stride + j;
                    uint32_t i2 = startIdx + (i + 1) * stride + (j + 1);
                    uint32_t i3 = startIdx + i * stride + (j + 1);

                    indices.push_back(i0);
                    indices.push_back(i1);
                    indices.push_back(i2);
                    indices.push_back(i0);
                    indices.push_back(i2);
                    indices.push_back(i3);
                }
            }
        };

        // 1. Pelvis / Hips (Bone 0)
        addCapsuleLimb({0.0f, 0.88f, 0.0f}, {0.0f, 1.04f, 0.0f}, 0.17f, 0.19f, 0, {0.50f, 0.50f}, {0.50f, 0.25f});

        // 2. Muscular V-Taper Torso & Chest (Bone 1)
        addCapsuleLimb({0.0f, 1.04f, 0.0f}, {0.0f, 1.54f, 0.0f}, 0.18f, 0.24f, 1, {0.0f, 0.50f}, {0.50f, 0.50f});

        // 3. Head & Face (Bone 2)
        addSmoothHead({0.0f, 1.76f, 0.02f}, 0.16f, 2, {0.0f, 0.0f}, {0.50f, 0.50f});

        // 4. Left Arm: Shoulder/Bicep & Forearm (Bones 3 & 4)
        addCapsuleLimb({-0.24f, 1.50f, 0.0f}, {-0.34f, 1.22f, 0.0f}, 0.09f, 0.08f, 3, {0.50f, 0.0f}, {0.25f, 0.25f});
        addCapsuleLimb({-0.34f, 1.22f, 0.0f}, {-0.36f, 0.94f, 0.02f}, 0.08f, 0.07f, 4, {0.50f, 0.25f}, {0.25f, 0.25f});

        // 5. Right Arm / The God Hand: Armored Gauntlet (Bones 5 & 6)
        addCapsuleLimb({0.24f, 1.50f, 0.0f}, {0.34f, 1.22f, 0.0f}, 0.095f, 0.085f, 5, {0.75f, 0.0f}, {0.25f, 0.25f});
        addCapsuleLimb({0.34f, 1.22f, 0.0f}, {0.36f, 0.94f, 0.02f}, 0.095f, 0.085f, 6, {0.75f, 0.25f}, {0.25f, 0.25f});

        // 6. Left Leg: Thigh & Riding Boot (Bones 7 & 8)
        addCapsuleLimb({-0.12f, 0.88f, 0.0f}, {-0.13f, 0.46f, 0.0f}, 0.11f, 0.09f, 7, {0.50f, 0.75f}, {0.25f, 0.25f});
        addCapsuleLimb({-0.13f, 0.46f, 0.0f}, {-0.14f, 0.05f, 0.03f}, 0.09f, 0.08f, 8, {0.75f, 0.75f}, {0.25f, 0.25f});

        // 7. Right Leg: Thigh & Riding Boot (Bones 9 & 10)
        addCapsuleLimb({0.12f, 0.88f, 0.0f}, {0.13f, 0.46f, 0.0f}, 0.11f, 0.09f, 9, {0.50f, 0.75f}, {0.25f, 0.25f});
        addCapsuleLimb({0.13f, 0.46f, 0.0f}, {0.14f, 0.05f, 0.03f}, 0.09f, 0.08f, 10, {0.75f, 0.75f}, {0.25f, 0.25f});

        return std::make_shared<Mesh>(vertices, indices);
    }

    std::shared_ptr<Mesh> AssetLoader::CreateTerrainMesh(float width, float depth, int resolution, float heightScale) {
        if (resolution < 4) resolution = 4;
        int resX = resolution;
        int resZ = resolution;

        float hw = width * 0.5f;
        float hd = depth * 0.5f;
        float dx = width / (resX - 1);
        float dz = depth / (resZ - 1);

        auto getHeight = [&](float x, float z) -> float {
            // Multi-octave natural dunes & ripples
            float h = std::sin(x * 0.12f) * std::cos(z * 0.12f) * 0.6f;
            h += std::sin(x * 0.35f + z * 0.25f) * 0.25f;
            h += std::sin(x * 0.8f) * 0.10f;
            // Flatten arena center (radius 8.0)
            float distFromCenter = std::sqrt(x * x + z * z);
            float blend = glm::clamp((distFromCenter - 4.0f) / 8.0f, 0.0f, 1.0f);
            return h * heightScale * blend;
        };

        std::vector<Vertex> vertices;
        vertices.reserve(resX * resZ);
        std::vector<uint32_t> indices;
        indices.reserve((resX - 1) * (resZ - 1) * 6);

        for (int z = 0; z < resZ; ++z) {
            float posZ = -hd + z * dz;
            float v = static_cast<float>(z) / (resZ - 1) * (depth * 0.25f);

            for (int x = 0; x < resX; ++x) {
                float posX = -hw + x * dx;
                float u = static_cast<float>(x) / (resX - 1) * (width * 0.25f);
                float posY = getHeight(posX, posZ);

                // Compute normal via central finite differences
                float eps = 0.1f;
                float hL = getHeight(posX - eps, posZ);
                float hR = getHeight(posX + eps, posZ);
                float hD = getHeight(posX, posZ - eps);
                float hU = getHeight(posX, posZ + eps);

                glm::vec3 normal = glm::normalize(glm::vec3(hL - hR, 2.0f * eps, hD - hU));

                vertices.push_back({{posX, posY, posZ}, normal, {u, v}, {0, 0, 0, 0}, {1.0f, 0.0f, 0.0f, 0.0f}});
            }
        }

        for (int z = 0; z < resZ - 1; ++z) {
            for (int x = 0; x < resX - 1; ++x) {
                uint32_t i0 = z * resX + x;
                uint32_t i1 = z * resX + (x + 1);
                uint32_t i2 = (z + 1) * resX + x;
                uint32_t i3 = (z + 1) * resX + (x + 1);

                // CCW Triangles
                indices.push_back(i0);
                indices.push_back(i2);
                indices.push_back(i1);

                indices.push_back(i1);
                indices.push_back(i2);
                indices.push_back(i3);
            }
        }

        return std::make_shared<Mesh>(vertices, indices);
    }
}

