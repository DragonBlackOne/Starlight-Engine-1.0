struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) tex_coords: vec2<f32>,
}

@vertex
fn vs_main(@builtin(vertex_index) vertex_index: u32) -> VertexOutput {
    var out: VertexOutput;
    // Fullscreen triangle logic
    let x = f32(i32(vertex_index == 1u) << 2u) - 1.0;
    let y = f32(i32(vertex_index == 2u) << 2u) - 1.0;
    out.position = vec4<f32>(x, y, 0.0, 1.0);
    out.tex_coords = vec2<f32>(x * 0.5 + 0.5, 1.0 - (y * 0.5 + 0.5));
    return out;
}

@group(0) @binding(0) var t_diffuse: texture_2d<f32>;
@group(0) @binding(1) var s_diffuse: sampler;

struct PostProcessUniforms {
    exposure: f32,
    gamma: f32,
    bloom_intensity: f32,
    bloom_threshold: f32,
    chromatic_aberration: f32,
    time: f32,
    // Volumetric Fog
    fog_density: f32,
    fog_height_falloff: f32,
    fog_scatter_intensity: f32,
    fog_max_distance: f32,
    // Camera
    camera_pos: vec4<f32>,
    inv_view_proj: mat4x4<f32>,
    // Light
    light_direction: vec4<f32>,
    fog_color: vec4<f32>,
};
@group(0) @binding(2) var<uniform> pp: PostProcessUniforms;

// ACES Tone Mapping
fn aces_tone_mapping(color: vec3<f32>) -> vec3<f32> {
    let a = 2.51;
    let b = 0.03;
    let c = 2.43;
    let d = 0.59;
    let e = 0.14;
    return clamp((color * (a * color + b)) / (color * (c * color + d) + e), vec3<f32>(0.0), vec3<f32>(1.0));
}

fn hash12(p: vec2<f32>) -> f32 {
	var p3  = fract(vec3<f32>(p.xyx) * .1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}

fn hash13(p: vec3<f32>) -> f32 {
    var p3 = fract(p * 0.1031);
    p3 += dot(p3, p3.zyx + 31.32);
    return fract((p3.x + p3.y) * p3.z);
}

@group(0) @binding(3) var t_depth: texture_depth_2d;

// Reconstruct world position from depth and UV
fn reconstruct_world_pos(uv: vec2<f32>, depth: f32) -> vec3<f32> {
    let ndc_x = uv.x * 2.0 - 1.0;
    let ndc_y = (1.0 - uv.y) * 2.0 - 1.0;
    let clip_pos = vec4<f32>(ndc_x, ndc_y, depth, 1.0);
    let world_pos = pp.inv_view_proj * clip_pos;
    return world_pos.xyz / world_pos.w;
}

// Height-based fog density function
fn fog_density_at(pos: vec3<f32>) -> f32 {
    // Exponential height falloff: denser near ground (y=0), thinning with altitude
    let height_factor = exp(-pp.fog_height_falloff * max(pos.y, 0.0));
    // Add subtle noise for visual variation
    let noise = hash13(pos * 0.1 + pp.time * 0.05) * 0.3 + 0.7;
    return pp.fog_density * height_factor * noise;
}

// Ray-marched volumetric fog with in-scattering
fn volumetric_fog(uv: vec2<f32>, world_pos: vec3<f32>, scene_color: vec3<f32>) -> vec3<f32> {
    if (pp.fog_density <= 0.0) {
        return scene_color;
    }

    let ray_origin = pp.camera_pos.xyz;
    let ray_dir = normalize(world_pos - ray_origin);
    let ray_length = min(length(world_pos - ray_origin), pp.fog_max_distance);
    
    let NUM_STEPS = 16;
    let step_size = ray_length / f32(NUM_STEPS);
    
    var accumulated_fog = 0.0;
    var accumulated_scatter = vec3<f32>(0.0);
    var transmittance = 1.0;
    
    // Henyey-Greenstein phase function approximation for in-scattering
    let sun_dir = normalize(-pp.light_direction.xyz);
    let cos_angle = dot(ray_dir, sun_dir);
    let g = 0.7; // Forward scattering bias
    let phase = (1.0 - g * g) / pow(1.0 + g * g - 2.0 * g * cos_angle, 1.5) / (4.0 * 3.14159);
    
    // Dithered start offset to reduce banding
    let dither = hash12(uv * vec2<f32>(1920.0, 1080.0) + pp.time) * step_size;
    
    for (var i = 0; i < NUM_STEPS; i++) {
        let t = f32(i) * step_size + dither;
        let sample_pos = ray_origin + ray_dir * t;
        
        let density = fog_density_at(sample_pos);
        let step_opacity = density * step_size;
        
        // In-scattering: light scattered towards camera by the fog
        let scatter = pp.fog_color.rgb * phase * pp.fog_scatter_intensity * density * step_size;
        
        accumulated_scatter += scatter * transmittance;
        transmittance *= exp(-step_opacity);
        
        // Early exit if fully opaque
        if (transmittance < 0.01) {
            break;
        }
    }
    
    // Blend: scene * transmittance + fog color accumulated + in-scattered light
    let fog_contribution = pp.fog_color.rgb * (1.0 - transmittance) * 0.5;
    return scene_color * transmittance + fog_contribution + accumulated_scatter;
}

// SSAO with 16-sample hemisphere kernel
fn compute_ssao(uv: vec2<f32>) -> f32 {
    let depth_center = textureSample(t_depth, s_diffuse, uv);
    
    // Skip sky (depth ~1.0)
    if (depth_center > 0.999) {
        return 0.0;
    }
    
    let world_pos = reconstruct_world_pos(uv, depth_center);
    
    // Estimate normal from depth cross-products
    let offset = 0.001;
    let pos_r = reconstruct_world_pos(uv + vec2<f32>(offset, 0.0), textureSample(t_depth, s_diffuse, uv + vec2<f32>(offset, 0.0)));
    let pos_u = reconstruct_world_pos(uv + vec2<f32>(0.0, offset), textureSample(t_depth, s_diffuse, uv + vec2<f32>(0.0, offset)));
    let normal = normalize(cross(pos_r - world_pos, pos_u - world_pos));
    
    let ssao_radius = 0.5;
    let ssao_bias = 0.025;
    var occlusion = 0.0;
    
    // Random rotation per pixel (reduces banding)
    let noise_seed = hash12(uv * vec2<f32>(1920.0, 1080.0));
    let angle = noise_seed * 6.28318; // 2*PI
    let rot = mat2x2<f32>(cos(angle), -sin(angle), sin(angle), cos(angle));
    
    // 16 hemisphere samples (pre-computed cosine-weighted directions)
    let samples = array<vec3<f32>, 16>(
        vec3<f32>( 0.04,  0.04,  0.07),
        vec3<f32>(-0.08,  0.05,  0.12),
        vec3<f32>( 0.12, -0.08,  0.15),
        vec3<f32>(-0.15,  0.12,  0.10),
        vec3<f32>( 0.06, -0.15,  0.20),
        vec3<f32>(-0.20,  0.06,  0.18),
        vec3<f32>( 0.18, -0.20,  0.08),
        vec3<f32>(-0.10,  0.18,  0.25),
        vec3<f32>( 0.25, -0.10,  0.22),
        vec3<f32>(-0.22,  0.25,  0.16),
        vec3<f32>( 0.16, -0.22,  0.30),
        vec3<f32>(-0.30,  0.16,  0.28),
        vec3<f32>( 0.28, -0.30,  0.12),
        vec3<f32>(-0.12,  0.28,  0.35),
        vec3<f32>( 0.35, -0.12,  0.32),
        vec3<f32>(-0.32,  0.35,  0.20),
    );
    
    for (var i = 0; i < 16; i++) {
        // Rotate sample XY for randomization
        var sample_dir = samples[i];
        let rotated = rot * sample_dir.xy;
        sample_dir = vec3<f32>(rotated.x, rotated.y, sample_dir.z);
        
        // Flip if sample is below surface
        if (dot(sample_dir, normal) < 0.0) {
            sample_dir = -sample_dir;
        }
        
        // Scale by radius with progressive distance
        let scale = f32(i + 1) / 16.0;
        let lerped_scale = 0.1 + scale * scale * 0.9; // Quadratic falloff
        let sample_pos = world_pos + sample_dir * ssao_radius * lerped_scale;
        
        // Project sample back to screen space
        let clip = pp.inv_view_proj * vec4<f32>(0.0); // We need view_proj, not inv
        // Workaround: use depth comparison in UV space
        let sample_offset = sample_dir.xy * ssao_radius * lerped_scale * 0.01;
        let sample_uv = uv + sample_offset;
        
        // Clamp UV
        let clamped_uv = clamp(sample_uv, vec2<f32>(0.001), vec2<f32>(0.999));
        let sample_depth = textureSample(t_depth, s_diffuse, clamped_uv);
        let sample_world = reconstruct_world_pos(clamped_uv, sample_depth);
        
        // Range check: prevent false occlusion from distant surfaces
        let range_check = smoothstep(0.0, 1.0, ssao_radius / abs(length(world_pos - sample_world) + 0.001));
        
        // Occlusion check: is the sample closer to camera than expected?
        let delta = length(world_pos - pp.camera_pos.xyz) - length(sample_world - pp.camera_pos.xyz);
        if (delta > ssao_bias) {
            occlusion += range_check;
        }
    }
    
    occlusion = occlusion / 16.0;
    return occlusion;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
    // Chromatic Aberration
    var color = vec3<f32>(0.0);
    let ca_strength = pp.chromatic_aberration * 0.01;
    
    // Glitch / Screen Shake effect via time
    var uv = in.tex_coords;
    if (pp.chromatic_aberration > 2.0) {
       let shake = (hash12(vec2<f32>(pp.time)) - 0.5) * 0.02;
       uv.x += shake;
    }

    if (ca_strength > 0.0001) {
        let r = textureSample(t_diffuse, s_diffuse, uv + vec2<f32>(ca_strength, 0.0)).r;
        let g = textureSample(t_diffuse, s_diffuse, uv).g;
        let b = textureSample(t_diffuse, s_diffuse, uv - vec2<f32>(ca_strength, 0.0)).b;
        color = vec3<f32>(r, g, b);
    } else {
        color = textureSample(t_diffuse, s_diffuse, uv).rgb;
    }

    // SSAO (16-sample hemisphere kernel)
    let occlusion = compute_ssao(uv);
    color = color * (1.0 - occlusion * 2.0);

    // Volumetric Fog (ray-marched in screen space)
    let depth = textureSample(t_depth, s_diffuse, uv);
    let world_pos = reconstruct_world_pos(uv, depth);
    color = volumetric_fog(uv, world_pos, color);

    // Exposure
    color = color * pp.exposure;

    // ACES Tone Mapping
    color = aces_tone_mapping(color);

    // Gamma Correction
    color = pow(color, vec3<f32>(1.0 / pp.gamma));

    // Grading (Cinematic Teal/Orange look)
    color.r *= 1.1;
    color.b *= 0.9;
    // Contrast Boost
    color = (color - 0.5) * 1.1 + 0.5;

    // Vignette
    let vignette_center = vec2<f32>(0.5, 0.5);
    let vignette_dist = length(in.tex_coords - vignette_center);
    let vignette = 1.0 - smoothstep(0.4, 1.0, vignette_dist) * 0.5;
    color = color * vignette;

    // Grain (Animated)
    let grain_seed = fract(sin(dot(in.tex_coords + pp.time * 0.1, vec2<f32>(12.9898, 78.233))) * 43758.5453);
    let grain_amount = 0.04 + (pp.chromatic_aberration * 0.01);
    color += vec3<f32>(grain_seed - 0.5) * grain_amount;

    return vec4<f32>(clamp(color, vec3<f32>(0.0), vec3<f32>(1.0)), 1.0);
}
