#version 330 core
layout (location = 0) out vec3 downsample;

uniform sampler2D srcTexture;
uniform vec2 srcResolution;

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

    // Apply weighted distribution:
    // 0.5 + 0.125 + 0.125 + 0.125 + 0.125 = 1
    // (g) is weighted 0.25, (d,e,i,j) are weighted 0.125 each, others 0.03125
    downsample = e*0.125 + d*0.125 + i*0.125 + j*0.125 + g*0.5;
    downsample += (a+b+c+f+h+k+l+m)*0.125;
    
    // Karis average to reduce fireflies
    float luma = dot(downsample, vec3(0.2126, 0.7152, 0.0722));
    downsample /= (1.0 + luma);
}
