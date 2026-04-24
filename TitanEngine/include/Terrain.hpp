// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Noise.hpp"

namespace titan {

    class Terrain {
    public:
        Terrain(int width, int depth, float scale);
        ~Terrain();

        void Render();
        float GetHeight(float x, float z) const;

    private:
        GLuint m_vao = 0, m_vbo = 0, m_ebo = 0;
        int m_indexCount = 0;
        int m_width, m_depth;
        float m_scale;
        
        Noise m_noise;

        void GenerateMesh();
    };

}
