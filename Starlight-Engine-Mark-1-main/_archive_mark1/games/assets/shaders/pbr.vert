#version 430 core
in vec3 in_position;
in vec3 in_normal;
in vec2 in_uv;
in vec3 in_tangent;
in vec4 in_joints;
in vec4 in_weights;
in mat4 in_instance_mat;

out vec3 v_normal;
out vec3 v_frag_pos;
out vec2 v_uv;
out mat3 v_tbn;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

uniform mat4 u_joint_mat[64]; // Joint Palette
uniform bool u_use_skinning;
uniform bool u_is_instanced;
uniform bool u_is_boids;

struct Boid {
    vec4 position;
    vec4 velocity;
};

layout(std430, binding = 0) buffer BoidBuffer {
    Boid boids[];
};

uniform float u_log_c = 1.0;
uniform float u_log_far = 10000.0;

void main() {
    mat4 model_mat;
    if (u_use_skinning) {
        mat4 skin_mat = 
            in_weights.x * u_joint_mat[int(in_joints.x)] +
            in_weights.y * u_joint_mat[int(in_joints.y)] +
            in_weights.z * u_joint_mat[int(in_joints.z)] +
            in_weights.w * u_joint_mat[int(in_joints.w)];
        model_mat = u_model * skin_mat;
    } else if (u_is_boids) {
        vec4 b_pos = boids[gl_InstanceID].position;
        vec3 b_vel = boids[gl_InstanceID].velocity.xyz;
        
        // Face velocity
        vec3 f = normalize(b_vel + vec3(0.0, 0.0001, 0.0));
        vec3 r = normalize(cross(vec3(0, 1, 0), f));
        vec3 u = cross(f, r);
        mat4 b_rot = mat4(
            vec4(r, 0.0),
            vec4(u, 0.0),
            vec4(f, 0.0),
            vec4(0.0, 0.0, 0.0, 1.0)
        );
        
        model_mat = u_model;
        model_mat[3] = vec4(b_pos.xyz, 1.0);
        model_mat = model_mat * b_rot;
    } else if (u_is_instanced) {
        model_mat = in_instance_mat;
    } else {
        model_mat = u_model;
    }

    vec4 world_pos = model_mat * vec4(in_position, 1.0);
    mat3 normal_mat = transpose(inverse(mat3(model_mat)));

    gl_Position = u_projection * u_view * world_pos;
    
    // Logarithmic Depth
    float w = gl_Position.w;
    gl_Position.z = (2.0 * log2(u_log_c * w + 1.0) / log2(u_log_c * u_log_far + 1.0) - 1.0) * w;

    v_frag_pos = world_pos.xyz;
    v_normal = normalize(normal_mat * in_normal);
    v_uv = in_uv;

    // TBN Matrix for Normal Mapping
    vec3 T = normalize(normal_mat * in_tangent);
    vec3 N = v_normal;
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    v_tbn = mat3(T, B, N);
}