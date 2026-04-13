// Vertex shader with Lighting and Normal Mapping

struct Uniforms {
    view_proj: mat4x4<f32>,
    model: mat4x4<f32>,
    light_view_proj: mat4x4<f32>,
    camera_pos: vec4<f32>,
};

struct PointLight {
    position: vec4<f32>, 
    color: vec4<f32>,
};

struct LightUniform {
    direction: vec3<f32>,
    fog_density: f32,
    color: vec3<f32>,
    shadow_matrices: array<mat4x4<f32>, 4>,
    point_light_count: u32,
    fog_r: f32,
    fog_g: f32,
    fog_b: f32,
};

struct PointLightData {
    lights: array<PointLight>,
};

// Group 0: Scene Globals
@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(0) @binding(1) var<uniform> light: LightUniform;
@group(0) @binding(2) var t_shadow: texture_depth_2d_array;
@group(0) @binding(3) var s_shadow: sampler_comparison;
@group(0) @binding(4) var<storage, read> point_lights_data: PointLightData;

// Group 1: Material
struct MaterialUniform {
    metallic: f32,
    roughness: f32,
    _padding: vec2<f32>,
};

@group(1) @binding(0) var t_diffuse: texture_2d<f32>;
@group(1) @binding(1) var s_diffuse: sampler;
@group(1) @binding(2) var t_normal: texture_2d<f32>;
@group(1) @binding(3) var s_normal: sampler;
@group(1) @binding(4) var<uniform> material: MaterialUniform;

// Group 2: Skybox (Only used in skybox pass)
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
    out.world_pos = world_pos;
    out.clip_position = uniforms.view_proj * world_pos;
    
    // Transform normal
    let normal_matrix = mat3x3<f32>(
        model_matrix[0].xyz,
        model_matrix[1].xyz,
        model_matrix[2].xyz
    );
    out.world_normal = normalize(normal_matrix * model.normal);
    out.color = model.color;
    out.tex_coords = model.tex_coords;
    out.instance_color = instance.instance_color;
    
    // Project world position to light space
    let pos_from_light = uniforms.light_view_proj * world_pos;
    
    out.shadow_pos = vec3<f32>(
        pos_from_light.x * 0.5 + 0.5,
        -pos_from_light.y * 0.5 + 0.5,
        pos_from_light.z
    );
    
    return out;
}

const PI: f32 = 3.14159265359;

fn distribution_ggx(N: vec3<f32>, H: vec3<f32>, roughness: f32) -> f32 {
    let a = roughness * roughness;
    let a2 = a * a;
    let NdotH = max(dot(N, H), 0.0);
    let NdotH2 = NdotH * NdotH;
    
    let nom = a2;
    let denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return nom / (PI * denom * denom);
}

fn geometry_schlick_ggx(NdotV: f32, roughness: f32) -> f32 {
    let r = (roughness + 1.0);
    let k = (r * r) / 8.0;

    let nom = NdotV;
    let denom = NdotV * (1.0 - k) + k;
    return nom / denom;
}

fn geometry_smith(N: vec3<f32>, V: vec3<f32>, L: vec3<f32>, roughness: f32) -> f32 {
    let NdotV = max(dot(N, V), 0.0);
    let NdotL = max(dot(N, L), 0.0);
    let ggx2 = geometry_schlick_ggx(NdotV, roughness);
    let ggx1 = geometry_schlick_ggx(NdotL, roughness);
    return ggx1 * ggx2;
}

fn fresnel_schlick(cosTheta: f32, F0: vec3<f32>) -> vec3<f32> {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

fn calculate_pbr(
    N: vec3<f32>, 
    V: vec3<f32>, 
    L: vec3<f32>, 
    radiance: vec3<f32>, 
    albedo: vec3<f32>, 
    metallic: f32, 
    roughness: f32, 
    F0: vec3<f32>
) -> vec3<f32> {
    let H = normalize(V + L);
    
    // Dot products
    let NdotL = max(dot(N, L), 0.0);
    if (NdotL <= 0.0) {
        return vec3<f32>(0.0);
    }
    
    let NdotV = max(dot(N, V), 0.0);
    let HdotV = max(dot(H, V), 0.0);
    
    let D = distribution_ggx(N, H, roughness);
    let G = geometry_smith(N, V, L, roughness);
    let F = fresnel_schlick(HdotV, F0);
    
    let numerator = D * G * F;
    let denominator = 4.0 * NdotV * NdotL + 0.0001;
    let specular = numerator / denominator;
    
    let kS = F;
    var kD = vec3<f32>(1.0) - kS;
    kD = kD * (1.0 - metallic);
    
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

fn perturb_normal(N: vec3<f32>, p: vec3<f32>, tex_coords: vec2<f32>) -> vec3<f32> {
    let normal_map = textureSample(t_normal, s_normal, tex_coords).rgb;
    // Check if normal map is present (blue > 0.5 usually).
    // Since we bind a default "flat" normal (0.5, 0.5, 1.0) if missing, this is safe.
    let tangent_normal = normal_map * 2.0 - 1.0;

    let q1 = dpdx(p);
    let q2 = dpdy(p);
    let st1 = dpdx(tex_coords);
    let st2 = dpdy(tex_coords);

    let n = normalize(N);
    let t = normalize(q1 * st2.y - q2 * st1.y);
    let b = -normalize(cross(n, t));
    let tbn = mat3x3<f32>(t, b, n);

    return normalize(tbn * tangent_normal);
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
    // 1. Base color and Texture
    let tex_color = textureSample(t_diffuse, s_diffuse, in.tex_coords);
    let albedo = tex_color.rgb * in.color * in.instance_color.rgb;
    
    // 2. Material Properties
    let metallic = material.metallic;
    let roughness = material.roughness;
    
    // 3. Normal Mapping
    // Use derivative-based normal mapping
    let N = perturb_normal(in.world_normal, in.world_pos.xyz, in.tex_coords);

    // 4. View Vector
    let cam_pos = uniforms.camera_pos.xyz; 
    let V = normalize(cam_pos - in.world_pos.xyz);
    
    // Base Reflectivity F0
    var F0 = vec3<f32>(0.04); 
    F0 = mix(F0, albedo, metallic);
    
    var color = vec3<f32>(0.0);
    
    // --- Directional Light (Sun) ---
    {
        let L = normalize(-light.direction);
        let radiance = light.color; 
        
        // PCF Soft Shadows (5x5 Kernel for smoothness)
        var shadow = 0.0;
        let texel_size = 1.0 / 2048.0;
        // Slope-scaled bias
        let bias = max(0.005 * (1.0 - dot(in.world_normal, L)), 0.001); 
        
        if (in.shadow_pos.z <= 1.0) {
            // 5x5 Grid
            for (var x = -2; x <= 2; x++) {
                for (var y = -2; y <= 2; y++) {
                    let offset = vec2<f32>(f32(x), f32(y)) * texel_size;
                    shadow += textureSampleCompare(
                        t_shadow, 
                        s_shadow, 
                        in.shadow_pos.xy + offset, 
                        in.shadow_pos.z - bias
                    );
                }
            }
            shadow /= 25.0; // Average of 25 samples
        } else {
            shadow = 1.0;
        }
        
        // Intensify shadows (make darkness deeper)
        shadow = clamp(shadow, 0.1, 1.0); // Ambient floor for shadows
        
        color += calculate_pbr(N, V, L, radiance, albedo, metallic, roughness, F0) * shadow;
    }
    
    // --- Point Lights ---
    let count = min(light.point_light_count, 256u);
    for (var i = 0u; i < count; i++) {
        let pl = point_lights_data.lights[i];
        let light_pos = pl.position.xyz;
        let light_color = pl.color.rgb;
        let intensity = pl.color.w; 
        
        let Lx = light_pos - in.world_pos.xyz;
        let dist = length(Lx);
        let L = normalize(Lx);
        
        let attenuation = 1.0 / (dist * dist);
        let radiance = light_color * intensity * attenuation;
        
        color += calculate_pbr(N, V, L, radiance, albedo, metallic, roughness, F0);
    }
    
    // 5. Ambient (IBL Implementation)
    // Calculate Reflection Vector
    let R = reflect(-V, N);
    
    // Sample Environment (Skybox)
    // Note: In a full engine we would use lod() based on roughness, checking t_skybox mips.
    // For now, we simulate roughness by blending reflection with the diffuse normal sample.
    let env_specular = textureSample(t_skybox, s_skybox, R).rgb;
    let env_diffuse = textureSample(t_skybox, s_skybox, N).rgb; // Crude approximation of irradiance
    
    // Ambient Fresnel
    let kS_ambient = fresnel_schlick(max(dot(N, V), 0.0), F0);
    let kD_ambient = (vec3<f32>(1.0) - kS_ambient) * (1.0 - metallic);
    
    // Combine
    let ibl_diffuse = env_diffuse * albedo * 0.5; // Dim down purely sampled texture as irradiance is integral
    let ibl_specular = mix(env_specular, env_diffuse, roughness); // Fake roughness Blur
    
    let ambient = (ibl_diffuse * kD_ambient + ibl_specular * kS_ambient) * light.ambient * 3.0; // Boost ambient to make it visible
    color += ambient;
    
    // 6. Fog
    let fog_color = vec3<f32>(light.fog_r, light.fog_g, light.fog_b);
    let fog_density = light.fog_density;

    if (fog_density > 0.0) {
        let distance = length(cam_pos - in.world_pos.xyz);
        let fog_factor = exp(-fog_density * distance);
        let fog_factor_clamped = clamp(fog_factor, 0.0, 1.0);
        color = mix(fog_color, color, fog_factor_clamped);
    }
    
    // 7. Tone Mapping (ACES Filmic)
    // Narkowicz 2015 implementation of ACES
    let a = 2.51f;
    let b = 0.03f;
    let c = 2.43f;
    let d = 0.59f;
    let e = 0.14f;
    let mapped = clamp((color * (a * color + b)) / (color * (c * color + d) + e), vec3<f32>(0.0), vec3<f32>(1.0));
    
    let gamma_corrected = pow(mapped, vec3<f32>(1.0/2.2));
    
    // 8. Vignette
    let screen_pos = in.clip_position.xy / vec2<f32>(1280.0, 720.0); // Fixed Res for demo
    let vignette_center = vec2<f32>(0.5, 0.5);
    let vignette_dist = length(screen_pos - vignette_center);
    let vignette = 1.0 - smoothstep(0.4, 1.0, vignette_dist) * 0.5;
    var final_color = gamma_corrected * vignette;
    
    // 9. Grain
    let grain_seed = fract(sin(dot(screen_pos * 1000.0, vec2<f32>(12.9898, 78.233))) * 43758.5453);
    let grain_amount = 0.04;
    final_color += vec3<f32>(grain_seed - 0.5) * grain_amount;
    
    // 10. Grading (Cinematic Teal/Orange push)
    final_color.r *= 1.1; 
    final_color.b *= 0.9;
    // Boost contrast slightly
    final_color = (final_color - 0.5) * 1.1 + 0.5;
    
    return vec4<f32>(clamp(final_color, vec3<f32>(0.0), vec3<f32>(1.0)), 1.0);
}

struct SkyboxOutput {
    @builtin(position) clip_position: vec4<f32>,
    @location(0) tex_coords: vec3<f32>,
};

@vertex
fn vs_skybox(model: VertexInput) -> SkyboxOutput {
    var out: SkyboxOutput;
    let world_pos = uniforms.model * vec4<f32>(model.position, 1.0);
    // Remove translation from view matrix for skybox
    // Handled in pipeline or here? We usually center skybox.
    // Ideally view matrix should have 0 translation.
    // For now assuming existing pipeline works (depth=1).
    out.clip_position = uniforms.view_proj * world_pos;
    out.clip_position = out.clip_position.xyww; // Force to far plane
    out.tex_coords = model.position;
    return out;
}

@fragment
fn fs_skybox(in: SkyboxOutput) -> @location(0) vec4<f32> {
    // Basic gradient fallback or texture
    // But we use t_skybox texture
    let color = textureSample(t_skybox, s_skybox, in.tex_coords).rgb;
    
    // Add sun glow (if not in texture)
    let sun_dir = normalize(-light.direction);
    let dir = normalize(in.tex_coords);
    let sun_strength = pow(max(dot(dir, sun_dir), 0.0), 64.0);
    let sun_color = light.color * sun_strength * 2.0;
    
    return vec4<f32>(color + sun_color, 1.0);
}
