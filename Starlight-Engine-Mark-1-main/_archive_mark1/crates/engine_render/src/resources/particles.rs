use bytemuck::{Pod, Zeroable};
use wgpu::util::DeviceExt;

#[repr(C)]
#[derive(Copy, Clone, Debug, Pod, Zeroable)]
pub struct Particle {
    pub position: [f32; 3],
    pub life: f32,
    pub velocity: [f32; 3],
    pub size: f32,
    pub color: [f32; 4],
}

#[repr(C)]
#[derive(Copy, Clone, Debug, Pod, Zeroable)]
pub struct EmitterUniforms {
    pub origin: [f32; 4],
    pub direction: [f32; 3],
    pub spread: f32,
    pub speed: f32,
    pub life_base: f32,
    pub size_base: f32,
    pub delta_time: f32,
    pub time: f32,
    pub count: u32,
    pub _pad: [f32; 3],
}

pub struct ParticleSystem {
    pub particle_buffer: wgpu::Buffer,
    pub emitter_buffer: wgpu::Buffer,
    pub compute_bind_group: wgpu::BindGroup,
    pub render_bind_group: wgpu::BindGroup,
    pub count: u32,
    pub emitter_data: EmitterUniforms,
}

impl ParticleSystem {
    pub fn new(
        device: &wgpu::Device,
        compute_layout: &wgpu::BindGroupLayout,
        render_layout: &wgpu::BindGroupLayout,
        count: u32,
        origin: [f32; 3],
    ) -> Self {
        let initial_particles = vec![
            Particle {
                position: origin,
                life: 0.0,
                velocity: [0.0; 3],
                size: 1.0,
                color: [1.0, 0.5, 0.2, 1.0], // Fire-ish
            };
            count as usize
        ];

        let particle_buffer = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("Particle Buffer"),
            contents: bytemuck::cast_slice(&initial_particles),
            usage: wgpu::BufferUsages::STORAGE
                | wgpu::BufferUsages::VERTEX
                | wgpu::BufferUsages::COPY_DST,
        });

        let emitter_data = EmitterUniforms {
            origin: [origin[0], origin[1], origin[2], 1.0],
            direction: [0.0, 1.0, 0.0],
            spread: 0.5,
            speed: 2.0,
            life_base: 2.0,
            size_base: 0.2,
            delta_time: 0.0,
            time: 0.0,
            count,
            _pad: [0.0; 3],
        };

        let emitter_buffer = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("Emitter Uniform"),
            contents: bytemuck::cast_slice(&[emitter_data]),
            usage: wgpu::BufferUsages::UNIFORM | wgpu::BufferUsages::COPY_DST,
        });

        let compute_bind_group = device.create_bind_group(&wgpu::BindGroupDescriptor {
            layout: compute_layout,
            entries: &[
                wgpu::BindGroupEntry {
                    binding: 0,
                    resource: particle_buffer.as_entire_binding(),
                },
                wgpu::BindGroupEntry {
                    binding: 1,
                    resource: emitter_buffer.as_entire_binding(),
                },
            ],
            label: Some("Particle Compute BindGroup"),
        });

        let render_bind_group = device.create_bind_group(&wgpu::BindGroupDescriptor {
            layout: render_layout,
            entries: &[wgpu::BindGroupEntry {
                binding: 0,
                resource: particle_buffer.as_entire_binding(),
            }],
            label: Some("Particle Render BindGroup"),
        });

        Self {
            particle_buffer,
            emitter_buffer,
            compute_bind_group,
            render_bind_group,
            count,
            emitter_data,
        }
    }

    pub fn update(&mut self, queue: &wgpu::Queue, dt: f32, time: f32) {
        self.emitter_data.delta_time = dt;
        self.emitter_data.time = time;
        queue.write_buffer(
            &self.emitter_buffer,
            0,
            bytemuck::cast_slice(&[self.emitter_data]),
        );
    }
}
