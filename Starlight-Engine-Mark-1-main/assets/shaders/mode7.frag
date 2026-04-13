#version 410 core
in vec2 v_uv;
out vec4 FragColor;

uniform sampler2D tex;
uniform int use_texture;

// Parâmetros de Mode 7 SNES PPU
uniform float map_x;      // Posição Câmera X
uniform float map_y;      // Posição Câmera Y
uniform float map_z;      // Altura da Câmera
uniform float horizon;    // Linha do Horizonte na tela (0.0 a 1.0)
uniform float angle;      // Rotação Y (Yaw)
uniform float pitch;      // Inclinação do plano

void main() {
    // 1. Calcular a distância vertical (Y) a partir do horizonte
    float y = v_uv.y;
    
    // Se o Y estiver acima do horizonte, é "céu" (descartamos o chão do mode 7)
    if (y < horizon) {
        discard;
    }

    // 2. Inverse Perspective Mapping
    // A distância Z na textura projetada é inversa à altura na tela.
    // Quanto mais perto do horizonte (y se aproxima de horizon), mais longe Z vai para o infinito.
    float distance = map_z / ((y - horizon) * pitch);
    
    // 3. Converter coordenada X (-1 a 1 via UV centrado) para largura projetada
    float sx = (v_uv.x - 0.5) * 2.0;
    
    // 4. Calcular X e Y no plano infinito
    float sample_x = sx * distance;
    float sample_y = distance;

    // 5. Aplicar rotação 2D (Yaw) ao modo da câmera clássica
    float s = sin(angle);
    float c = cos(angle);
    float rot_x = sample_x * c - sample_y * s;
    float rot_y = sample_x * s + sample_y * c;

    // 6. Loop infinito da textura (Tiling do Mode 7)
    vec2 final_uv = vec2(rot_x + map_x, rot_y + map_y);
    
    vec4 resultColor;
    if (use_texture == 1) {
        // Usa GL_REPEAT automaticamente no OpenGL para ladrilho
        resultColor = texture(tex, final_uv);
    } else {
        // Fundo escuro e sóbrio condizente com o tema do Pong (Removido o Checkerboard Roxo)
        resultColor = vec4(0.015, 0.015, 0.04, 1.0);
        
        // Grade sutil quase invisível para profundidade
        float grid = step(0.98, fract(final_uv.x * 2.0)) + step(0.98, fract(final_uv.y * 2.0));
        resultColor += vec4(0.02, 0.02, 0.03, 0.0) * grid;
    }
    
    // Efeito de Fog de profundidade (esconde o serrilhado no horizonte infinito)
    float fog = clamp((distance) / 10.0, 0.0, 1.0);
    resultColor = mix(resultColor, vec4(0.01, 0.01, 0.03, 1.0), fog); // Mistura com a cor de fundo do Pong

    FragColor = resultColor;
}
