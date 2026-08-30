#version 430

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) buffer InputBuffer {
    float values[];
};

layout(std430, binding = 1) buffer OutputBuffer {
    float results[];
};

void main() {
    uint index = gl_GlobalInvocationID.x;
    results[index] = values[index] * 2.0;
}
