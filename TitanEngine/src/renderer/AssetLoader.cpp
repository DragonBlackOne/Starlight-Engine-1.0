#include "AssetLoader.hpp"
#include "Log.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"

#include <vector>
#include <fstream>
#include <glad/glad.h>

namespace titan {
    // Callback para tinyobj
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

        auto mesh = std::make_shared<Mesh>(vertices, indices);

        tinyobj_attrib_free(&attrib);
        if (shapes) tinyobj_shapes_free(shapes, num_shapes);
        if (materials) tinyobj_materials_free(materials, num_materials);

        return mesh;
    }

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
