#pragma once
#include <GL/glew.h>
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "meshoptimizer/src/meshoptimizer.h"

namespace Vamos {
    struct Mesh {
        std::vector<float> vertices; // Formato: x, y, z, u, v
        std::vector<unsigned int> indices;
        
        // Skeletal Animation Data
        struct BoneData {
            unsigned int ids[4] = {0, 0, 0, 0};
            float weights[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            void add(unsigned int id, float weight) {
                for (int i = 0; i < 4; i++) {
                    if (weights[i] == 0.0f) {
                        ids[i] = id;
                        weights[i] = weight;
                        return;
                    }
                }
            }
        };
        std::vector<BoneData> boneData;
        
        // LODs
        std::vector<std::vector<unsigned int>> lods;
        
        unsigned int vao = 0, vbo = 0, ebo = 0, boneVbo = 0;
        unsigned int lodEbos[5] = {0, 0, 0, 0, 0};
        
        size_t vertexCount = 0;
        size_t indexCount = 0;
        bool initialized = false;

        void LoadModel(const std::string& path) {
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_LimitBoneWeights);

            if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                return;
            }

            vertices.clear();
            indices.clear();
            boneData.clear();
            aiMesh* mesh = scene->mMeshes[0];
            
            boneData.resize(mesh->mNumVertices);

            for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
                vertices.push_back(mesh->mVertices[i].x);
                vertices.push_back(mesh->mVertices[i].y);
                vertices.push_back(mesh->mVertices[i].z);
                if(mesh->mTextureCoords[0]) {
                    vertices.push_back(mesh->mTextureCoords[0][i].x);
                    vertices.push_back(mesh->mTextureCoords[0][i].y);
                } else {
                    vertices.push_back(0.0f); vertices.push_back(0.0f);
                }
            }

            // Extract Bone Data
            for (unsigned int i = 0; i < mesh->mNumBones; i++) {
                aiBone* bone = mesh->mBones[i];
                for (unsigned int j = 0; j < bone->mNumWeights; j++) {
                    unsigned int vertexID = bone->mWeights[j].mVertexId;
                    float weight = bone->mWeights[j].mWeight;
                    if (vertexID < boneData.size()) {
                        boneData[vertexID].add(i, weight);
                    }
                }
            }

            for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
                aiFace face = mesh->mFaces[i];
                for(unsigned int j = 0; j < face.mNumIndices; j++)
                    indices.push_back(face.mIndices[j]);
            }

            vertexCount = mesh->mNumVertices;
            indexCount = indices.size();
            initialized = false;
            
            GenerateLODs();
        }

        void GenerateLODs() {
            if (indices.empty()) return;
            lods.clear();
            lods.push_back(indices); // LOD 0

            float threshold = 0.5f;
            for (int i = 1; i < 4; i++) {
                size_t target_index_count = size_t(indices.size() * threshold);
                float target_error = 0.01f * i;
                
                std::vector<unsigned int> lod(indices.size());
                size_t lod_size = meshopt_simplify(lod.data(), indices.data(), indices.size(), &vertices[0], vertexCount, sizeof(float) * 5, target_index_count, target_error, 0, nullptr);
                lod.resize(lod_size);
                lods.push_back(lod);
                threshold *= 0.5f;
            }
        }
    };
}
