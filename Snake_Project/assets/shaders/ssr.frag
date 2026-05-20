#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D sceneTexture;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 camPos;

const float step = 0.1;
const float minRayStep = 0.1;
const int maxSteps = 50;
const int numBinarySearchSteps = 5;
const float reflectionThreshold = 0.5;

vec3 PositionFromDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    vec4 clipSpacePosition = vec4(TexCoords * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = inverse(projection) * clipSpacePosition;
    viewSpacePosition /= viewSpacePosition.w;
    return viewSpacePosition.xyz;
}

vec2 BinarySearch(vec3 dir, inout vec3 hitCoord, out float dDepth) {
    for (int i = 0; i < numBinarySearchSteps; i++) {
        vec4 projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

        float depth = texture(gPosition, projectedCoord.xy).z;
        dDepth = hitCoord.z - depth;

        dir *= 0.5;
        if (dDepth > 0.0)
            hitCoord += dir;
        else
            hitCoord -= dir;
    }

    vec4 projectedCoord = projection * vec4(hitCoord, 1.0);
    projectedCoord.xy /= projectedCoord.w;
    projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
    return projectedCoord.xy;
}

vec2 RayCast(vec3 dir, inout vec3 hitCoord, out float dDepth) {
    dir *= step;
    for (int i = 0; i < maxSteps; i++) {
        hitCoord += dir;

        vec4 projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

        float depth = texture(gPosition, projectedCoord.xy).z;
        dDepth = hitCoord.z - depth;

        if (dDepth <= 0.0)
            return BinarySearch(dir, hitCoord, dDepth);
    }
    return vec2(-1.0);
}

void main() {
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 worldPos = texture(gPosition, TexCoords).rgb;
    float metallic = texture(gAlbedoSpec, TexCoords).a;
    
    if (metallic < 0.01) {
        FragColor = vec4(0.0);
        return;
    }

    vec3 viewPos = (view * vec4(worldPos, 1.0)).xyz;
    vec3 viewNormal = (view * vec4(normal, 0.0)).xyz;
    vec3 viewDir = normalize(viewPos);
    vec3 reflectDir = normalize(reflect(viewDir, viewNormal));

    vec3 hitPos = viewPos;
    float dDepth;
    vec2 coords = RayCast(reflectDir, hitPos, dDepth);

    if (coords != vec2(-1.0)) {
        vec3 reflectedColor = texture(sceneTexture, coords).rgb;
        FragColor = vec4(reflectedColor * metallic, 1.0);
    } else {
        FragColor = vec4(0.0);
    }
}
