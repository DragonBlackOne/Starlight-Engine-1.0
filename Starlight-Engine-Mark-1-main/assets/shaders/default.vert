#version 330 core

// Atributos do modelo (o cubo em si)
in vec3 in_position;
in vec3 in_normal;
in vec2 in_texcoord;

// Atributos da Instância (dados únicos para cada cópia do cubo)
in vec3 in_instance_pos;

// Matrizes globais
uniform mat4 m_proj;
uniform mat4 m_view;
uniform mat4 u_light_mvp;

out vec2 v_texcoord;
out vec3 v_normal;
out vec4 v_shadow_coord;

void main() {
    // Posição final = Projeção * Câmera * (Posição do Vértice + Posição da Instância)
    vec3 final_pos = in_position + in_instance_pos;
    gl_Position = m_proj * m_view * vec4(final_pos, 1.0);
    
    // Calcula coordenadas de sombra (Espaço da Luz)
    // Bias matrix para converter [-1, 1] -> [0, 1]
    mat4 biasMatrix = mat4(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    );
    v_shadow_coord = biasMatrix * u_light_mvp * vec4(final_pos, 1.0);
    
    v_texcoord = in_texcoord;
    v_normal = in_normal;
}
