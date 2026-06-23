#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Noise.hpp"

namespace starlight {

    class Terrain {
    public:
        Terrain(int width, int depth, float scale);
        ~Terrain();

        void Render();
        float GetHeight(float x, float z) const;

    private:
        GLuint m_vao = 0, m_vbo = 0, m_ebo = 0;
        int m_indexCount = 0;
        int m_width = 0, m_depth = 0;
        float m_scale = 1.0f;
        
        Noise m_noise;

        void GenerateMesh();
    };

}
