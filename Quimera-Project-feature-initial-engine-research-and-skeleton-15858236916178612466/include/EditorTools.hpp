// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include "MeshComponent.hpp"
#include "TransformComponent.hpp"
#include "FastNoiseLite/Cpp/FastNoiseLite.h"
#include <vector>

namespace Vamos {
    class EditorTools {
    public:
        static void GenerateTerrain(Mesh& mesh, int width, int height, float scale, float strength) {
            FastNoiseLite noise;
            noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
            
            mesh.vertices.clear();
            mesh.indices.clear();

            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    float noiseValue = noise.GetNoise((float)x * scale, (float)y * scale);
                    float h = noiseValue * strength;

                    // Posição
                    mesh.vertices.push_back((float)x - width/2.0f);
                    mesh.vertices.push_back(h);
                    mesh.vertices.push_back((float)y - height/2.0f);

                    // UVs
                    mesh.vertices.push_back((float)x / (float)width);
                    mesh.vertices.push_back((float)y / (float)height);
                }
            }

            for (int y = 0; y < height - 1; y++) {
                for (int x = 0; x < width - 1; x++) {
                    int root = y * width + x;
                    mesh.indices.push_back(root);
                    mesh.indices.push_back(root + width);
                    mesh.indices.push_back(root + 1);

                    mesh.indices.push_back(root + 1);
                    mesh.indices.push_back(root + width);
                    mesh.indices.push_back(root + width + 1);
                }
            }
            mesh.vertexCount = (unsigned int)(mesh.vertices.size() / 5);
            mesh.indexCount = (unsigned int)mesh.indices.size();
        }

        static void CreateCube(Mesh& mesh) {
            mesh.vertices = {
                // Front (Pos, UV)
                -0.5f, -0.5f,  0.5f, 0, 0,  0.5f, -0.5f,  0.5f, 1, 0,  0.5f,  0.5f,  0.5f, 1, 1, -0.5f,  0.5f,  0.5f, 0, 1,
                // Back
                -0.5f, -0.5f, -0.5f, 0, 0,  0.5f, -0.5f, -0.5f, 1, 0,  0.5f,  0.5f, -0.5f, 1, 1, -0.5f,  0.5f, -0.5f, 0, 1,
                // Left
                -0.5f,  0.5f,  0.5f, 1, 0, -0.5f,  0.5f, -0.5f, 1, 1, -0.5f, -0.5f, -0.5f, 0, 1, -0.5f, -0.5f,  0.5f, 0, 0,
                // Right
                 0.5f,  0.5f,  0.5f, 1, 0,  0.5f,  0.5f, -0.5f, 1, 1,  0.5f, -0.5f, -0.5f, 0, 1,  0.5f, -0.5f,  0.5f, 0, 0,
                // Top
                -0.5f,  0.5f, -0.5f, 0, 1,  0.5f,  0.5f, -0.5f, 1, 1,  0.5f,  0.5f,  0.5f, 1, 0, -0.5f,  0.5f,  0.5f, 0, 0,
                // Bottom
                -0.5f, -0.5f, -0.5f, 0, 1,  0.5f, -0.5f, -0.5f, 1, 1,  0.5f, -0.5f,  0.5f, 1, 0, -0.5f, -0.5f,  0.5f, 0, 0
            };
            mesh.indices = {
                0,1,2, 2,3,0, 4,5,6, 6,7,4, 8,9,10, 10,11,8, 12,13,14, 14,15,12, 16,17,18, 18,19,16, 20,21,22, 22,23,20
            };
            mesh.vertexCount = 24;
            mesh.indexCount = 36;
            mesh.initialized = false;
        }

        static void CreateSphere(Mesh& mesh, int sectors = 20, int stacks = 20) {
            mesh.vertices.clear();
            mesh.indices.clear();
            float x, y, z, xy;
            float nx, ny, nz, s, t;

            float sectorStep = 2.0f * 3.14159f / sectors;
            float stackStep = 3.14159f / stacks;
            float sectorAngle, stackAngle;

            for(int i = 0; i <= stacks; ++i) {
                stackAngle = 3.14159f / 2 - i * stackStep;
                xy = cosf(stackAngle);
                z = sinf(stackAngle);

                for(int j = 0; j <= sectors; ++j) {
                    sectorAngle = j * sectorStep;

                    x = xy * cosf(sectorAngle);
                    y = xy * sinf(sectorAngle);
                    mesh.vertices.push_back(x * 0.5f);
                    mesh.vertices.push_back(y * 0.5f);
                    mesh.vertices.push_back(z * 0.5f);

                    s = (float)j / sectors;
                    t = (float)i / stacks;
                    mesh.vertices.push_back(s);
                    mesh.vertices.push_back(t);
                }
            }

            int k1, k2;
            for(int i = 0; i < stacks; ++i) {
                k1 = i * (sectors + 1);
                k2 = k1 + sectors + 1;
                for(int j = 0; j < sectors; ++j, ++k1, ++k2) {
                    if(i != 0) {
                        mesh.indices.push_back(k1);
                        mesh.indices.push_back(k2);
                        mesh.indices.push_back(k1 + 1);
                    }
                    if(i != (stacks - 1)) {
                        mesh.indices.push_back(k1 + 1);
                        mesh.indices.push_back(k2);
                        mesh.indices.push_back(k2 + 1);
                    }
                }
            }
            mesh.vertexCount = (unsigned int)(mesh.vertices.size() / 5);
            mesh.indexCount = (unsigned int)mesh.indices.size();
            mesh.initialized = false;
        }

        static void CreateFloor(Mesh& mesh, float size = 100.0f) {
            float half = size * 0.5f;
            mesh.vertices = {
                -half, 0.0f, -half, 0.0f, 0.0f,
                 half, 0.0f, -half, 10.0f, 0.0f,
                 half, 0.0f,  half, 10.0f, 10.0f,
                -half, 0.0f,  half, 0.0f, 10.0f
            };
            mesh.indices = { 0, 1, 2, 2, 3, 0 };
            mesh.vertexCount = 4;
            mesh.indexCount = 6;
            mesh.initialized = false;
        }

    };
}
