// Water Shader

struct Uniforms {
    view_proj: mat4x4<f32>,
    model: mat4x4<f32>,
    light_view_proj: mat4x4<f32>,
    camera_pos: vec4<f32>,
    time: f32,
    wind_strength: f32,
    _padding: vec2<f32>,
};

struct LightUniform {
    direction: vec3<f32>,
    fog_density: f32,
    color: vec3<f32>,
    ambient: f32,
    sun_intensity: f32,
    _pad0: f32,
    _pad1: f32,
    _pad2: f32,
    point_lights: array<vec4<f32>, 4>, // Simplified since we don't use point lights on water much yet
    point_light_count: u32,
    fog_r: f32,
    fog_g: f32,
    fog_b: f32,
    _padding_end: vec4<f32>,
};

// Group 0: Scene Globals
@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var<uniform> light: LightUniform;
@group(0) @binding(2) var t_shadow: texture_depth_2d;
@group(0) @binding(3) var s_shadow: sampler_comparison;

// Group 1: Material (Water Normal Map)
struct MaterialUniform {
    metallic: f32,
    roughness: f32,
    _padding: vec2<f32>,
};

@group(1) @binding(0) var t_diffuse: texture_2d<f32>; // Not used much, maybe foam?
@group(1) @binding(1) var s_diffuse: sampler;
@group(1) @binding(2) var t_normal: texture_2d<f32>;
@group(1) @binding(3) var s_normal: sampler;
@group(1) @binding(4) var<uniform> material: MaterialUniform;

// Group 2: Skybox
@group(2) @binding(0) var t_skybox: texture_cube<f32>;
@group(2) @binding(1) var s_skybox: sampler;

struct VertexInput {
    @location(0) position: vec3<f32>,
    @location(1) normal: vec3<f32>,
    @location(2) color: vec3<f32>,
    @location(3) tex_coords: vec2<f32>,
};

struct VertexOutput {
    @builtin(position) clip_position: vec4<f32>,
    @location(0) world_normal: vec3<f32>,
    @location(1) color: vec3<f32>,
    @location(2) tex_coords: vec2<f32>,
    @location(3) shadow_pos: vec3<f32>,
    @location(4) world_pos: vec4<f32>,
    @location(5) instance_color: vec4<f32>,
};

struct InstanceInput {
    @location(6) model_matrix_0: vec4<f32>,
    @location(7) model_matrix_1: vec4<f32>,
    @location(8) model_matrix_2: vec4<f32>,
    @location(9) model_matrix_3: vec4<f32>,
    @location(10) instance_color: vec4<f32>,
};

@vertex
fn vs_main(model: VertexInput, instance: InstanceInput) -> VertexOutput {
    var out: VertexOutput;
    
    let model_matrix = mat4x4<f32>(
        instance.model_matrix_0,
        instance.model_matrix_1,
        instance.model_matrix_2,
        instance.model_matrix_3
    );

    let world_pos = model_matrix * vec4<f32>(model.position, 1.0);
    
    // Water Wave Animation
    let time = uniforms.time * 0.5;
    
    // Simple Gerstner-like waves (approximated with sine sums)
    let wave1 = sin(world_pos.x * 0.5 + time) * 0.2;
    let wave2 = cos(world_pos.z * 0.4 + time * 0.8) * 0.2;
    let wave3 = sin((world_pos.x + world_pos.z) * 0.2 + time * 1.2) * 0.1;
    
    let wave_height = wave1 + wave2 + wave3;
    
    let world_pos_animated = world_pos + vec4<f32>(0.0, wave_height, 0.0, 0.0);
    
    out.world_pos = world_pos_animated;
    out.clip_position = uniforms.view_proj * world_pos_animated;
    
    // Recalculate normal based on wave derivatives (simplified)
    // dH/dx = 0.5*cos(...) ...
    // For now, keep original normal (usually up) and let normal map handle small details
    // But we should lean the normal a bit based on wave slope for large waves
    var n = vec3<f32>(0.0, 1.0, 0.0);
    // Approximation of wave slope
    n.x = -(cos(world_pos.x * 0.5 + time) * 0.1 + cos((world_pos.x + world_pos.z) * 0.2 + time * 1.2) * 0.02);
    n.z = -(sin(world_pos.z * 0.4 + time * 0.8) * 0.08 + cos((world_pos.x + world_pos.z) * 0.2 + time * 1.2) * 0.02);
    n = normalize(n);
    
    out.world_normal = n; // Ignore model normal for water plane
    out.color = model.color;
    out.tex_coords = model.tex_coords * 10.0; // Tiling
    out.instance_color = instance.instance_color;
    
    // Shadow pos
    let pos_from_light = uniforms.light_view_proj * world_pos_animated;
    out.shadow_pos = vec3<f32>(
        pos_from_light.x * 0.5 + 0.5,
        -pos_from_light.y * 0.5 + 0.5,
        pos_from_light.z
    );
    
    return out;
}

const PI: f32 = 3.14159265359;

fn fresnel_schlick(cosTheta: f32, F0: vec3<f32>) -> vec3<f32> {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
    let time = uniforms.time;
    
    // Scroll Normal Maps
    let uv1 = in.tex_coords + vec2<f32>(time * 0.02, time * 0.01);
    let uv2 = in.tex_coords * 0.5 + vec2<f32>(-time * 0.01, time * 0.02);
    
    let n1 = textureSample(t_normal, s_normal, uv1).rgb * 2.0 - 1.0;
    let n2 = textureSample(t_normal, s_normal, uv2).rgb * 2.0 - 1.0;
    
    let normal_tangent = normalize(n1 + n2);
    
    // TBN Matrix (assuming flat plane facing Up)
    let N_geom = normalize(in.world_normal);
    let T = normalize(vec3<f32>(1.0, 0.0, 0.0)); // Tangent along X
    let B = normalize(vec3<f32>(0.0, 0.0, 1.0)); // Bitangent along Z
    
    // Adjust TBN to match wave normal
    // If N_geom is tilted, T and B should be orthogonal.
    // Use proper Gramm-Schmidt if needed, but for small waves this is usually okay-ish or we construct from derivatives
    
    let N = normalize(normal_tangent.x * T + normal_tangent.y * B + normal_tangent.z * N_geom);
    
    // View Vector
    let cam_pos = uniforms.camera_pos.xyz;
    let V = normalize(cam_pos - in.world_pos.xyz);
    
    // Sun
    let L = normalize(-light.direction);
    let H = normalize(V + L);
    
    // Simple Water Color
    let deep_color = vec3<f32>(0.0, 0.1, 0.3);
    let shallow_color = vec3<f32>(0.0, 0.4, 0.6);
    
    let fresnel = fresnel_schlick(max(dot(N, V), 0.0), vec3<f32>(0.02));
    
    // Specular (Sun)
    let NdotH = max(dot(N, H), 0.0);
    let specular = pow(NdotH, 500.0) * light.sun_intensity * 2.0;
    
    // Environment Reflection
    let R = reflect(-V, N);
    let env_reflection = textureSample(t_skybox, s_skybox, R).rgb;
    
    // Mix Colors
    var color = mix(deep_color, shallow_color, 0.5); // Base
    color = mix(color, env_reflection, fresnel * 0.8 + 0.2); // Reflections
    color += vec3<f32>(specular);
    
    // Foam (simple procedural based on height/normal)
    let foam = smoothstep(0.8, 1.0, 1.0 - in.world_normal.y); // White caps on steep waves
    color += vec3<f32>(foam);
    
    // Alpha
    let alpha = clamp(0.6 + fresnel.x * 0.4, 0.0, 1.0);
    
    // Fog
    let fog_color = vec3<f32>(light.fog_r, light.fog_g, light.fog_b);
    let fog_density = light.fog_density;
    if (fog_density > 0.0) {
        let distance = length(cam_pos - in.world_pos.xyz);
        let fog_factor = exp(-fog_density * distance);
        color = mix(fog_color, color, clamp(fog_factor, 0.0, 1.0));
    }
    
    // Tone Mapping (Standard)
    let exposure = 1.0;
    let mapped = vec3<f32>(1.0) - exp(-color * exposure);
    let gamma = 2.2;
    let final_color = pow(mapped, vec3<f32>(1.0 / gamma));

    return vec4<f32>(final_color, alpha * in.instance_color.a);
}
