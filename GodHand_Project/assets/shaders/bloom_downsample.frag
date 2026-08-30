#version 330 core
layout (location = 0) out vec3 downsample;

uniform sampler2D srcTexture;
uniform vec2 srcResolution;
uniform float threshold = 1.0;
uniform int isFirstMip = 0;

in vec2 TexCoords;

void main()
{
    vec2 srcTexelSize = 1.0 / srcResolution;
    float x = srcTexelSize.x;
    float y = srcTexelSize.y;

    // Take 13 samples around current texel:
    // a - b - c
    // - d - e -
    // f - g - h
    // - i - j -
    // k - l - m

    vec3 a = texture(srcTexture, vec2(TexCoords.x - 2*x, TexCoords.y + 2*y)).rgb;
    vec3 b = texture(srcTexture, vec2(TexCoords.x,       TexCoords.y + 2*y)).rgb;
    vec3 c = texture(srcTexture, vec2(TexCoords.x + 2*x, TexCoords.y + 2*y)).rgb;

    vec3 d = texture(srcTexture, vec2(TexCoords.x - x,   TexCoords.y + y)).rgb;
    vec3 e = texture(srcTexture, vec2(TexCoords.x + x,   TexCoords.y + y)).rgb;

    vec3 f = texture(srcTexture, vec2(TexCoords.x - 2*x, TexCoords.y)).rgb;
    vec3 g = texture(srcTexture, vec2(TexCoords.x,       TexCoords.y)).rgb;
    vec3 h = texture(srcTexture, vec2(TexCoords.x + 2*x, TexCoords.y)).rgb;

    vec3 i = texture(srcTexture, vec2(TexCoords.x - x,   TexCoords.y - y)).rgb;
    vec3 j = texture(srcTexture, vec2(TexCoords.x + x,   TexCoords.y - y)).rgb;

    vec3 k = texture(srcTexture, vec2(TexCoords.x - 2*x, TexCoords.y - 2*y)).rgb;
    vec3 l = texture(srcTexture, vec2(TexCoords.x,       TexCoords.y - 2*y)).rgb;
    vec3 m = texture(srcTexture, vec2(TexCoords.x + 2*x, TexCoords.y - 2*y)).rgb;

    // Weighted distribution (Jimenez 13-tap filter with exact 1.0 energy sum):
    vec3 corner = (a + c + k + m) * 0.03125;
    vec3 edge   = (b + f + h + l) * 0.0625;
    vec3 inner  = (d + e + i + j) * 0.125;
    vec3 center = g * 0.125;
    downsample = corner + edge + inner + center;

    if (isFirstMip != 0) {
        // Karis average on first pass to kill high-energy firefly subpixels
        float luma = dot(downsample, vec3(0.2126, 0.7152, 0.0722));
        downsample /= (1.0 + luma * 0.25);

        float brightness = max(downsample.r, max(downsample.g, downsample.b));
        float soft = brightness - threshold;
        if (soft <= 0.0) {
            downsample = vec3(0.0);
        } else {
            downsample *= (soft / max(brightness, 0.0001));
        }
    }
}
