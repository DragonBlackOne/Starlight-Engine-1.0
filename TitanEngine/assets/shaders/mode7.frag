#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D tex;
uniform float map_x;
uniform float map_y;
uniform float map_z;
uniform float horizon;
uniform float angle;
uniform float pitch;

void main()
{
    // Mode 7 Perspective floor projection logic
    float y = TexCoords.y;
    if (y < horizon) {
        discard; // Above horizon
    }

    float py = y - horizon;
    float space_z = map_z / py;
    
    float cos_a = cos(angle);
    float sin_a = sin(angle);
    
    // Calculate world coordinates from screen coordinates
    float screen_x = (TexCoords.x - 0.5) * pitch;
    float world_x = map_x + space_z * (screen_x * cos_a - sin_a);
    float world_y = map_y + space_z * (screen_x * sin_a + cos_a);
    
    // Texture sampling with repeat
    vec2 uv = vec2(world_x, world_y);
    FragColor = texture(tex, uv);
}
