// Vertex shader with Lighting and Normal Mapping

struct Uniforms {
    view_proj: mat4x4<f32>,
    model: mat4x4<f32>,
    camera_pos: vec4<f32>,
    time: f32,
    wind_strength: f32,
    _padding: vec2<f32>,
};

struct PointLight {
    position: vec4<f32>, 
    color: vec4<f32>,
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
    cascade_splits: vec4<f32>,
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

// Group 3: Animation
@group(3) @binding(0) var<uniform> joint_matrices: array<mat4x4<f32>, 64>;

struct VertexInput {
    @location(0) position: vec3<f32>,
    @location(1) normal: vec3<f32>,
    @location(2) color: vec3<f32>,
    @location(3) tex_coords: vec2<f32>,
    @location(4) joints: vec4<u32>,
    @location(5) weights: vec4<f32>,
};

struct VertexOutput {
    @builtin(position) clip_position: vec4<f32>,
    @location(0) world_normal: vec3<f32>,
    @location(1) color: vec3<f32>,
    @location(2) tex_coords: vec2<f32>,
    @location(3) world_pos: vec4<f32>,
    @location(4) instance_color: vec4<f32>,
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

    // Default skin matrix (identity)
    var skin_matrix = mat4x4<f32>(
        vec4<f32>(1.0, 0.0, 0.0, 0.0),
        vec4<f32>(0.0, 1.0, 0.0, 0.0),
        vec4<f32>(0.0, 0.0, 1.0, 0.0),
        vec4<f32>(0.0, 0.0, 0.0, 1.0)
    );

    // Apply skeletal animation if weights are significant
    let total_weight = model.weights.x + model.weights.y + model.weights.z + model.weights.w;
    if (total_weight > 0.0) {
        skin_matrix = 
            joint_matrices[model.joints.x] * model.weights.x +
            joint_matrices[model.joints.y] * model.weights.y +
            joint_matrices[model.joints.z] * model.weights.z +
            joint_matrices[model.joints.w] * model.weights.w;
    }

    let skinned_pos = skin_matrix * vec4<f32>(model.position, 1.0);
    let world_pos = model_matrix * skinned_pos;
    
    // Wind Animation
    // Sway based on height (model.position.y) and world position
    var pos_offset = vec3<f32>(0.0);
    let height_factor = clamp(model.position.y / 15.0, 0.0, 1.0); // Assuming typical tree height
    
    if (uniforms.wind_strength > 0.0) {
        let wind_speed = 1.2;
        // Natural multi-octave sway
        let sway_x = sin(uniforms.time * wind_speed + world_pos.x * 0.2) * uniforms.wind_strength * height_factor;
        let sway_z = cos(uniforms.time * wind_speed * 0.8 + world_pos.z * 0.15) * uniforms.wind_strength * height_factor * 0.6;
        pos_offset = vec3<f32>(sway_x, 0.0, sway_z);
    }
    
    let world_pos_animated = world_pos + vec4<f32>(pos_offset, 0.0);
    out.world_pos = world_pos_animated;
    out.clip_position = uniforms.view_proj * world_pos_animated;
    
    // Transform normal
    let normal_matrix = mat3x3<f32>(
        model_matrix[0].xyz,
        model_matrix[1].xyz,
        model_matrix[2].xyz
    );
    let skinned_normal = (skin_matrix * vec4<f32>(model.normal, 0.0)).xyz;
    out.world_normal = normalize(normal_matrix * skinned_normal);
    out.color = model.color;
    out.tex_coords = model.tex_coords;
    out.instance_color = instance.instance_color;
    
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
    
    // Alpha Discard (Transparency)
    if (tex_color.a < 0.1) {
        discard;
    }
    
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
        let radiance = light.color * light.sun_intensity; 
        
        // --- CSM Cascade Selection ---
        let depth = length(cam_pos - in.world_pos.xyz);
        var cascade_index = 3;
        if (depth < light.cascade_splits.x) { cascade_index = 0; }
        else if (depth < light.cascade_splits.y) { cascade_index = 1; }
        else if (depth < light.cascade_splits.z) { cascade_index = 2; }

        let light_matrix = light.shadow_matrices[cascade_index];
        let pos_from_light = light_matrix * in.world_pos;
        let shadow_pos = vec3<f32>(
            pos_from_light.x * 0.5 + 0.5,
            -pos_from_light.y * 0.5 + 0.5,
            pos_from_light.z
        );

        // PCF Soft Shadows (5x5 Kernel)
        var shadow = 0.0;
        let texel_size = 1.0 / 2048.0;
        let bias = max(0.005 * (1.0 - dot(N, L)), 0.0005) * (1.0 / (f32(cascade_index) + 1.0)); 
        
        if (shadow_pos.z <= 1.0) {
            for (var x = -2; x <= 2; x++) {
                for (var y = -2; y <= 2; y++) {
                    let offset = vec2<f32>(f32(x), f32(y)) * texel_size;
                    shadow += textureSampleCompare(
                        t_shadow, 
                        s_shadow, 
                        shadow_pos.xy + offset, 
                        cascade_index,
                        shadow_pos.z - bias
                    );
                }
            }
            shadow /= 25.0; 
        } else {
            shadow = 1.0;
        }
        
        shadow = clamp(shadow, 0.1, 1.0); 
        color += calculate_pbr(N, V, L, radiance, albedo, metallic, roughness, F0) * shadow;

        // Debug: Visualize Cascades (Comment out for production)
        // if (cascade_index == 0) { color += vec3<f32>(0.1, 0.0, 0.0); }
        // if (cascade_index == 1) { color += vec3<f32>(0.0, 0.1, 0.0); }
        // if (cascade_index == 2) { color += vec3<f32>(0.0, 0.0, 0.1); }
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
    
    // 6. Fog (moved to post-process volumetric fog pass)
    
    // 7. Output Linear HDR Color
    // Post-processing (Tone Mapping, Gamma, Bloom, Volumetric Fog) is now handled in the Blit pass.
    return vec4<f32>(color, 1.0);
}

@fragment
fn fs_shadow(in: VertexOutput) {
    let tex_color = textureSample(t_diffuse, s_diffuse, in.tex_coords);
    if (tex_color.a < 0.5) {
        discard;
    }
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
