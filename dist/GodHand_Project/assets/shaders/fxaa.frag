#version 330 core

in vec2 v_texcoord;
out vec4 f_color;

uniform sampler2D u_texture;
uniform vec2 u_resolution;
uniform float u_quality;  // 0.0 = low, 0.5 = medium, 1.0 = high

#define FXAA_REDUCE_MIN   (1.0/128.0)
#define FXAA_REDUCE_MUL   (1.0/8.0)
#define FXAA_SPAN_MAX     8.0

vec3 fxaa(sampler2D tex, vec2 fragCoord, vec2 resolution) {
    vec2 inverseVP = 1.0 / resolution;
    vec3 rgbNW = texture(tex, fragCoord + vec2(-1.0, -1.0) * inverseVP).rgb;
    vec3 rgbNE = texture(tex, fragCoord + vec2(1.0, -1.0) * inverseVP).rgb;
    vec3 rgbSW = texture(tex, fragCoord + vec2(-1.0, 1.0) * inverseVP).rgb;
    vec3 rgbSE = texture(tex, fragCoord + vec2(1.0, 1.0) * inverseVP).rgb;
    vec3 rgbM  = texture(tex, fragCoord).rgb;
    
    // Luma calculation
    vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgbM,  luma);
    
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
    
    // Early exit if no edge detected
    if (lumaMax - lumaMin < lumaMax * 0.125) {
        return rgbM;
    }
    
    // Edge direction
    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
    
    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    
    dir = min(vec2(FXAA_SPAN_MAX), max(vec2(-FXAA_SPAN_MAX), dir * rcpDirMin)) * inverseVP;
    
    // Sample along edge direction
    vec3 rgbA = 0.5 * (
        texture(tex, fragCoord + dir * (1.0/3.0 - 0.5)).rgb +
        texture(tex, fragCoord + dir * (2.0/3.0 - 0.5)).rgb
    );
    
    vec3 rgbB = rgbA * 0.5 + 0.25 * (
        texture(tex, fragCoord + dir * -0.5).rgb +
        texture(tex, fragCoord + dir * 0.5).rgb
    );
    
    float lumaB = dot(rgbB, luma);
    
    if (lumaB < lumaMin || lumaB > lumaMax) {
        return rgbA;
    } else {
        return rgbB;
    }
}

void main() {
    vec3 color = fxaa(u_texture, v_texcoord, u_resolution);
    f_color = vec4(color, 1.0);
}
