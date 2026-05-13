
// Particle System Shader (Compute + Render)

struct Particle {
    position: vec3<f32>,
    life: f32,
    velocity: vec3<f32>,
    size: f32,
    color: vec4<f32>,
};

struct ParticleBuffer {
    particles: array<Particle>,
};

struct EmitterUniforms {
    origin: vec4<f32>,
    direction: vec3<f32>,
    spread: f32,
    speed: f32,
    life_base: f32,
    size_base: f32,
    delta_time: f32,
    time: f32,
    count: u32,
    _pad: vec3<f32>,
};

// Bindings for Compute
@group(0) @binding(0) var<storage, read_write> p_buffer: ParticleBuffer;
@group(0) @binding(1) var<uniform> emitter: EmitterUniforms;

// Hash function for random-ish values on GPU
fn hash(u: u32) -> f32 {
    var x = u;
    x = ((x >> 16u) ^ x) * 0x45d9f3bu;
    x = ((x >> 16u) ^ x) * 0x45d9f3bu;
    x = (x >> 16u) ^ x;
    return f32(x) / 4294967295.0;
}

@compute @workgroup_size(64)
fn compute_main(@builtin(global_invocation_id) id: vec3<u32>) {
    let idx = id.x;
    if (idx >= emitter.count) { return; }

    var p = p_buffer.particles[idx];

    // Update life
    p.life -= emitter.delta_time;

    if (p.life <= 0.0) {
        // Respawn particle at emitter
        let h1 = hash(idx + u32(emitter.time * 1000.0));
        let h2 = hash(idx + u32(emitter.time * 1000.0) + 1u);
        let h3 = hash(idx + u32(emitter.time * 1000.0) + 2u);

        p.position = emitter.origin.xyz;
        
        let random_dir = normalize(emitter.direction + vec3<f32>(
            (h1 - 0.5) * emitter.spread,
            (h2 - 0.5) * emitter.spread,
            (h3 - 0.5) * emitter.spread
        ));

        p.velocity = random_dir * emitter.speed;
        p.life = emitter.life_base * (0.5 + h1 * 0.5);
        p.size = emitter.size_base;
    } else {
        // Simpulated physics (gravity fallback)
        p.velocity.y -= 9.8 * emitter.delta_time * 0.1; 
        p.position += p.velocity * emitter.delta_time;
    }

    p_buffer.particles[idx] = p;
}

// Rendering
struct ViewUniforms {
    view_proj: mat4x4<f32>,
    camera_pos: vec4<f32>,
    camera_right: vec4<f32>,
    camera_up: vec4<f32>,
};

@group(1) @binding(0) var<uniform> view: ViewUniforms;

struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) color: vec4<f32>,
    @location(1) uv: vec2<f32>,
};

@vertex
fn vs_main(@builtin(instance_index) instance_idx: u32, @builtin(vertex_index) vertex_idx: u32) -> VertexOutput {
    let p = p_buffer.particles[instance_idx];
    
    let quad_idx = vertex_idx % 4u;
    var uv: vec2<f32>;
    if (quad_idx == 0u) { uv = vec2<f32>(-0.5, -0.5); }
    else if (quad_idx == 1u) { uv = vec2<f32>(0.5, -0.5); }
    else if (quad_idx == 2u) { uv = vec2<f32>(0.5, 0.5); }
    else { uv = vec2<f32>(-0.5, 0.5); }
    
    let offset = uv * p.size;
    
    // Billboard calculation: pos + (right * offset.x) + (up * offset.y)
    let world_pos = p.position + (view.camera_right.xyz * offset.x) + (view.camera_up.xyz * offset.y);
    
    var out: VertexOutput;
    out.position = view.view_proj * vec4<f32>(world_pos, 1.0);
    out.color = p.color;
    // Fade out based on life
    out.color.a = clamp(p.life, 0.0, 1.0);
    out.uv = uv + 0.5;
    
    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
    // Simple circular particle
    let dist = length(in.uv - 0.5);
    if (dist > 0.5) { discard; }
    
    let alpha = (0.5 - dist) * 2.0;
    return vec4<f32>(in.color.rgb, in.color.a * alpha);
}
