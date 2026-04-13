use super::DEPTH_FORMAT;
use crate::resources::material::{
    LightUniform, MaterialUniform, PointLight, PostProcessUniform, Uniforms,
};
use crate::resources::mesh::{create_cube_mesh, load_model, InstanceRaw, Mesh, Vertex};
use crate::resources::texture::Texture;
use std::collections::HashMap;
use std::sync::Arc;
use wgpu::util::DeviceExt;
use winit::window::Window;

pub struct RenderState {
    pub device: wgpu::Device,
    pub queue: wgpu::Queue,

    // Core Render Targets
    pub texture: wgpu::Texture,
    pub offscreen_view: wgpu::TextureView,
    pub depth_view: wgpu::TextureView,
    pub msaa_texture_view: wgpu::TextureView,
    pub output_buffer: wgpu::Buffer,

    // Pipelines
    pub render_pipeline: wgpu::RenderPipeline,
    pub shadow_pipeline: wgpu::RenderPipeline,
    pub skybox_pipeline: wgpu::RenderPipeline,
    pub transparent_pipeline: wgpu::RenderPipeline,
    pub water_pipeline: wgpu::RenderPipeline,
    pub blit_pipeline: Option<wgpu::RenderPipeline>,
    pub blit_layout: Option<wgpu::BindGroupLayout>,

    // Particle System
    pub particle_compute_pipeline: wgpu::ComputePipeline,
    pub particle_render_pipeline: wgpu::RenderPipeline,
    pub _particle_compute_layout: wgpu::BindGroupLayout,
    pub particle_render_layout: wgpu::BindGroupLayout,
    pub particle_view_layout: wgpu::BindGroupLayout,
    pub particle_view_buffer: wgpu::Buffer,
    pub particle_view_bind_group: wgpu::BindGroup,
    pub particle_systems: Vec<crate::resources::particles::ParticleSystem>,

    // Bind Group Layouts
    pub _global_layout: wgpu::BindGroupLayout,
    pub material_layout: wgpu::BindGroupLayout,
    pub skybox_layout: wgpu::BindGroupLayout,
    pub animation_layout: wgpu::BindGroupLayout,

    // Global Bind Group Resources
    pub uniform_buffer: wgpu::Buffer,
    pub light_buffer: wgpu::Buffer,
    pub light_storage_buffer: wgpu::Buffer,
    pub shadow_view: wgpu::TextureView,
    pub shadow_cascade_views: Vec<wgpu::TextureView>,
    pub _shadow_sampler: wgpu::Sampler,
    pub global_bind_group: wgpu::BindGroup,
    pub _shadow_uniform_buffer: wgpu::Buffer,
    pub shadow_bind_group: wgpu::BindGroup,
    pub shadow_cascade_buffers: Vec<wgpu::Buffer>,
    pub shadow_cascade_bind_groups: Vec<wgpu::BindGroup>,
    pub _skybox_uniform_buffer: wgpu::Buffer,
    pub skybox_bind_group: wgpu::BindGroup,
    pub default_material_bind_group: wgpu::BindGroup,
    pub default_animation_bind_group: wgpu::BindGroup,

    // Assets
    pub meshes: HashMap<String, Arc<Mesh>>,
    pub textures: HashMap<String, Arc<Texture>>,

    // Instancing (Key: (MeshID, DiffuseID, NormalID, AnimationID))
    pub instances: HashMap<(String, String, String, Option<String>), Vec<InstanceRaw>>,
    pub instance_buffers: HashMap<(String, String, String, Option<String>), (wgpu::Buffer, usize)>, // Buffer + Capacity
    pub instances_transparent: HashMap<(String, String, String, Option<String>), Vec<InstanceRaw>>,
    pub instance_buffers_transparent:
        HashMap<(String, String, String, Option<String>), (wgpu::Buffer, usize)>,
    pub material_bind_groups: HashMap<(String, String), wgpu::BindGroup>,
    pub animation_bind_groups: HashMap<String, wgpu::BindGroup>, // Keyed by EntityID or MeshID
    pub animation_buffers: HashMap<String, wgpu::Buffer>,

    // State
    pub uniforms: Uniforms,
    pub light_uniform: LightUniform,
    pub material_uniform: MaterialUniform,
    pub material_buffer: wgpu::Buffer,
    pub post_process_uniform: PostProcessUniform,
    pub post_process_buffer: wgpu::Buffer,
    pub post_process_sampler: wgpu::Sampler,
    pub blit_bind_group: Option<wgpu::BindGroup>,
    pub pending_screenshot: Option<String>,

    pub gizmos_pipeline: Option<wgpu::RenderPipeline>,
    pub gizmo_vertices: Vec<crate::resources::mesh::GizmoVertex>,
    pub gizmo_buffer: Option<wgpu::Buffer>,

    pub width: u32,
    pub height: u32,
    pub surface: Option<wgpu::Surface<'static>>,
    pub surface_config: Option<wgpu::SurfaceConfiguration>,
    pub vram_allocated: usize,
}

impl RenderState {
    pub async fn new(window: Option<&Window>, width: u32, height: u32) -> Self {
        let instance = wgpu::Instance::new(wgpu::InstanceDescriptor::default());
        let surface: Option<wgpu::Surface<'static>> = if let Some(w) = window {
            Some(unsafe { std::mem::transmute(instance.create_surface(w).unwrap()) })
        } else {
            None
        };

        let adapter = instance
            .request_adapter(&wgpu::RequestAdapterOptions {
                compatible_surface: surface.as_ref(),
                ..Default::default()
            })
            .await
            .unwrap();

        let (device, queue) = adapter
            .request_device(
                &wgpu::DeviceDescriptor {
                    label: None,
                    required_features: wgpu::Features::VERTEX_WRITABLE_STORAGE,
                    required_limits: wgpu::Limits::default(),
                    memory_hints: wgpu::MemoryHints::default(),
                },
                None,
            )
            .await
            .unwrap();

        let mut surface_config = None;
        if let Some(s) = &surface {
            let caps = s.get_capabilities(&adapter);
            let format = caps.formats[0];
            let config = wgpu::SurfaceConfiguration {
                usage: wgpu::TextureUsages::RENDER_ATTACHMENT,
                format,
                width,
                height,
                present_mode: wgpu::PresentMode::Fifo,
                alpha_mode: caps.alpha_modes[0],
                view_formats: vec![],
                desired_maximum_frame_latency: 2,
            };
            s.configure(&device, &config);
            surface_config = Some(config);
        }

        // Layouts
        let (
            global_layout,
            material_layout,
            skybox_layout,
            animation_layout,
            pipeline_layout,
            particle_compute_layout,
            particle_render_layout,
            particle_view_layout,
        ) = crate::pipeline::create_layouts(&device);

        // Pipelines
        let (
            render_pipeline,
            shadow_pipeline,
            skybox_pipeline,
            transparent_pipeline,
            water_pipeline,
            particle_compute_pipeline,
            particle_render_pipeline,
        ) = crate::pipeline::create_pipelines(
            &device,
            &pipeline_layout,
            wgpu::TextureFormat::Rgba16Float, // HDR
            &particle_compute_layout,
            &particle_render_layout,
            &particle_view_layout,
        );

        let post_process_sampler = device.create_sampler(&wgpu::SamplerDescriptor {
            label: Some("PostProcess Sampler"),
            address_mode_u: wgpu::AddressMode::ClampToEdge,
            address_mode_v: wgpu::AddressMode::ClampToEdge,
            address_mode_w: wgpu::AddressMode::ClampToEdge,
            mag_filter: wgpu::FilterMode::Linear,
            min_filter: wgpu::FilterMode::Linear,
            mipmap_filter: wgpu::FilterMode::Nearest,
            ..Default::default()
        });

        let mut blit_pipeline = None;
        let mut blit_layout = None;
        let mut blit_bind_group = None;
        if let Some(config) = &surface_config {
            let (p, l) = crate::pipeline::create_blit_pipeline(&device, config.format);
            blit_pipeline = Some(p);
            blit_layout = Some(l);
        }

        // Resources
        let uniforms = Uniforms::new();
        let uniform_buffer = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("Uniforms"),
            contents: bytemuck::cast_slice(&[uniforms]),
            usage: wgpu::BufferUsages::UNIFORM | wgpu::BufferUsages::COPY_DST,
        });
        let light_uniform = LightUniform {
            direction: [0.3, -0.8, -0.3],
            fog_density: 0.0,
            color: [1.5, 1.5, 1.4],
            ambient: 0.5,
            sun_intensity: 1.0,
            _padding_sun: [0.0; 3],
            cascade_splits: [0.1, 0.2, 0.5, 1.0], // Sample splits
            shadow_matrices: [[[0.0; 4]; 4]; 4],
            point_light_count: 0,
            fog_color: [0.5, 0.8, 0.9],
        };
        let light_buffer = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("Light"),
            contents: bytemuck::cast_slice(&[light_uniform]),
            usage: wgpu::BufferUsages::UNIFORM | wgpu::BufferUsages::COPY_DST,
        });

        let dummy_lights = vec![PointLight::default(); 256];
        let light_storage_buffer = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("Light Storage Buffer"),
            contents: bytemuck::cast_slice(&dummy_lights),
            usage: wgpu::BufferUsages::STORAGE | wgpu::BufferUsages::COPY_DST,
        });

        let post_process_uniform = PostProcessUniform::default();
        let post_process_buffer = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("PostProcess"),
            contents: bytemuck::cast_slice(&[post_process_uniform]),
            usage: wgpu::BufferUsages::UNIFORM | wgpu::BufferUsages::COPY_DST,
        });

        let shadow_texture = device.create_texture(&wgpu::TextureDescriptor {
            size: wgpu::Extent3d {
                width: 2048,
                height: 2048,
                depth_or_array_layers: 4,
            },
            mip_level_count: 1,
            sample_count: 1,
            dimension: wgpu::TextureDimension::D2,
            format: DEPTH_FORMAT,
            usage: wgpu::TextureUsages::RENDER_ATTACHMENT | wgpu::TextureUsages::TEXTURE_BINDING,
            label: Some("ShadowTexArray"),
            view_formats: &[],
        });
        let shadow_view = shadow_texture.create_view(&wgpu::TextureViewDescriptor {
            label: Some("Shadow View Array"),
            dimension: Some(wgpu::TextureViewDimension::D2Array),
            ..Default::default()
        });

        let mut shadow_cascade_views = Vec::new();
        for i in 0..4 {
            shadow_cascade_views.push(shadow_texture.create_view(&wgpu::TextureViewDescriptor {
                label: Some(&format!("Shadow Cascade View {}", i)),
                dimension: Some(wgpu::TextureViewDimension::D2),
                base_array_layer: i,
                array_layer_count: Some(1),
                ..Default::default()
            }));
        }

        let shadow_sampler = device.create_sampler(&wgpu::SamplerDescriptor {
            compare: Some(wgpu::CompareFunction::LessEqual),
            mag_filter: wgpu::FilterMode::Linear,
            min_filter: wgpu::FilterMode::Linear,
            ..Default::default()
        });

        let global_bind_group = device.create_bind_group(&wgpu::BindGroupDescriptor {
            layout: &global_layout,
            entries: &[
                wgpu::BindGroupEntry {
                    binding: 0,
                    resource: uniform_buffer.as_entire_binding(),
                },
                wgpu::BindGroupEntry {
                    binding: 1,
                    resource: light_buffer.as_entire_binding(),
                },
                wgpu::BindGroupEntry {
                    binding: 2,
                    resource: wgpu::BindingResource::TextureView(&shadow_view),
                },
                wgpu::BindGroupEntry {
                    binding: 3,
                    resource: wgpu::BindingResource::Sampler(&shadow_sampler),
                },
            ],
            label: Some("Global BindGroup"),
        });

        let shadow_uniform_buffer = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("Shadow Uniforms"),
            contents: bytemuck::cast_slice(&[uniforms]),
            usage: wgpu::BufferUsages::UNIFORM | wgpu::BufferUsages::COPY_DST,
        });
        let dummy_shadow_array = device.create_texture(&wgpu::TextureDescriptor {
            size: wgpu::Extent3d {
                width: 1,
                height: 1,
                depth_or_array_layers: 4,
            },
            mip_level_count: 1,
            sample_count: 1,
            dimension: wgpu::TextureDimension::D2,
            format: DEPTH_FORMAT,
            usage: wgpu::TextureUsages::TEXTURE_BINDING,
            label: Some("DummyShadowArray"),
            view_formats: &[],
        });
        let dummy_shadow_view = dummy_shadow_array.create_view(&wgpu::TextureViewDescriptor {
            dimension: Some(wgpu::TextureViewDimension::D2Array),
            ..Default::default()
        });

        let shadow_bind_group = device.create_bind_group(&wgpu::BindGroupDescriptor {
            layout: &global_layout,
            entries: &[
                wgpu::BindGroupEntry {
                    binding: 0,
                    resource: shadow_uniform_buffer.as_entire_binding(),
                },
                wgpu::BindGroupEntry {
                    binding: 1,
                    resource: light_buffer.as_entire_binding(),
                },
                wgpu::BindGroupEntry {
                    binding: 2,
                    resource: wgpu::BindingResource::TextureView(&dummy_shadow_view),
                },
                wgpu::BindGroupEntry {
                    binding: 3,
                    resource: wgpu::BindingResource::Sampler(&shadow_sampler),
                },
                wgpu::BindGroupEntry {
                    binding: 4,
                    resource: light_storage_buffer.as_entire_binding(),
                },
            ],
            label: Some("Shadow Pass BindGroup"),
        });

        let mut shadow_cascade_buffers = Vec::new();
        let mut shadow_cascade_bind_groups = Vec::new();
        for i in 0..4 {
            let buf = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
                label: Some(&format!("Shadow Cascade Buffer {}", i)),
                contents: bytemuck::cast_slice(&[uniforms]),
                usage: wgpu::BufferUsages::UNIFORM | wgpu::BufferUsages::COPY_DST,
            });
            let bg = device.create_bind_group(&wgpu::BindGroupDescriptor {
                layout: &global_layout,
                entries: &[
                    wgpu::BindGroupEntry {
                        binding: 0,
                        resource: buf.as_entire_binding(),
                    },
                    wgpu::BindGroupEntry {
                        binding: 1,
                        resource: light_buffer.as_entire_binding(),
                    },
                    wgpu::BindGroupEntry {
                        binding: 2,
                        resource: wgpu::BindingResource::TextureView(&dummy_shadow_view),
                    },
                    wgpu::BindGroupEntry {
                        binding: 3,
                        resource: wgpu::BindingResource::Sampler(&shadow_sampler),
                    },
                    wgpu::BindGroupEntry {
                        binding: 4,
                        resource: light_storage_buffer.as_entire_binding(),
                    },
                ],
                label: Some(&format!("Shadow Cascade BindGroup {}", i)),
            });
            shadow_cascade_buffers.push(buf);
            shadow_cascade_bind_groups.push(bg);
        }

        let skybox_uniform_buffer = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("Skybox Uniforms"),
            contents: bytemuck::cast_slice(&[uniforms]),
            usage: wgpu::BufferUsages::UNIFORM | wgpu::BufferUsages::COPY_DST,
        });
        // Skybox BindGroup requires a Texture, create default blue
        let skybox_tex = device.create_texture(&wgpu::TextureDescriptor {
            size: wgpu::Extent3d {
                width: 1,
                height: 1,
                depth_or_array_layers: 6,
            },
            mip_level_count: 1,
            sample_count: 1,
            dimension: wgpu::TextureDimension::D2,
            format: wgpu::TextureFormat::Rgba8UnormSrgb, // Skybox texture internally is Rgba
            usage: wgpu::TextureUsages::TEXTURE_BINDING | wgpu::TextureUsages::COPY_DST,
            label: Some("SkyboxDefault"),
            view_formats: &[],
        });
        let skybox_view = skybox_tex.create_view(&wgpu::TextureViewDescriptor {
            dimension: Some(wgpu::TextureViewDimension::Cube),
            ..Default::default()
        });
        let skybox_sampler = device.create_sampler(&wgpu::SamplerDescriptor {
            mag_filter: wgpu::FilterMode::Linear,
            min_filter: wgpu::FilterMode::Linear,
            ..Default::default()
        });

        // Particle View Buffer (Billboard data)
        let particle_view_buffer = device.create_buffer(&wgpu::BufferDescriptor {
            label: Some("Particle View Buffer"),
            size: 128, // Enough for mat4x4 + extras
            usage: wgpu::BufferUsages::UNIFORM | wgpu::BufferUsages::COPY_DST,
            mapped_at_creation: false,
        });

        let particle_view_bind_group = device.create_bind_group(&wgpu::BindGroupDescriptor {
            layout: &particle_view_layout,
            entries: &[wgpu::BindGroupEntry {
                binding: 0,
                resource: particle_view_buffer.as_entire_binding(),
            }],
            label: Some("Particle View BindGroup"),
        });
        let skybox_bind_group = device.create_bind_group(&wgpu::BindGroupDescriptor {
            layout: &skybox_layout,
            entries: &[
                wgpu::BindGroupEntry {
                    binding: 0,
                    resource: wgpu::BindingResource::TextureView(&skybox_view),
                },
                wgpu::BindGroupEntry {
                    binding: 1,
                    resource: wgpu::BindingResource::Sampler(&skybox_sampler),
                },
            ],
            label: Some("Skybox BindGroup"),
        });

        // Asset Maps
        let mut meshes = HashMap::new();
        let mut textures = HashMap::new();

        // Default Assets
        meshes.insert("cube".to_string(), Arc::new(create_cube_mesh(&device)));

        let white_pixel = image::DynamicImage::ImageRgba8(image::ImageBuffer::from_pixel(
            1,
            1,
            image::Rgba([255, 255, 255, 255]),
        ));
        let flat_normal = image::DynamicImage::ImageRgba8(image::ImageBuffer::from_pixel(
            1,
            1,
            image::Rgba([128, 128, 255, 255]),
        ));

        let default_tex = Arc::new(
            Texture::from_image(&device, &queue, &white_pixel, Some("Default"), false).unwrap(),
        );
        let default_norm = Arc::new(
            Texture::from_image(&device, &queue, &flat_normal, Some("FlatNormal"), true).unwrap(),
        );

        textures.insert("default".to_string(), default_tex.clone());
        textures.insert("flat_normal".to_string(), default_norm.clone());

        // Render Targets
        let texture_desc = wgpu::TextureDescriptor {
            size: wgpu::Extent3d {
                width,
                height,
                depth_or_array_layers: 1,
            },
            mip_level_count: 1,
            sample_count: 1,
            dimension: wgpu::TextureDimension::D2,
            format: wgpu::TextureFormat::Rgba16Float, // HDR Format
            usage: wgpu::TextureUsages::COPY_SRC
                | wgpu::TextureUsages::RENDER_ATTACHMENT
                | wgpu::TextureUsages::TEXTURE_BINDING
                | wgpu::TextureUsages::COPY_DST,
            label: Some("Offscreen"),
            view_formats: &[],
        };
        let texture = device.create_texture(&texture_desc);
        let offscreen_view = texture.create_view(&wgpu::TextureViewDescriptor::default());
        let depth_view = device
            .create_texture(&wgpu::TextureDescriptor {
                size: wgpu::Extent3d {
                    width,
                    height,
                    depth_or_array_layers: 1,
                },
                mip_level_count: 1,
                sample_count: 1,
                dimension: wgpu::TextureDimension::D2,
                format: DEPTH_FORMAT,
                usage: wgpu::TextureUsages::RENDER_ATTACHMENT
                    | wgpu::TextureUsages::TEXTURE_BINDING,
                label: Some("Depth"),
                view_formats: &[],
            })
            .create_view(&wgpu::TextureViewDescriptor::default());
        let msaa_texture_view = device
            .create_texture(&wgpu::TextureDescriptor {
                size: wgpu::Extent3d {
                    width,
                    height,
                    depth_or_array_layers: 1,
                },
                mip_level_count: 1,
                sample_count: 1,
                dimension: wgpu::TextureDimension::D2,
                format: wgpu::TextureFormat::Rgba8UnormSrgb,
                usage: wgpu::TextureUsages::RENDER_ATTACHMENT,
                label: Some("DummyMSAA"),
                view_formats: &[],
            })
            .create_view(&wgpu::TextureViewDescriptor::default());

        println!(
            "RenderState: MSAA disabled for stability (Single-Sampled). Format: Rgba16Float (HDR)"
        );

        let pixel_size = 8u32; // Rgba16Float = 8 bytes
        let unpadded_bytes_per_row = pixel_size * width;
        let align = 256;
        let bytes_per_row = if unpadded_bytes_per_row % align != 0 {
            unpadded_bytes_per_row + (align - unpadded_bytes_per_row % align)
        } else {
            unpadded_bytes_per_row
        };
        let output_buffer = device.create_buffer(&wgpu::BufferDescriptor {
            size: (bytes_per_row * height) as u64,
            usage: wgpu::BufferUsages::COPY_DST | wgpu::BufferUsages::MAP_READ,
            label: Some("Output"),
            mapped_at_creation: false,
        });

        if let Some(layout) = &blit_layout {
            let bg = device.create_bind_group(&wgpu::BindGroupDescriptor {
                layout,
                entries: &[
                    wgpu::BindGroupEntry {
                        binding: 0,
                        resource: wgpu::BindingResource::TextureView(&offscreen_view),
                    },
                    wgpu::BindGroupEntry {
                        binding: 1,
                        resource: wgpu::BindingResource::Sampler(&post_process_sampler),
                    },
                    wgpu::BindGroupEntry {
                        binding: 2,
                        resource: post_process_buffer.as_entire_binding(),
                    },
                    wgpu::BindGroupEntry {
                        binding: 3,
                        resource: wgpu::BindingResource::TextureView(&depth_view),
                    },
                ],
                label: Some("Blit BindGroup"),
            });
            blit_bind_group = Some(bg);
        }

        let material_uniform = MaterialUniform {
            metallic: 0.0,
            roughness: 0.5,
            _padding: [0.0; 2],
        };
        let material_buffer = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("Material"),
            contents: bytemuck::cast_slice(&[material_uniform]),
            usage: wgpu::BufferUsages::UNIFORM | wgpu::BufferUsages::COPY_DST,
        });

        let default_material_bind_group = device.create_bind_group(&wgpu::BindGroupDescriptor {
            layout: &material_layout,
            entries: &[
                wgpu::BindGroupEntry {
                    binding: 0,
                    resource: wgpu::BindingResource::TextureView(&default_tex.view),
                },
                wgpu::BindGroupEntry {
                    binding: 1,
                    resource: wgpu::BindingResource::Sampler(&default_tex.sampler),
                },
                wgpu::BindGroupEntry {
                    binding: 2,
                    resource: wgpu::BindingResource::TextureView(&default_norm.view),
                },
                wgpu::BindGroupEntry {
                    binding: 3,
                    resource: wgpu::BindingResource::Sampler(&default_norm.sampler),
                },
                wgpu::BindGroupEntry {
                    binding: 4,
                    resource: material_buffer.as_entire_binding(),
                },
            ],
            label: Some("Default Material BindGroup"),
        });

        let default_animation_buffer =
            device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
                label: Some("Default Animation Buffer"),
                contents: bytemuck::cast_slice(&[
                    crate::resources::material::JointUniform::default(),
                ]),
                usage: wgpu::BufferUsages::UNIFORM | wgpu::BufferUsages::COPY_DST,
            });

        let default_animation_bind_group = device.create_bind_group(&wgpu::BindGroupDescriptor {
            layout: &animation_layout,
            entries: &[wgpu::BindGroupEntry {
                binding: 0,
                resource: default_animation_buffer.as_entire_binding(),
            }],
            label: Some("Default Animation BindGroup"),
        });

        let mut gizmos_pipeline = None;
        if surface_config.is_some() {
            let gizmo_shader = device.create_shader_module(wgpu::ShaderModuleDescriptor {
                label: Some("Gizmo Shader"),
                source: wgpu::ShaderSource::Wgsl(
                    include_str!("../../../../assets/shaders/gizmo_lines.wgsl").into(),
                ),
            });
            let gizmos_pipeline_layout =
                device.create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
                    label: Some("Gizmo Pipeline Layout"),
                    bind_group_layouts: &[&global_layout],
                    push_constant_ranges: &[],
                });
            gizmos_pipeline = Some(device.create_render_pipeline(
                &wgpu::RenderPipelineDescriptor {
                    label: Some("Gizmo Pipeline"),
                    layout: Some(&gizmos_pipeline_layout),
                    vertex: wgpu::VertexState {
                        module: &gizmo_shader,
                        entry_point: "vs_main",
                        buffers: &[crate::resources::mesh::GizmoVertex::desc()],
                        compilation_options: Default::default(),
                    },
                    fragment: Some(wgpu::FragmentState {
                        module: &gizmo_shader,
                        entry_point: "fs_main",
                        targets: &[Some(wgpu::ColorTargetState {
                            format: wgpu::TextureFormat::Rgba16Float, // HDR target!
                            blend: Some(wgpu::BlendState::ALPHA_BLENDING),
                            write_mask: wgpu::ColorWrites::ALL,
                        })],
                        compilation_options: Default::default(),
                    }),
                    primitive: wgpu::PrimitiveState {
                        topology: wgpu::PrimitiveTopology::LineList,
                        strip_index_format: None,
                        front_face: wgpu::FrontFace::Ccw,
                        cull_mode: None,
                        unclipped_depth: false,
                        polygon_mode: wgpu::PolygonMode::Fill,
                        conservative: false,
                    },
                    depth_stencil: Some(wgpu::DepthStencilState {
                        format: super::DEPTH_FORMAT,
                        depth_write_enabled: false,
                        depth_compare: wgpu::CompareFunction::Always,
                        stencil: wgpu::StencilState::default(),
                        bias: wgpu::DepthBiasState::default(),
                    }),
                    multisample: wgpu::MultisampleState {
                        count: 4,
                        mask: !0,
                        alpha_to_coverage_enabled: false,
                    },
                    multiview: None,
                    cache: None,
                },
            ));
        }

        Self {
            device,
            queue,
            texture,
            offscreen_view,
            depth_view,
            msaa_texture_view,
            output_buffer,
            render_pipeline,
            shadow_pipeline,
            skybox_pipeline,
            transparent_pipeline,
            water_pipeline,
            blit_pipeline,
            blit_layout,
            particle_compute_pipeline,
            particle_render_pipeline,
            _particle_compute_layout: particle_compute_layout,
            particle_render_layout,
            particle_view_layout,
            particle_view_buffer,
            particle_view_bind_group,
            particle_systems: Vec::new(),
            _global_layout: global_layout,
            material_layout,
            skybox_layout,
            animation_layout,
            uniform_buffer,
            light_buffer,
            light_storage_buffer,
            shadow_view,
            shadow_cascade_views,
            _shadow_sampler: shadow_sampler,
            global_bind_group,
            _shadow_uniform_buffer: shadow_uniform_buffer,
            shadow_bind_group,
            shadow_cascade_buffers,
            shadow_cascade_bind_groups,
            _skybox_uniform_buffer: skybox_uniform_buffer,
            skybox_bind_group,
            default_material_bind_group,
            default_animation_bind_group,
            meshes,
            textures,
            instances: HashMap::new(),
            instance_buffers: HashMap::new(),
            instances_transparent: HashMap::new(),
            instance_buffers_transparent: HashMap::new(),
            material_bind_groups: HashMap::new(),
            animation_bind_groups: HashMap::new(),
            animation_buffers: HashMap::new(),
            uniforms,
            light_uniform,
            material_uniform,
            material_buffer,
            post_process_uniform,
            post_process_buffer,
            post_process_sampler,
            blit_bind_group,
            pending_screenshot: None,
            gizmos_pipeline,
            gizmo_vertices: Vec::new(),
            gizmo_buffer: None,
            width,
            height,
            surface,
            surface_config,
            vram_allocated: 0,
        }
    }

    pub fn update_animation(
        &mut self,
        entity_id: &str,
        joints: &crate::resources::material::JointUniform,
    ) {
        if !self.animation_buffers.contains_key(entity_id) {
            let buffer = self
                .device
                .create_buffer_init(&wgpu::util::BufferInitDescriptor {
                    label: Some(&format!("Animation Buffer {}", entity_id)),
                    contents: bytemuck::cast_slice(&[*joints]),
                    usage: wgpu::BufferUsages::UNIFORM | wgpu::BufferUsages::COPY_DST,
                });

            let bind_group = self.device.create_bind_group(&wgpu::BindGroupDescriptor {
                layout: &self.animation_layout,
                entries: &[wgpu::BindGroupEntry {
                    binding: 0,
                    resource: buffer.as_entire_binding(),
                }],
                label: Some(&format!("Animation BindGroup {}", entity_id)),
            });

            self.animation_buffers.insert(entity_id.to_string(), buffer);
            self.animation_bind_groups
                .insert(entity_id.to_string(), bind_group);
        } else {
            let buffer = self.animation_buffers.get(entity_id).unwrap();
            self.queue
                .write_buffer(buffer, 0, bytemuck::cast_slice(&[*joints]));
        }
    }

    pub fn resize(&mut self, width: u32, height: u32) {
        if width > 0 && height > 0 {
            self.width = width;
            self.height = height;
            let format = if let Some(surface) = &self.surface {
                if let Some(config) = &mut self.surface_config {
                    config.width = width;
                    config.height = height;
                    surface.configure(&self.device, config);
                    config.format
                } else {
                    wgpu::TextureFormat::Rgba8UnormSrgb
                }
            } else {
                wgpu::TextureFormat::Rgba8UnormSrgb
            };
            self.depth_view = self
                .device
                .create_texture(&wgpu::TextureDescriptor {
                    size: wgpu::Extent3d {
                        width,
                        height,
                        depth_or_array_layers: 1,
                    },
                    mip_level_count: 1,
                    sample_count: 1,
                    dimension: wgpu::TextureDimension::D2,
                    format: DEPTH_FORMAT,
                    usage: wgpu::TextureUsages::RENDER_ATTACHMENT
                        | wgpu::TextureUsages::TEXTURE_BINDING,
                    label: Some("Depth"),
                    view_formats: &[],
                })
                .create_view(&wgpu::TextureViewDescriptor::default());
            self.msaa_texture_view = self
                .device
                .create_texture(&wgpu::TextureDescriptor {
                    size: wgpu::Extent3d {
                        width,
                        height,
                        depth_or_array_layers: 1,
                    },
                    mip_level_count: 1,
                    sample_count: 1,
                    dimension: wgpu::TextureDimension::D2,
                    format,
                    usage: wgpu::TextureUsages::RENDER_ATTACHMENT,
                    label: Some("MSAA"),
                    view_formats: &[],
                })
                .create_view(&wgpu::TextureViewDescriptor::default());
            self.texture = self.device.create_texture(&wgpu::TextureDescriptor {
                size: wgpu::Extent3d {
                    width,
                    height,
                    depth_or_array_layers: 1,
                },
                mip_level_count: 1,
                sample_count: 1,
                dimension: wgpu::TextureDimension::D2,
                format: wgpu::TextureFormat::Rgba16Float, // HDR Format
                usage: wgpu::TextureUsages::COPY_SRC
                    | wgpu::TextureUsages::RENDER_ATTACHMENT
                    | wgpu::TextureUsages::TEXTURE_BINDING,
                label: Some("Offscreen"),
                view_formats: &[],
            });
            self.offscreen_view = self
                .texture
                .create_view(&wgpu::TextureViewDescriptor::default());

            let pixel_size = 8u32; // Rgba16Float = 8 bytes
            let unpadded_bytes_per_row = pixel_size * width;
            let align = 256;
            let bytes_per_row = if unpadded_bytes_per_row % align != 0 {
                unpadded_bytes_per_row + (align - unpadded_bytes_per_row % align)
            } else {
                unpadded_bytes_per_row
            };
            self.output_buffer = self.device.create_buffer(&wgpu::BufferDescriptor {
                size: (bytes_per_row * height) as u64,
                usage: wgpu::BufferUsages::COPY_DST | wgpu::BufferUsages::MAP_READ,
                label: Some("Output"),
                mapped_at_creation: false,
            });

            if let Some(layout) = &self.blit_layout {
                let bg = self.device.create_bind_group(&wgpu::BindGroupDescriptor {
                    layout,
                    entries: &[
                        wgpu::BindGroupEntry {
                            binding: 0,
                            resource: wgpu::BindingResource::TextureView(&self.offscreen_view),
                        },
                        wgpu::BindGroupEntry {
                            binding: 1,
                            resource: wgpu::BindingResource::Sampler(&self.post_process_sampler),
                        },
                        wgpu::BindGroupEntry {
                            binding: 2,
                            resource: self.post_process_buffer.as_entire_binding(),
                        },
                        wgpu::BindGroupEntry {
                            binding: 3,
                            resource: wgpu::BindingResource::TextureView(&self.depth_view),
                        },
                    ],
                    label: Some("Blit BindGroup"),
                });
                self.blit_bind_group = Some(bg);
            }
        }
    }

    pub fn get_vram_usage(&self) -> usize {
        self.vram_allocated
    }

    pub fn load_mesh(&mut self, name: &str, path: &str) {
        let mesh =
            if path.to_lowercase().ends_with(".glb") || path.to_lowercase().ends_with(".gltf") {
                crate::resources::mesh::load_gltf(path, &self.device).expect("Failed to load GLTF")
            } else {
                load_model(path, &self.device)
            };
        self.meshes.insert(name.to_string(), Arc::new(mesh));
    }

    pub fn load_texture(&mut self, name: &str, path: &str, is_normal: bool) {
        if path.to_lowercase().ends_with(".dds") {
            if let Ok(mut file) = std::fs::File::open(path) {
                if let Ok(dds) = ddsfile::Dds::read(&mut file) {
                    if let Ok(tex) =
                        Texture::from_dds(&self.device, &self.queue, &dds, Some(name), is_normal)
                    {
                        self.textures.insert(name.to_string(), Arc::new(tex));
                        return; // Successfully loaded DDS
                    } else {
                        println!("Failed to parse DDS format or upload to GPU for: {}", path);
                    }
                } else {
                    println!("Failed to read DDS file structure: {}", path);
                }
            }
        }

        if let Ok(img) = image::open(path) {
            if let Ok(tex) =
                Texture::from_image(&self.device, &self.queue, &img, Some(name), is_normal)
            {
                self.textures.insert(name.to_string(), Arc::new(tex));
            }
        }
    }

    pub fn load_skybox(&mut self, folder_path: &str) {
        let faces = ["px.png", "nx.png", "py.png", "ny.png", "pz.png", "nz.png"];
        let mut image_data = Vec::new();
        for face in faces {
            let path = std::path::Path::new(folder_path).join(face);
            let img = image::open(&path)
                .map(|i| i.to_rgba8())
                .unwrap_or_else(|_| {
                    image::ImageBuffer::from_pixel(1, 1, image::Rgba([50, 50, 150, 255]))
                });
            image_data.push(img);
        }
        let width = image_data[0].width();
        let height = image_data[0].height();
        let tex = self.device.create_texture(&wgpu::TextureDescriptor {
            size: wgpu::Extent3d {
                width,
                height,
                depth_or_array_layers: 6,
            },
            mip_level_count: 1,
            sample_count: 1,
            dimension: wgpu::TextureDimension::D2,
            format: wgpu::TextureFormat::Rgba8UnormSrgb,
            usage: wgpu::TextureUsages::TEXTURE_BINDING | wgpu::TextureUsages::COPY_DST,
            label: Some("Skybox"),
            view_formats: &[],
        });
        for (i, img) in image_data.iter().enumerate() {
            self.queue.write_texture(
                wgpu::ImageCopyTexture {
                    texture: &tex,
                    mip_level: 0,
                    origin: wgpu::Origin3d {
                        x: 0,
                        y: 0,
                        z: i as u32,
                    },
                    aspect: wgpu::TextureAspect::All,
                },
                img,
                wgpu::ImageDataLayout {
                    offset: 0,
                    bytes_per_row: Some(4 * width),
                    rows_per_image: Some(height),
                },
                wgpu::Extent3d {
                    width,
                    height,
                    depth_or_array_layers: 1,
                },
            );
        }
        let view = tex.create_view(&wgpu::TextureViewDescriptor {
            dimension: Some(wgpu::TextureViewDimension::Cube),
            ..Default::default()
        });
        let sampler = self.device.create_sampler(&wgpu::SamplerDescriptor {
            mag_filter: wgpu::FilterMode::Linear,
            min_filter: wgpu::FilterMode::Linear,
            ..Default::default()
        });

        self.skybox_bind_group = self.device.create_bind_group(&wgpu::BindGroupDescriptor {
            layout: &self.skybox_layout,
            entries: &[
                wgpu::BindGroupEntry {
                    binding: 0,
                    resource: wgpu::BindingResource::TextureView(&view),
                },
                wgpu::BindGroupEntry {
                    binding: 1,
                    resource: wgpu::BindingResource::Sampler(&sampler),
                },
            ],
            label: Some("Skybox BindGroup"),
        });
    }

    pub fn set_material(&mut self, metallic: f32, roughness: f32) {
        self.material_uniform.metallic = metallic;
        self.material_uniform.roughness = roughness;
        self.queue.write_buffer(
            &self.material_buffer,
            0,
            bytemuck::cast_slice(&[self.material_uniform]),
        );
    }

    pub fn set_point_light(
        &mut self,
        index: usize,
        position: [f32; 3],
        color: [f32; 3],
        intensity: f32,
    ) {
        if index < 256 {
            let point_light = PointLight {
                position: [position[0], position[1], position[2], 1.0],
                color: [color[0], color[1], color[2], intensity],
            };

            let offset = (index * std::mem::size_of::<PointLight>()) as wgpu::BufferAddress;
            self.queue.write_buffer(
                &self.light_storage_buffer,
                offset,
                bytemuck::cast_slice(&[point_light]),
            );

            let new_count = (index + 1).max(self.light_uniform.point_light_count as usize) as u32;
            if self.light_uniform.point_light_count != new_count {
                self.light_uniform.point_light_count = new_count;
                self.queue.write_buffer(
                    &self.light_buffer,
                    0,
                    bytemuck::cast_slice(&[self.light_uniform]),
                );
            }
        }
    }

    pub fn set_skybox_color(&mut self, r: f32, g: f32, b: f32) {
        // Create 1x1x6 texture
        let pixel = [(r * 255.0) as u8, (g * 255.0) as u8, (b * 255.0) as u8, 255];
        let tex = self.device.create_texture(&wgpu::TextureDescriptor {
            size: wgpu::Extent3d {
                width: 1,
                height: 1,
                depth_or_array_layers: 6,
            },
            mip_level_count: 1,
            sample_count: 1,
            dimension: wgpu::TextureDimension::D2,
            format: wgpu::TextureFormat::Rgba8UnormSrgb,
            usage: wgpu::TextureUsages::TEXTURE_BINDING | wgpu::TextureUsages::COPY_DST,
            label: Some("SkyboxColor"),
            view_formats: &[],
        });
        for layer in 0..6 {
            self.queue.write_texture(
                wgpu::ImageCopyTexture {
                    texture: &tex,
                    mip_level: 0,
                    origin: wgpu::Origin3d {
                        x: 0,
                        y: 0,
                        z: layer,
                    },
                    aspect: wgpu::TextureAspect::All,
                },
                &pixel,
                wgpu::ImageDataLayout {
                    offset: 0,
                    bytes_per_row: Some(4),
                    rows_per_image: Some(1),
                },
                wgpu::Extent3d {
                    width: 1,
                    height: 1,
                    depth_or_array_layers: 1,
                },
            );
        }
        let view = tex.create_view(&wgpu::TextureViewDescriptor {
            dimension: Some(wgpu::TextureViewDimension::Cube),
            ..Default::default()
        });
        let sampler = self.device.create_sampler(&wgpu::SamplerDescriptor {
            mag_filter: wgpu::FilterMode::Linear,
            min_filter: wgpu::FilterMode::Linear,
            ..Default::default()
        });

        self.skybox_bind_group = self.device.create_bind_group(&wgpu::BindGroupDescriptor {
            layout: &self.skybox_layout,
            entries: &[
                wgpu::BindGroupEntry {
                    binding: 0,
                    resource: wgpu::BindingResource::TextureView(&view),
                },
                wgpu::BindGroupEntry {
                    binding: 1,
                    resource: wgpu::BindingResource::Sampler(&sampler),
                },
            ],
            label: Some("Skybox BindGroup"),
        });
    }

    pub fn update_camera(&mut self, view_proj: [[f32; 4]; 4], position: [f32; 3]) {
        self.uniforms.view_proj = view_proj;
        self.uniforms.camera_pos = [position[0], position[1], position[2], 1.0];
        // We write the buffer in render() with the correction
    }

    pub fn set_wind_strength(&mut self, strength: f32) {
        self.uniforms.wind_strength = strength;
    }

    pub fn set_sun_direction(&mut self, x: f32, y: f32, z: f32) {
        self.light_uniform.direction = [x, y, z];
        self.queue.write_buffer(
            &self.light_buffer,
            0,
            bytemuck::cast_slice(&[self.light_uniform]),
        );
    }

    pub fn set_sun_color(&mut self, r: f32, g: f32, b: f32, intensity: f32) {
        self.light_uniform.color = [r, g, b];
        self.light_uniform.sun_intensity = intensity;
        self.queue.write_buffer(
            &self.light_buffer,
            0,
            bytemuck::cast_slice(&[self.light_uniform]),
        );
    }

    pub fn set_ambient_intensity(&mut self, intensity: f32) {
        self.light_uniform.ambient = intensity;
        self.queue.write_buffer(
            &self.light_buffer,
            0,
            bytemuck::cast_slice(&[self.light_uniform]),
        );
    }

    pub fn set_fog(&mut self, density: f32, r: f32, g: f32, b: f32) {
        self.light_uniform.fog_density = density;
        self.light_uniform.fog_color = [r, g, b];
        self.queue.write_buffer(
            &self.light_buffer,
            0,
            bytemuck::cast_slice(&[self.light_uniform]),
        );
    }

    // Instancing API
    pub fn clear_instances(&mut self) {
        for list in self.instances.values_mut() {
            list.clear();
        }
        for list in self.instances_transparent.values_mut() {
            list.clear();
        }
    }

    pub fn set_post_process_params(
        &mut self,
        exposure: f32,
        gamma: f32,
        bloom_intensity: f32,
        bloom_threshold: f32,
        chromatic_aberration: f32,
        time: f32,
    ) {
        self.post_process_uniform.exposure = exposure;
        self.post_process_uniform.gamma = gamma;
        self.post_process_uniform.bloom_intensity = bloom_intensity;
        self.post_process_uniform.bloom_threshold = bloom_threshold;
        self.post_process_uniform.chromatic_aberration = chromatic_aberration;
        self.post_process_uniform.time = time;
        // Note: fog params, camera_pos, inv_view_proj, light_direction, fog_color
        // are populated automatically in the render() loop from existing state.
        // Fog density is taken from light_uniform.fog_density (set via set_fog()).
    }

    pub fn set_volumetric_fog_params(
        &mut self,
        height_falloff: f32,
        scatter_intensity: f32,
        max_distance: f32,
    ) {
        self.post_process_uniform.fog_height_falloff = height_falloff;
        self.post_process_uniform.fog_scatter_intensity = scatter_intensity;
        self.post_process_uniform.fog_max_distance = max_distance;
    }

    pub fn add_instance(
        &mut self,
        mesh: &str,
        diffuse: &str,
        normal: &str,
        model: [[f32; 4]; 4],
        color: [f32; 4],
        metallic: f32,
        roughness: f32,
        animation_id: Option<String>,
    ) {
        let key = (
            mesh.to_string(),
            diffuse.to_string(),
            normal.to_string(),
            animation_id,
        );
        self.instances
            .entry(key)
            .or_insert_with(Vec::new)
            .push(InstanceRaw {
                model,
                color,
                metallic,
                roughness,
                _padding: [0.0; 2],
            });
    }

    pub fn add_transparent_instance(
        &mut self,
        mesh: &str,
        diffuse: &str,
        normal: &str,
        model: [[f32; 4]; 4],
        color: [f32; 4],
        metallic: f32,
        roughness: f32,
        animation_id: Option<String>,
    ) {
        let key = (
            mesh.to_string(),
            diffuse.to_string(),
            normal.to_string(),
            animation_id,
        );
        self.instances_transparent
            .entry(key)
            .or_insert_with(Vec::new)
            .push(InstanceRaw {
                model,
                color,
                metallic,
                roughness,
                _padding: [0.0; 2],
            });
    }

    // CSM Matrix Update
    fn update_shadow_cascades(&mut self) {
        use glam::{Mat4, Vec3, Vec4};
        // Shadow Direction
        let dir = Vec3::from_slice(&self.light_uniform.direction).normalize();
        let cam_pos_raw = self.uniforms.camera_pos;
        let cam_pos = Vec3::new(cam_pos_raw[0], cam_pos_raw[1], cam_pos_raw[2]);

        // Cascade splits (distances from camera)
        let splits = [15.0, 45.0, 100.0, 300.0];
        self.light_uniform.cascade_splits = [splits[0], splits[1], splits[2], splits[3]];

        let shadow_map_size = 2048.0;

        for i in 0..4 {
            let radius = splits[i];
            let world_units_per_texel = (radius * 2.0) / shadow_map_size;

            let mut center = cam_pos;
            // Snap to texel grid to prevent shimmering
            center.x = (center.x / world_units_per_texel).floor() * world_units_per_texel;
            center.y = (center.y / world_units_per_texel).floor() * world_units_per_texel;
            center.z = (center.z / world_units_per_texel).floor() * world_units_per_texel;

            let light_pos = center - dir * 200.0; // Pull back far enough to catch tall objects
            let view = Mat4::look_at_rh(light_pos, center, Vec3::Y);
            let proj = Mat4::orthographic_rh(-radius, radius, -radius, radius, 0.1, 500.0);

            let correction = Mat4::from_cols(
                Vec4::new(1.0, 0.0, 0.0, 0.0),
                Vec4::new(0.0, 1.0, 0.0, 0.0),
                Vec4::new(0.0, 0.0, 0.5, 0.0),
                Vec4::new(0.0, 0.0, 0.5, 1.0),
            );

            self.light_uniform.shadow_matrices[i] = (correction * proj * view).to_cols_array_2d();
        }
    }

    pub fn upload_mesh(&mut self, name: &str, vertices: &[Vertex], indices: &[u32]) {
        self.meshes.insert(
            name.to_string(),
            Arc::new(Mesh::new(&self.device, vertices, indices)),
        );
    }

    pub fn update_texture(&mut self, name: &str, img: &image::DynamicImage, is_normal: bool) {
        match Texture::from_image(&self.device, &self.queue, img, Some(name), is_normal) {
            Ok(tex) => {
                self.textures.insert(name.to_string(), Arc::new(tex));

                // Invalidate any cached material bind groups that use this texture
                let name_string = name.to_string();
                self.material_bind_groups
                    .retain(|(d, n), _| d != &name_string && n != &name_string);
            }
            Err(e) => {
                eprintln!("Failed to create texture {}: {:?}", name, e);
            }
        }
    }

    pub fn update_skybox(&mut self, images: &[image::RgbaImage]) {
        if images.len() != 6 {
            return;
        }
        let width = images[0].width();
        let height = images[0].height();

        // Recreate texture with new data size
        // Note: For now we just create a new bind group and replace it.
        // In a real engine we might stream into existing texture if size matches, but skybox change is rare.

        let tex = self.device.create_texture(&wgpu::TextureDescriptor {
            size: wgpu::Extent3d {
                width,
                height,
                depth_or_array_layers: 6,
            },
            mip_level_count: 1,
            sample_count: 1,
            dimension: wgpu::TextureDimension::D2,
            format: wgpu::TextureFormat::Rgba8UnormSrgb,
            usage: wgpu::TextureUsages::TEXTURE_BINDING | wgpu::TextureUsages::COPY_DST,
            label: Some("Skybox"),
            view_formats: &[],
        });
        for (i, img) in images.iter().enumerate() {
            self.queue.write_texture(
                wgpu::ImageCopyTexture {
                    texture: &tex,
                    mip_level: 0,
                    origin: wgpu::Origin3d {
                        x: 0,
                        y: 0,
                        z: i as u32,
                    },
                    aspect: wgpu::TextureAspect::All,
                },
                img,
                wgpu::ImageDataLayout {
                    offset: 0,
                    bytes_per_row: Some(4 * width),
                    rows_per_image: Some(height),
                },
                wgpu::Extent3d {
                    width,
                    height,
                    depth_or_array_layers: 1,
                },
            );
        }
        let view = tex.create_view(&wgpu::TextureViewDescriptor {
            dimension: Some(wgpu::TextureViewDimension::Cube),
            ..Default::default()
        });
        let sampler = self.device.create_sampler(&wgpu::SamplerDescriptor {
            mag_filter: wgpu::FilterMode::Linear,
            min_filter: wgpu::FilterMode::Linear,
            ..Default::default()
        });

        self.skybox_bind_group = self.device.create_bind_group(&wgpu::BindGroupDescriptor {
            layout: &self.skybox_layout,
            entries: &[
                wgpu::BindGroupEntry {
                    binding: 0,
                    resource: wgpu::BindingResource::TextureView(&view),
                },
                wgpu::BindGroupEntry {
                    binding: 1,
                    resource: wgpu::BindingResource::Sampler(&sampler),
                },
            ],
            label: Some("Skybox BindGroup"),
        });
    }

    pub fn add_gizmo_line(&mut self, start: [f32; 3], end: [f32; 3], color: [f32; 3]) {
        self.gizmo_vertices
            .push(crate::resources::mesh::GizmoVertex {
                position: start,
                color,
            });
        self.gizmo_vertices
            .push(crate::resources::mesh::GizmoVertex {
                position: end,
                color,
            });
    }

    pub fn render(&mut self) -> Vec<u8> {
        if !self.gizmo_vertices.is_empty() {
            let gizmo_bytes = bytemuck::cast_slice(&self.gizmo_vertices);
            if let Some(buf) = &self.gizmo_buffer {
                if buf.size() >= gizmo_bytes.len() as u64 {
                    self.queue.write_buffer(buf, 0, gizmo_bytes);
                } else {
                    self.gizmo_buffer = Some(self.device.create_buffer_init(
                        &wgpu::util::BufferInitDescriptor {
                            label: Some("Gizmo Buffer"),
                            contents: gizmo_bytes,
                            usage: wgpu::BufferUsages::VERTEX | wgpu::BufferUsages::COPY_DST,
                        },
                    ));
                }
            } else {
                self.gizmo_buffer = Some(self.device.create_buffer_init(
                    &wgpu::util::BufferInitDescriptor {
                        label: Some("Gizmo Buffer"),
                        contents: gizmo_bytes,
                        usage: wgpu::BufferUsages::VERTEX | wgpu::BufferUsages::COPY_DST,
                    },
                ));
            }
        }

        let mut inst_count = 0;
        for (_, data) in &self.instances {
            inst_count += data.len();
        }
        if inst_count > 0 {
            // println!("[RENDERER] Rendering {} instances", inst_count);
        }

        self.update_shadow_cascades();

        // Update Cascade Buffers
        for i in 0..4 {
            let mut cascade_uniforms = self.uniforms;
            cascade_uniforms.view_proj = self.light_uniform.shadow_matrices[i];
            self.queue.write_buffer(
                &self.shadow_cascade_buffers[i],
                0,
                bytemuck::cast_slice(&[cascade_uniforms]),
            );
        }

        // Update Light Buffer (important for cascade_splits and shadow_matrices in fragment shader)
        self.queue.write_buffer(
            &self.light_buffer,
            0,
            bytemuck::cast_slice(&[self.light_uniform]),
        );

        // Increment time for animations
        self.uniforms.time += 0.016;

        // Apply OpenGL to WGPU correction if view_proj is GLC-style
        let mut corrected_uniforms = self.uniforms;
        let proj = glam::Mat4::from_cols_array_2d(&self.uniforms.view_proj);
        let correction = glam::Mat4::from_cols(
            glam::Vec4::new(1.0, 0.0, 0.0, 0.0),
            glam::Vec4::new(0.0, 1.0, 0.0, 0.0),
            glam::Vec4::new(0.0, 0.0, 0.5, 0.0),
            glam::Vec4::new(0.0, 0.0, 0.5, 1.0),
        );
        corrected_uniforms.view_proj = (correction * proj).to_cols_array_2d();

        self.queue.write_buffer(
            &self.uniform_buffer,
            0,
            bytemuck::cast_slice(&[corrected_uniforms]),
        );

        let (surface_output, surface_view) = if let Some(s) = &self.surface {
            match s.get_current_texture() {
                Ok(f) => {
                    let v = f
                        .texture
                        .create_view(&wgpu::TextureViewDescriptor::default());
                    (Some(f), Some(v))
                }
                Err(e) => {
                    eprintln!("[RENDERER ERROR] get_current_texture failed: {:?}", e);
                    (None, None)
                }
            }
        } else {
            (None, None)
        };

        let mut encoder = self
            .device
            .create_command_encoder(&wgpu::CommandEncoderDescriptor {
                label: Some("Render Encoder"),
            });

        // Update Particle View Buffer (Billboard alignment)
        let view_proj = corrected_uniforms.view_proj;
        self.queue.write_buffer(
            &self.particle_view_buffer,
            0,
            bytemuck::cast_slice(&view_proj),
        );

        // Update PostProcess Uniform (fog, camera, light data for volumetric effects)
        {
            self.post_process_uniform.camera_pos = self.uniforms.camera_pos;
            self.post_process_uniform.fog_density = self.light_uniform.fog_density;
            self.post_process_uniform.fog_color = [
                self.light_uniform.fog_color[0],
                self.light_uniform.fog_color[1],
                self.light_uniform.fog_color[2],
                1.0,
            ];
            self.post_process_uniform.light_direction = [
                self.light_uniform.direction[0],
                self.light_uniform.direction[1],
                self.light_uniform.direction[2],
                0.0,
            ];
            // Compute inverse view-projection matrix for world-space reconstruction
            let vp = glam::Mat4::from_cols_array_2d(&corrected_uniforms.view_proj);
            self.post_process_uniform.inv_view_proj = vp.inverse().to_cols_array_2d();

            self.queue.write_buffer(
                &self.post_process_buffer,
                0,
                bytemuck::cast_slice(&[self.post_process_uniform]),
            );
        }

        // Update Instance Buffers
        for (key, data) in &self.instances {
            if data.is_empty() {
                continue;
            }

            // Optimization: avoid cloning strings every frame.
            // We only clone the key if it's not already in the map.
            if !self.instance_buffers.contains_key(key) {
                let initial_cap = data.len().max(100);
                let buffer = self.device.create_buffer(&wgpu::BufferDescriptor {
                    size: (std::mem::size_of::<InstanceRaw>() * initial_cap) as u64,
                    usage: wgpu::BufferUsages::VERTEX | wgpu::BufferUsages::COPY_DST,
                    label: None,
                    mapped_at_creation: false,
                });
                self.instance_buffers
                    .insert(key.clone(), (buffer, initial_cap));
            }

            let (buffer, capacity) = self.instance_buffers.get_mut(key).unwrap();

            if data.len() > *capacity {
                let new_cap = data.len();
                let buf = self.device.create_buffer(&wgpu::BufferDescriptor {
                    size: (std::mem::size_of::<InstanceRaw>() * new_cap) as u64,
                    usage: wgpu::BufferUsages::VERTEX | wgpu::BufferUsages::COPY_DST,
                    label: None,
                    mapped_at_creation: false,
                });
                *capacity = new_cap;
                *buffer = buf;
            }
            self.queue
                .write_buffer(buffer, 0, bytemuck::cast_slice(data));
        }

        // Update Transparent Instance Buffers
        for (key, data) in &self.instances_transparent {
            if data.is_empty() {
                continue;
            }

            if !self.instance_buffers_transparent.contains_key(key) {
                let initial_cap = data.len().max(100);
                let buffer = self.device.create_buffer(&wgpu::BufferDescriptor {
                    size: (std::mem::size_of::<InstanceRaw>() * initial_cap) as u64,
                    usage: wgpu::BufferUsages::VERTEX | wgpu::BufferUsages::COPY_DST,
                    label: None,
                    mapped_at_creation: false,
                });
                self.instance_buffers_transparent
                    .insert(key.clone(), (buffer, initial_cap));
            }

            let (buffer, capacity) = self.instance_buffers_transparent.get_mut(key).unwrap();

            if data.len() > *capacity {
                let new_cap = data.len();
                let buf = self.device.create_buffer(&wgpu::BufferDescriptor {
                    size: (std::mem::size_of::<InstanceRaw>() * new_cap) as u64,
                    usage: wgpu::BufferUsages::VERTEX | wgpu::BufferUsages::COPY_DST,
                    label: None,
                    mapped_at_creation: false,
                });
                *capacity = new_cap;
                *buffer = buf;
            }
            self.queue
                .write_buffer(buffer, 0, bytemuck::cast_slice(data));
        }

        // Pass 1: Shadow (Cascaded)
        for i in 0..4 {
            let mut pass = encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
                label: Some(&format!("Shadow Pass Cascade {}", i)),
                color_attachments: &[],
                depth_stencil_attachment: Some(wgpu::RenderPassDepthStencilAttachment {
                    view: &self.shadow_cascade_views[i],
                    depth_ops: Some(wgpu::Operations {
                        load: wgpu::LoadOp::Clear(1.0),
                        store: wgpu::StoreOp::Store,
                    }),
                    stencil_ops: None,
                }),
                ..Default::default()
            });
            pass.set_pipeline(&self.shadow_pipeline);
            pass.set_bind_group(0, &self.shadow_cascade_bind_groups[i], &[]);
            pass.set_bind_group(2, &self.skybox_bind_group, &[]); // Dummy bind

            // Opaque Instances
            for (key, data) in &self.instances {
                if data.is_empty() {
                    continue;
                }
                let (mesh_name, diff_name, norm_name, anim_id) = key;
                let mesh = self
                    .meshes
                    .get(mesh_name)
                    .unwrap_or_else(|| self.meshes.get("cube").unwrap());
                let (buffer, _) = &self.instance_buffers[key];

                // Bind Material
                let mat_bg = self
                    .material_bind_groups
                    .get(&(diff_name.clone(), norm_name.clone()))
                    .unwrap_or(&self.default_material_bind_group);

                pass.set_bind_group(1, mat_bg, &[]);

                // Bind Animation
                if let Some(id) = anim_id {
                    let anim_bg = self
                        .animation_bind_groups
                        .get(id)
                        .unwrap_or(&self.default_animation_bind_group);
                    pass.set_bind_group(3, anim_bg, &[]);
                } else {
                    pass.set_bind_group(3, &self.default_animation_bind_group, &[]);
                }

                pass.set_vertex_buffer(0, mesh.vertex_buffer.slice(..));
                pass.set_vertex_buffer(1, buffer.slice(..));
                pass.set_index_buffer(mesh.index_buffer.slice(..), wgpu::IndexFormat::Uint32);
                pass.draw_indexed(0..mesh.num_indices, 0, 0..data.len() as u32);
            }

            // Transparent Instances (Foliage shadows!)
            for (key, data) in &self.instances_transparent {
                if data.is_empty() {
                    continue;
                }
                let (mesh_name, diff_name, norm_name, anim_id) = key;

                if diff_name == "water" {
                    continue;
                }

                let mesh = self
                    .meshes
                    .get(mesh_name)
                    .unwrap_or_else(|| self.meshes.get("cube").unwrap());
                let (buffer, _) = &self.instance_buffers_transparent[key];

                let mat_bg = self
                    .material_bind_groups
                    .get(&(diff_name.clone(), norm_name.clone()))
                    .unwrap_or(&self.default_material_bind_group);

                pass.set_bind_group(1, mat_bg, &[]);

                // Bind Animation
                if let Some(id) = anim_id {
                    let anim_bg = self
                        .animation_bind_groups
                        .get(id)
                        .unwrap_or(&self.default_animation_bind_group);
                    pass.set_bind_group(3, anim_bg, &[]);
                } else {
                    pass.set_bind_group(3, &self.default_animation_bind_group, &[]);
                }

                pass.set_vertex_buffer(0, mesh.vertex_buffer.slice(..));
                pass.set_vertex_buffer(1, buffer.slice(..));
                pass.set_index_buffer(mesh.index_buffer.slice(..), wgpu::IndexFormat::Uint32);
                pass.draw_indexed(0..mesh.num_indices, 0, 0..data.len() as u32);
            }
        }

        // Pre-pass: Ensure bind groups exist
        let needed_keys: Vec<(String, String)> = self
            .instances
            .keys()
            .map(|(_, d, n, _)| (d.clone(), n.clone()))
            .collect();

        for (d, n) in needed_keys {
            self.ensure_material_bind_group(&d, &n);
        }

        // Pre-pass: Ensure bind groups exist for transparent items
        let needed_keys_transparent: Vec<(String, String)> = self
            .instances_transparent
            .keys()
            .map(|(_, d, n, _)| (d.clone(), n.clone()))
            .collect();

        for (d, n) in needed_keys_transparent {
            self.ensure_material_bind_group(&d, &n);
        }

        // 1. Particle Compute Pass
        {
            let mut compute_pass = encoder.begin_compute_pass(&wgpu::ComputePassDescriptor {
                label: Some("Particle Compute Pass"),
                timestamp_writes: None,
            });
            compute_pass.set_pipeline(&self.particle_compute_pipeline);
            for ps in &self.particle_systems {
                compute_pass.set_bind_group(0, &ps.compute_bind_group, &[]);
                let dispatch_count = (ps.count + 63) / 64;
                compute_pass.dispatch_workgroups(dispatch_count, 1, 1);
            }
        }

        // Pass 2: Main
        {
            let mut pass = encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
                label: Some("Main Pass"),
                color_attachments: &[Some(wgpu::RenderPassColorAttachment {
                    view: &self.offscreen_view, // Render directly to offscreen
                    resolve_target: None,
                    ops: wgpu::Operations {
                        load: wgpu::LoadOp::Clear(wgpu::Color {
                            r: 0.1,
                            g: 0.2,
                            b: 0.3,
                            a: 1.0,
                        }),
                        store: wgpu::StoreOp::Store,
                    },
                })],
                depth_stencil_attachment: Some(wgpu::RenderPassDepthStencilAttachment {
                    view: &self.depth_view,
                    depth_ops: Some(wgpu::Operations {
                        load: wgpu::LoadOp::Clear(1.0),
                        store: wgpu::StoreOp::Store,
                    }),
                    stencil_ops: None,
                }),
                ..Default::default()
            });

            pass.set_viewport(0.0, 0.0, self.width as f32, self.height as f32, 0.0, 1.0);
            pass.set_pipeline(&self.render_pipeline);
            pass.set_bind_group(0, &self.global_bind_group, &[]);
            pass.set_bind_group(2, &self.skybox_bind_group, &[]);

            for (key, data) in &self.instances {
                if data.is_empty() {
                    continue;
                }
                let (mesh_name, diff_name, norm_name, anim_id) = key;
                let mesh = self
                    .meshes
                    .get(mesh_name)
                    .unwrap_or_else(|| self.meshes.get("cube").unwrap());
                let buffer = &self.instance_buffers[key].0;

                // Retrieve cached bind group
                let mat_bg = self
                    .material_bind_groups
                    .get(&(diff_name.clone(), norm_name.clone()))
                    .unwrap();

                pass.set_bind_group(1, mat_bg, &[]);

                // Bind Animation (Group 3)
                if let Some(id) = anim_id {
                    let anim_bg = self
                        .animation_bind_groups
                        .get(id)
                        .unwrap_or(&self.default_animation_bind_group);
                    pass.set_bind_group(3, anim_bg, &[]);
                } else {
                    pass.set_bind_group(3, &self.default_animation_bind_group, &[]);
                }

                pass.set_vertex_buffer(0, mesh.vertex_buffer.slice(..));
                pass.set_vertex_buffer(1, buffer.slice(..));
                pass.set_index_buffer(mesh.index_buffer.slice(..), wgpu::IndexFormat::Uint32);
                pass.draw_indexed(0..mesh.num_indices, 0, 0..data.len() as u32);
            }

            // Skybox
            pass.set_pipeline(&self.skybox_pipeline);
            pass.set_bind_group(0, &self.global_bind_group, &[]);
            pass.set_bind_group(1, &self.default_material_bind_group, &[]); // Dummy bind
            pass.set_bind_group(2, &self.skybox_bind_group, &[]);

            let cube = self.meshes.get("cube").unwrap();
            pass.set_vertex_buffer(0, cube.vertex_buffer.slice(..));
            pass.set_index_buffer(cube.index_buffer.slice(..), wgpu::IndexFormat::Uint32);
            pass.draw_indexed(0..cube.num_indices, 0, 0..1);

            drop(pass);

            // Pass 3: Transparent
            {
                let mut pass = encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
                    label: Some("Transparent Pass"),
                    color_attachments: &[Some(wgpu::RenderPassColorAttachment {
                        view: &self.offscreen_view,
                        resolve_target: None,
                        ops: wgpu::Operations {
                            load: wgpu::LoadOp::Load, // Load existing opaque colors
                            store: wgpu::StoreOp::Store,
                        },
                    })],
                    depth_stencil_attachment: Some(wgpu::RenderPassDepthStencilAttachment {
                        view: &self.depth_view,
                        depth_ops: Some(wgpu::Operations {
                            load: wgpu::LoadOp::Load, // Load existing depth
                            store: wgpu::StoreOp::Store,
                        }),
                        stencil_ops: None,
                    }),
                    ..Default::default()
                });

                pass.set_viewport(0.0, 0.0, self.width as f32, self.height as f32, 0.0, 1.0);

                // Particles Rendering (Transparent)
                pass.set_pipeline(&self.particle_render_pipeline);
                pass.set_bind_group(1, &self.particle_view_bind_group, &[]);
                for ps in &self.particle_systems {
                    pass.set_bind_group(0, &ps.render_bind_group, &[]);
                    // Draw 6 vertices (2 triangles) per particle. Instancing handled via storage buffer index.
                    pass.draw(0..6, 0..ps.count);
                }

                // Default to transparent pipeline for meshes
                pass.set_pipeline(&self.transparent_pipeline);
                pass.set_bind_group(0, &self.global_bind_group, &[]);
                pass.set_bind_group(2, &self.skybox_bind_group, &[]);

                for (key, data) in &self.instances_transparent {
                    if data.is_empty() {
                        continue;
                    }
                    let (mesh_name, diff_name, norm_name, anim_id) = key;

                    // Check for Water
                    if diff_name == "water" || norm_name == "water_normal" {
                        // Heuristic
                        pass.set_pipeline(&self.water_pipeline);
                    } else {
                        pass.set_pipeline(&self.transparent_pipeline);
                    }

                    let mesh = self
                        .meshes
                        .get(mesh_name)
                        .unwrap_or_else(|| self.meshes.get("cube").unwrap());
                    let buffer = &self.instance_buffers_transparent[key].0;

                    let mat_bg = self
                        .material_bind_groups
                        .get(&(diff_name.clone(), norm_name.clone()))
                        .unwrap();

                    pass.set_bind_group(1, mat_bg, &[]);

                    // Bind Animation (Group 3)
                    if let Some(id) = anim_id {
                        let anim_bg = self
                            .animation_bind_groups
                            .get(id)
                            .unwrap_or(&self.default_animation_bind_group);
                        pass.set_bind_group(3, anim_bg, &[]);
                    } else {
                        pass.set_bind_group(3, &self.default_animation_bind_group, &[]);
                    }

                    pass.set_vertex_buffer(0, mesh.vertex_buffer.slice(..));
                    pass.set_vertex_buffer(1, buffer.slice(..));
                    pass.set_index_buffer(mesh.index_buffer.slice(..), wgpu::IndexFormat::Uint32);
                    pass.draw_indexed(0..mesh.num_indices, 0, 0..data.len() as u32);
                }
            }

            // Perform Blit to Surface if available
            // Perform Blit to Surface if available
            if let (Some(pipeline), Some(blit_bg), Some(view)) =
                (&self.blit_pipeline, &self.blit_bind_group, &surface_view)
            {
                let mut blit_pass = encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
                    label: Some("Blit Pass"),
                    color_attachments: &[Some(wgpu::RenderPassColorAttachment {
                        view,
                        resolve_target: None,
                        ops: wgpu::Operations {
                            load: wgpu::LoadOp::Clear(wgpu::Color::BLACK),
                            store: wgpu::StoreOp::Store,
                        },
                    })],
                    ..Default::default()
                });

                blit_pass.set_pipeline(pipeline);
                blit_pass.set_bind_group(0, blit_bg, &[]);
                blit_pass.draw(0..3, 0..1);
            }
        }

        if let Some(path) = self.pending_screenshot.take() {
            // Copy for Screenshot
            let pixel_size = 8u32; // Rgba16Float
            let unpadded_bytes_per_row = pixel_size * self.width;
            let align = 256;
            let bytes_per_row = if unpadded_bytes_per_row % align != 0 {
                unpadded_bytes_per_row + (align - unpadded_bytes_per_row % align)
            } else {
                unpadded_bytes_per_row
            };

            let mut screenshot_encoder =
                self.device
                    .create_command_encoder(&wgpu::CommandEncoderDescriptor {
                        label: Some("Screenshot Encoder"),
                    });

            screenshot_encoder.copy_texture_to_buffer(
                wgpu::ImageCopyTexture {
                    aspect: wgpu::TextureAspect::All,
                    texture: &self.texture, // Always copy from offscreen (resolved above)
                    mip_level: 0,
                    origin: wgpu::Origin3d::ZERO,
                },
                wgpu::ImageCopyBuffer {
                    buffer: &self.output_buffer,
                    layout: wgpu::ImageDataLayout {
                        offset: 0,
                        bytes_per_row: Some(bytes_per_row),
                        rows_per_image: Some(self.height),
                    },
                },
                wgpu::Extent3d {
                    width: self.width,
                    height: self.height,
                    depth_or_array_layers: 1,
                },
            );
            self.queue
                .submit([encoder.finish(), screenshot_encoder.finish()]);

            let buffer_slice = self.output_buffer.slice(..);
            let (tx, rx) = std::sync::mpsc::channel();
            buffer_slice.map_async(wgpu::MapMode::Read, move |result| {
                tx.send(result).unwrap();
            });
            self.device.poll(wgpu::Maintain::Wait);
            rx.recv().unwrap().unwrap();
            let data = buffer_slice.get_mapped_range().to_vec();
            self.output_buffer.unmap();

            // Unpad
            let saved_data = if bytes_per_row != unpadded_bytes_per_row {
                let mut unpadded = Vec::with_capacity((self.width * self.height * 8) as usize);
                for i in 0..self.height {
                    let start = (i * bytes_per_row) as usize;
                    let end = start + (self.width * 8) as usize;
                    unpadded.extend_from_slice(&data[start..end]);
                }
                unpadded
            } else {
                data
            };

            // Convert Rgba16Float (8 bytes) to Rgba8 (4 bytes) for PNG
            let mut rgba8_data = Vec::with_capacity((self.width * self.height * 4) as usize);
            for chunk in saved_data.chunks(8) {
                // Parse f16
                let r_f16 = u16::from_le_bytes([chunk[0], chunk[1]]);
                let g_f16 = u16::from_le_bytes([chunk[2], chunk[3]]);
                let b_f16 = u16::from_le_bytes([chunk[4], chunk[5]]);
                let a_f16 = u16::from_le_bytes([chunk[6], chunk[7]]);

                let r = half::f16::from_bits(r_f16).to_f32();
                let g = half::f16::from_bits(g_f16).to_f32();
                let b = half::f16::from_bits(b_f16).to_f32();
                let a = half::f16::from_bits(a_f16).to_f32();

                rgba8_data.push((r.clamp(0.0, 1.0) * 255.0) as u8);
                rgba8_data.push((g.clamp(0.0, 1.0) * 255.0) as u8);
                rgba8_data.push((b.clamp(0.0, 1.0) * 255.0) as u8);
                rgba8_data.push((a.clamp(0.0, 1.0) * 255.0) as u8);
            }

            // Save logic (requires image crate dependency in engine_render!)
            // engine_render has image dep? It has logic for textures. Yes.
            // But usually image crate usage:
            if let Err(e) = image::save_buffer_with_format(
                &path,
                &rgba8_data,
                self.width,
                self.height,
                image::ColorType::Rgba8,
                image::ImageFormat::Png,
            ) {
                eprintln!("Failed to save screenshot: {}", e);
            } else {
                println!("Screenshot saved to {}", path);
            }
        } else {
            // CRITICAL: Must submit encoder to GPU before presenting!
            self.queue.submit([encoder.finish()]);
            if let Some(output) = surface_output {
                output.present();
            }
        }

        self.gizmo_vertices.clear();
        Vec::new()
    }

    pub fn render_to_f32(&mut self) -> Vec<f32> {
        self.render();

        let pixel_size = 8u32; // Rgba16Float
        let unpadded_bytes_per_row = pixel_size * self.width;
        let align = 256;
        let bytes_per_row = if unpadded_bytes_per_row % align != 0 {
            unpadded_bytes_per_row + (align - unpadded_bytes_per_row % align)
        } else {
            unpadded_bytes_per_row
        };

        let mut encoder = self
            .device
            .create_command_encoder(&wgpu::CommandEncoderDescriptor {
                label: Some("F32 Export Encoder"),
            });

        encoder.copy_texture_to_buffer(
            wgpu::ImageCopyTexture {
                aspect: wgpu::TextureAspect::All,
                texture: &self.texture,
                mip_level: 0,
                origin: wgpu::Origin3d::ZERO,
            },
            wgpu::ImageCopyBuffer {
                buffer: &self.output_buffer,
                layout: wgpu::ImageDataLayout {
                    offset: 0,
                    bytes_per_row: Some(bytes_per_row),
                    rows_per_image: Some(self.height),
                },
            },
            wgpu::Extent3d {
                width: self.width,
                height: self.height,
                depth_or_array_layers: 1,
            },
        );
        self.queue.submit([encoder.finish()]);

        let buffer_slice = self.output_buffer.slice(..);
        let (tx, rx) = std::sync::mpsc::channel();
        buffer_slice.map_async(wgpu::MapMode::Read, move |result| {
            tx.send(result).unwrap();
        });
        self.device.poll(wgpu::Maintain::Wait);
        rx.recv().unwrap().unwrap();
        let data = buffer_slice.get_mapped_range().to_vec();
        self.output_buffer.unmap();

        let saved_data = if bytes_per_row != unpadded_bytes_per_row {
            let mut unpadded = Vec::with_capacity((self.width * self.height * 8) as usize);
            for i in 0..self.height {
                let start = (i * bytes_per_row) as usize;
                let end = start + (self.width * 8) as usize;
                unpadded.extend_from_slice(&data[start..end]);
            }
            unpadded
        } else {
            data
        };

        let mut f32_data = Vec::with_capacity((self.width * self.height * 4) as usize);
        for chunk in saved_data.chunks(8) {
            let r_f16 = u16::from_le_bytes([chunk[0], chunk[1]]);
            let g_f16 = u16::from_le_bytes([chunk[2], chunk[3]]);
            let b_f16 = u16::from_le_bytes([chunk[4], chunk[5]]);
            let a_f16 = u16::from_le_bytes([chunk[6], chunk[7]]);

            f32_data.push(half::f16::from_bits(r_f16).to_f32());
            f32_data.push(half::f16::from_bits(g_f16).to_f32());
            f32_data.push(half::f16::from_bits(b_f16).to_f32());
            f32_data.push(half::f16::from_bits(a_f16).to_f32());
        }

        f32_data
    }

    pub fn request_screenshot(&mut self, path: String) {
        self.pending_screenshot = Some(path);
    }

    pub fn ensure_material_bind_group(&mut self, diff_name: &str, norm_name: &str) {
        let key = (diff_name.to_string(), norm_name.to_string());
        if self.material_bind_groups.contains_key(&key) {
            return;
        }

        let diff = self
            .textures
            .get(diff_name)
            .unwrap_or_else(|| self.textures.get("default").unwrap());
        let norm = self
            .textures
            .get(norm_name)
            .unwrap_or_else(|| self.textures.get("flat_normal").unwrap());

        let mat_bg = self.device.create_bind_group(&wgpu::BindGroupDescriptor {
            layout: &self.material_layout,
            entries: &[
                wgpu::BindGroupEntry {
                    binding: 0,
                    resource: wgpu::BindingResource::TextureView(&diff.view),
                },
                wgpu::BindGroupEntry {
                    binding: 1,
                    resource: wgpu::BindingResource::Sampler(&diff.sampler),
                },
                wgpu::BindGroupEntry {
                    binding: 2,
                    resource: wgpu::BindingResource::TextureView(&norm.view),
                },
                wgpu::BindGroupEntry {
                    binding: 3,
                    resource: wgpu::BindingResource::Sampler(&norm.sampler),
                },
                wgpu::BindGroupEntry {
                    binding: 4,
                    resource: self.material_buffer.as_entire_binding(),
                },
            ],
            label: Some(&format!("MaterialBG_{}_{}", diff_name, norm_name)),
        });

        self.material_bind_groups.insert(key, mat_bg);
    }

    pub fn recompile_shader(&mut self, source: &str) -> std::result::Result<(), String> {
        let pipeline_layout = self
            .device
            .create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
                label: Some("Pipeline Layout (Recompiled)"),
                bind_group_layouts: &[
                    &self._global_layout,
                    &self.material_layout,
                    &self.skybox_layout,
                    &self.animation_layout,
                ],
                push_constant_ranges: &[],
            });

        let format = wgpu::TextureFormat::Rgba16Float;

        match crate::pipeline::create_render_pipeline_from_source(
            &self.device,
            &pipeline_layout,
            format,
            source,
        ) {
            Ok(new_pipeline) => {
                self.render_pipeline = new_pipeline;
                Ok(())
            }
            Err(e) => Err(e),
        }
    }
}
