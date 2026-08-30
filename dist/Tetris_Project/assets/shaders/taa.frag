#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D currentTexture;
uniform sampler2D historyTexture;
uniform sampler2D gPosition;
uniform sampler2D gNormal;

uniform mat4 prevViewProj;
uniform vec2 texelSize;
uniform int firstFrame;

void main()
{
    vec3 currentColor = texture(currentTexture, TexCoords).rgb;

    if (firstFrame == 1) {
        FragColor = vec4(currentColor, 1.0);
        return;
    }

    vec3 worldPos = texture(gPosition, TexCoords).rgb;
    vec3 normal = texture(gNormal, TexCoords).rgb;

    vec2 prevTexCoords = TexCoords;
    bool isSkybox = (length(normal) < 0.0001);

    if (!isSkybox) {
        vec4 prevClip = prevViewProj * vec4(worldPos, 1.0);
        if (prevClip.w > 0.0) {
            prevTexCoords = (prevClip.xy / prevClip.w) * 0.5 + 0.5;
        }
    }

    // Out of screen fallback
    if (prevTexCoords.x < 0.0 || prevTexCoords.x > 1.0 || prevTexCoords.y < 0.0 || prevTexCoords.y > 1.0) {
        FragColor = vec4(currentColor, 1.0);
        return;
    }

    // Neighborhood clamping to eliminate ghosting artifacts
    vec3 minColor = vec3(9999.0);
    vec3 maxColor = vec3(-9999.0);

    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            vec3 c = texture(currentTexture, TexCoords + vec2(x, y) * texelSize).rgb;
            minColor = min(minColor, c);
            maxColor = max(maxColor, c);
        }
    }

    vec3 historyColor = texture(historyTexture, prevTexCoords).rgb;
    historyColor = clamp(historyColor, minColor, maxColor);

    // Dynamic blend weight: 90% history / 10% current frame for high temporal stability
    vec3 blended = mix(historyColor, currentColor, 0.1);
    FragColor = vec4(blended, 1.0);
}
