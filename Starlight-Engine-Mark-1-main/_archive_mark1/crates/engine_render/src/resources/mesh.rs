use bytemuck::{Pod, Zeroable};
use wgpu::util::DeviceExt;

#[repr(C)]
#[derive(Copy, Clone, Debug, Pod, Zeroable)]
pub struct Vertex {
    pub position: [f32; 3],
    pub normal: [f32; 3],
    pub color: [f32; 3],
    pub tex_coords: [f32; 2],
    pub joints: [u32; 4],
    pub weights: [f32; 4],
}

impl Vertex {
    const ATTRIBS: [wgpu::VertexAttribute; 6] = wgpu::vertex_attr_array![
        0 => Float32x3,
        1 => Float32x3,
        2 => Float32x3,
        3 => Float32x2,
        4 => Uint32x4,
        5 => Float32x4
    ];

    pub fn desc() -> wgpu::VertexBufferLayout<'static> {
        wgpu::VertexBufferLayout {
            array_stride: std::mem::size_of::<Vertex>() as wgpu::BufferAddress,
            step_mode: wgpu::VertexStepMode::Vertex,
            attributes: &Self::ATTRIBS,
        }
    }
}

#[repr(C)]
#[derive(Copy, Clone, Debug, Pod, Zeroable)]
pub struct InstanceRaw {
    pub model: [[f32; 4]; 4],
    pub color: [f32; 4],
    pub metallic: f32,
    pub roughness: f32,
    pub _padding: [f32; 2],
}

impl InstanceRaw {
    pub fn desc() -> wgpu::VertexBufferLayout<'static> {
        use std::mem;
        wgpu::VertexBufferLayout {
            array_stride: mem::size_of::<InstanceRaw>() as wgpu::BufferAddress,
            step_mode: wgpu::VertexStepMode::Instance,
            attributes: &[
                wgpu::VertexAttribute {
                    offset: 0,
                    shader_location: 6,
                    format: wgpu::VertexFormat::Float32x4,
                },
                wgpu::VertexAttribute {
                    offset: mem::size_of::<[f32; 4]>() as wgpu::BufferAddress,
                    shader_location: 7,
                    format: wgpu::VertexFormat::Float32x4,
                },
                wgpu::VertexAttribute {
                    offset: mem::size_of::<[f32; 8]>() as wgpu::BufferAddress,
                    shader_location: 8,
                    format: wgpu::VertexFormat::Float32x4,
                },
                wgpu::VertexAttribute {
                    offset: mem::size_of::<[f32; 12]>() as wgpu::BufferAddress,
                    shader_location: 9,
                    format: wgpu::VertexFormat::Float32x4,
                },
                wgpu::VertexAttribute {
                    offset: mem::size_of::<[f32; 16]>() as wgpu::BufferAddress,
                    shader_location: 10,
                    format: wgpu::VertexFormat::Float32x4,
                },
                wgpu::VertexAttribute {
                    offset: mem::size_of::<[f32; 20]>() as wgpu::BufferAddress,
                    shader_location: 11,
                    format: wgpu::VertexFormat::Float32, // Metallic
                },
                wgpu::VertexAttribute {
                    offset: (mem::size_of::<[f32; 20]>() + mem::size_of::<f32>())
                        as wgpu::BufferAddress,
                    shader_location: 12,
                    format: wgpu::VertexFormat::Float32, // Roughness
                },
            ],
        }
    }
}

#[repr(C)]
#[derive(Copy, Clone, Debug, Pod, Zeroable)]
pub struct GizmoVertex {
    pub position: [f32; 3],
    pub color: [f32; 3],
}

impl GizmoVertex {
    pub fn desc() -> wgpu::VertexBufferLayout<'static> {
        wgpu::VertexBufferLayout {
            array_stride: std::mem::size_of::<GizmoVertex>() as wgpu::BufferAddress,
            step_mode: wgpu::VertexStepMode::Vertex,
            attributes: &[
                wgpu::VertexAttribute {
                    offset: 0,
                    shader_location: 0,
                    format: wgpu::VertexFormat::Float32x3,
                },
                wgpu::VertexAttribute {
                    offset: std::mem::size_of::<[f32; 3]>() as wgpu::BufferAddress,
                    shader_location: 1,
                    format: wgpu::VertexFormat::Float32x3,
                },
            ],
        }
    }
}

#[derive(Debug, Clone)]
pub struct MeshData {
    pub vertices: Vec<Vertex>,
    pub indices: Vec<u32>,
    pub nodes: Vec<Node>,
    pub skins: Vec<Skin>,
    pub animations: Vec<Animation>,
}

pub struct Mesh {
    pub vertex_buffer: wgpu::Buffer,
    pub index_buffer: wgpu::Buffer,
    pub num_indices: u32,
    pub nodes: Vec<Node>,
    pub skins: Vec<Skin>,
    pub animations: Vec<Animation>,
}

impl Mesh {
    pub fn from_data(device: &wgpu::Device, data: MeshData) -> Self {
        let vertex_buffer = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("Vertex Buffer"),
            contents: bytemuck::cast_slice(&data.vertices),
            usage: wgpu::BufferUsages::VERTEX,
        });

        let index_buffer = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("Index Buffer"),
            contents: bytemuck::cast_slice(&data.indices),
            usage: wgpu::BufferUsages::INDEX,
        });

        Self {
            vertex_buffer,
            index_buffer,
            num_indices: data.indices.len() as u32,
            nodes: data.nodes,
            skins: data.skins,
            animations: data.animations,
        }
    }
}

#[derive(Debug, Clone)]
pub struct Node {
    pub name: Option<String>,
    pub children: Vec<usize>,
    pub transform: glam::Mat4,
    pub mesh: Option<usize>,
    pub skin: Option<usize>,
}

#[derive(Debug, Clone)]
pub struct Skin {
    pub name: Option<String>,
    pub inverse_bind_matrices: Vec<glam::Mat4>,
    pub joints: Vec<usize>,
}

#[derive(Debug, Clone)]
pub struct Animation {
    pub name: Option<String>,
    pub channels: Vec<Channel>,
    pub samplers: Vec<AnimationSampler>,
}

#[derive(Debug, Clone)]
pub struct Channel {
    pub target_node: usize,
    pub target_property: Property,
    pub sampler_index: usize,
}

#[derive(Debug, Clone)]
pub enum Property {
    Translation,
    Rotation,
    Scale,
}

#[derive(Debug, Clone)]
pub struct AnimationSampler {
    pub input: Vec<f32>,
    pub output: Vec<glam::Vec4>, // Quaternions or Vec3s
    pub interpolation: Interpolation,
}

#[derive(Debug, Clone)]
pub enum Interpolation {
    Linear,
    Step,
    CubicSpline,
}

#[derive(Debug, Clone)]
pub struct NodeTransform {
    pub translation: glam::Vec3,
    pub rotation: glam::Quat,
    pub scale: glam::Vec3,
}

impl Default for NodeTransform {
    fn default() -> Self {
        Self {
            translation: glam::Vec3::ZERO,
            rotation: glam::Quat::IDENTITY,
            scale: glam::Vec3::ONE,
        }
    }
}

#[derive(Debug, Clone)]
pub struct Skeleton {
    pub joint_matrices: Vec<glam::Mat4>,
    pub local_transforms: Vec<NodeTransform>,
}

impl Skeleton {
    pub fn from_mesh(mesh: &Mesh) -> Self {
        let local_transforms = mesh
            .nodes
            .iter()
            .map(|node| {
                let (scale, rotation, translation) = node.transform.to_scale_rotation_translation();
                NodeTransform {
                    translation,
                    rotation,
                    scale,
                }
            })
            .collect();

        Self {
            joint_matrices: vec![glam::Mat4::IDENTITY; 64], // MAX_BONES = 64
            local_transforms,
        }
    }
}

impl Mesh {
    pub fn new(device: &wgpu::Device, vertices: &[Vertex], indices: &[u32]) -> Self {
        let vertex_buffer = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("Vertex Buffer"),
            contents: bytemuck::cast_slice(vertices),
            usage: wgpu::BufferUsages::VERTEX,
        });

        let index_buffer = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("Index Buffer"),
            contents: bytemuck::cast_slice(indices),
            usage: wgpu::BufferUsages::INDEX,
        });

        Self {
            vertex_buffer,
            index_buffer,
            num_indices: indices.len() as u32,
            nodes: Vec::new(),
            skins: Vec::new(),
            animations: Vec::new(),
        }
    }
}

pub fn load_model(path: &str, device: &wgpu::Device) -> Mesh {
    let (models, _) = tobj::load_obj(
        path,
        &tobj::LoadOptions {
            single_index: true,
            triangulate: true,
            ..Default::default()
        },
    )
    .expect("Failed to load model");

    let mut vertices = Vec::new();
    let mut indices = Vec::new();

    for model in models {
        let mesh = model.mesh;

        for i in 0..mesh.positions.len() / 3 {
            let px = mesh.positions[i * 3];
            let py = mesh.positions[i * 3 + 1];
            let pz = mesh.positions[i * 3 + 2];

            let nx = if !mesh.normals.is_empty() {
                mesh.normals[i * 3]
            } else {
                0.0
            };
            let ny = if !mesh.normals.is_empty() {
                mesh.normals[i * 3 + 1]
            } else {
                1.0
            };
            let nz = if !mesh.normals.is_empty() {
                mesh.normals[i * 3 + 2]
            } else {
                0.0
            };

            let tx = if !mesh.texcoords.is_empty() {
                mesh.texcoords[i * 2]
            } else {
                0.0
            };
            let ty = if !mesh.texcoords.is_empty() {
                1.0 - mesh.texcoords[i * 2 + 1]
            } else {
                0.0
            };

            vertices.push(Vertex {
                position: [px, py, pz],
                normal: [nx, ny, nz],
                color: [1.0, 1.0, 1.0],
                tex_coords: [tx, ty],
                joints: [0; 4],
                weights: [0.0; 4],
            });
        }
        for idx in mesh.indices {
            indices.push(idx);
        }
    }
    Mesh::new(device, &vertices, &indices)
}

pub fn create_cube_mesh(device: &wgpu::Device) -> Mesh {
    // Basic cube
    let vertices = vec![
        // Front (z+)
        Vertex {
            position: [-0.5, -0.5, 0.5],
            normal: [0.0, 0.0, 1.0],
            color: [1.0, 1.0, 1.0],
            tex_coords: [0.0, 1.0],
            joints: [0; 4],
            weights: [0.0; 4],
        },
        Vertex {
            position: [0.5, 0.5, 0.5],
            normal: [0.0, 0.0, 1.0],
            color: [1.0, 1.0, 1.0],
            tex_coords: [1.0, 0.0],
            joints: [0; 4],
            weights: [0.0; 4],
        },
        Vertex {
            position: [-0.5, 0.5, 0.5],
            normal: [0.0, 0.0, 1.0],
            color: [1.0, 1.0, 1.0],
            tex_coords: [0.0, 0.0],
            joints: [0; 4],
            weights: [0.0; 4],
        },
        Vertex {
            position: [0.5, -0.5, 0.5],
            normal: [0.0, 0.0, 1.0],
            color: [1.0, 1.0, 1.0],
            tex_coords: [1.0, 1.0],
            joints: [0; 4],
            weights: [0.0; 4],
        },
        // Back (z-)
        Vertex {
            position: [-0.5, -0.5, -0.5],
            normal: [0.0, 0.0, -1.0],
            color: [1.0, 1.0, 1.0],
            tex_coords: [0.0, 1.0],
            joints: [0; 4],
            weights: [0.0; 4],
        },
        Vertex {
            position: [0.5, -0.5, -0.5],
            normal: [0.0, 0.0, -1.0],
            color: [1.0, 1.0, 1.0],
            tex_coords: [1.0, 1.0],
            joints: [0; 4],
            weights: [0.0; 4],
        },
        Vertex {
            position: [0.5, 0.5, -0.5],
            normal: [0.0, 0.0, -1.0],
            color: [1.0, 1.0, 1.0],
            tex_coords: [1.0, 0.0],
            joints: [0; 4],
            weights: [0.0; 4],
        },
        Vertex {
            position: [-0.5, 0.5, -0.5],
            normal: [0.0, 0.0, -1.0],
            color: [1.0, 1.0, 1.0],
            tex_coords: [0.0, 0.0],
            joints: [0; 4],
            weights: [0.0; 4],
        },
        // Top (y+)
        Vertex {
            position: [-0.5, 0.5, -0.5],
            normal: [0.0, 1.0, 0.0],
            color: [1.0, 1.0, 1.0],
            tex_coords: [0.0, 1.0],
            joints: [0; 4],
            weights: [0.0; 4],
        },
        Vertex {
            position: [0.5, 0.5, -0.5],
            normal: [0.0, 1.0, 0.0],
            color: [1.0, 1.0, 1.0],
            tex_coords: [1.0, 1.0],
            joints: [0; 4],
            weights: [0.0; 4],
        },
        Vertex {
            position: [0.5, 0.5, 0.5],
            normal: [0.0, 1.0, 0.0],
            color: [1.0, 1.0, 1.0],
            tex_coords: [1.0, 0.0],
            joints: [0; 4],
            weights: [0.0; 4],
        },
        Vertex {
            position: [-0.5, 0.5, 0.5],
            normal: [0.0, 1.0, 0.0],
            color: [1.0, 1.0, 1.0],
            tex_coords: [0.0, 0.0],
            joints: [0; 4],
            weights: [0.0; 4],
        },
        // Bottom (y-)
        Vertex {
            position: [-0.5, -0.5, -0.5],
            normal: [0.0, -1.0, 0.0],
            color: [1.0, 1.0, 1.0],
            tex_coords: [0.0, 1.0],
            joints: [0; 4],
            weights: [0.0; 4],
        },
        Vertex {
            position: [0.5, -0.5, -0.5],
            normal: [0.0, -1.0, 0.0],
            color: [1.0, 1.0, 1.0],
            tex_coords: [1.0, 1.0],
            joints: [0; 4],
            weights: [0.0; 4],
        },
        Vertex {
            position: [0.5, -0.5, 0.5],
            normal: [0.0, -1.0, 0.0],
            color: [1.0, 1.0, 1.0],
            tex_coords: [1.0, 0.0],
            joints: [0; 4],
            weights: [0.0; 4],
        },
        Vertex {
            position: [-0.5, -0.5, 0.5],
            normal: [0.0, -1.0, 0.0],
            color: [1.0, 1.0, 1.0],
            tex_coords: [0.0, 0.0],
            joints: [0; 4],
            weights: [0.0; 4],
        },
        // Right (x+)
        Vertex {
            position: [0.5, -0.5, -0.5],
            normal: [1.0, 0.0, 0.0],
            color: [1.0, 1.0, 1.0],
            tex_coords: [0.0, 1.0],
            joints: [0; 4],
            weights: [0.0; 4],
        },
        Vertex {
            position: [0.5, 0.5, -0.5],
            normal: [1.0, 0.0, 0.0],
            color: [1.0, 1.0, 1.0],
            tex_coords: [1.0, 1.0],
            joints: [0; 4],
            weights: [0.0; 4],
        },
        Vertex {
            position: [0.5, 0.5, 0.5],
            normal: [1.0, 0.0, 0.0],
            color: [1.0, 1.0, 1.0],
            tex_coords: [1.0, 0.0],
            joints: [0; 4],
            weights: [0.0; 4],
        },
        Vertex {
            position: [0.5, -0.5, 0.5],
            normal: [1.0, 0.0, 0.0],
            color: [1.0, 1.0, 1.0],
            tex_coords: [0.0, 0.0],
            joints: [0; 4],
            weights: [0.0; 4],
        },
        // Left (x-)
        Vertex {
            position: [-0.5, -0.5, -0.5],
            normal: [-1.0, 0.0, 0.0],
            color: [1.0, 1.0, 1.0],
            tex_coords: [0.0, 1.0],
            joints: [0; 4],
            weights: [0.0; 4],
        },
        Vertex {
            position: [-0.5, 0.5, -0.5],
            normal: [-1.0, 0.0, 0.0],
            color: [1.0, 1.0, 1.0],
            tex_coords: [1.0, 1.0],
            joints: [0; 4],
            weights: [0.0; 4],
        },
        Vertex {
            position: [-0.5, 0.5, 0.5],
            normal: [-1.0, 0.0, 0.0],
            color: [1.0, 1.0, 1.0],
            tex_coords: [1.0, 0.0],
            joints: [0; 4],
            weights: [0.0; 4],
        },
        Vertex {
            position: [-0.5, -0.5, 0.5],
            normal: [-1.0, 0.0, 0.0],
            color: [1.0, 1.0, 1.0],
            tex_coords: [0.0, 0.0],
            joints: [0; 4],
            weights: [0.0; 4],
        },
    ];
    let indices = vec![
        0, 1, 2, 2, 3, 0, 4, 7, 6, 6, 5, 4, 8, 9, 10, 10, 11, 8, 12, 15, 14, 14, 13, 12, 16, 19,
        18, 18, 17, 16, 20, 21, 22, 22, 23, 20,
    ];
    Mesh::new(device, &vertices, &indices)
}

pub fn parse_gltf(document: &gltf::Document, buffers: &[gltf::buffer::Data]) -> MeshData {
    let mut vertices = Vec::new();
    let mut indices = Vec::new();

    for mesh in document.meshes() {
        for primitive in mesh.primitives() {
            let reader = primitive.reader(|buffer| Some(&buffers[buffer.index()]));

            let mut positions = Vec::new();
            if let Some(iter) = reader.read_positions() {
                positions = iter.collect();
            }

            let mut normals = Vec::new();
            if let Some(iter) = reader.read_normals() {
                normals = iter.collect();
            }

            let mut tex_coords = Vec::new();
            if let Some(iter) = reader.read_tex_coords(0) {
                tex_coords = iter.into_f32().collect();
            }

            let mut joint_indices = Vec::new();
            if let Some(iter) = reader.read_joints(0) {
                match iter {
                    gltf::mesh::util::ReadJoints::U8(i) => {
                        for j in i {
                            joint_indices.push([
                                j[0] as u32,
                                j[1] as u32,
                                j[2] as u32,
                                j[3] as u32,
                            ]);
                        }
                    }
                    gltf::mesh::util::ReadJoints::U16(i) => {
                        for j in i {
                            joint_indices.push([
                                j[0] as u32,
                                j[1] as u32,
                                j[2] as u32,
                                j[3] as u32,
                            ]);
                        }
                    }
                }
            }

            let mut joint_weights = Vec::new();
            if let Some(iter) = reader.read_weights(0) {
                joint_weights = iter.into_f32().collect();
            }

            let base_index = vertices.len() as u32;

            for i in 0..positions.len() {
                let pos = positions[i];
                let norm = if i < normals.len() {
                    normals[i]
                } else {
                    [0.0, 1.0, 0.0]
                };
                let uv = if i < tex_coords.len() {
                    tex_coords[i]
                } else {
                    [0.0, 0.0]
                };

                let joints = if i < joint_indices.len() {
                    let j = joint_indices[i];
                    [j[0] as u32, j[1] as u32, j[2] as u32, j[3] as u32]
                } else {
                    [0; 4]
                };

                let weights = if i < joint_weights.len() {
                    joint_weights[i]
                } else {
                    [0.0; 4]
                };

                vertices.push(Vertex {
                    position: pos,
                    normal: norm,
                    color: [1.0, 1.0, 1.0],
                    tex_coords: uv,
                    joints,
                    weights,
                });
            }

            if let Some(iter) = reader.read_indices() {
                for idx in iter.into_u32() {
                    indices.push(base_index + idx);
                }
            }
        }
    }

    // Extract Nodes
    let mut nodes = Vec::new();
    for node in document.nodes() {
        let (translation, rotation, scale) = node.transform().decomposed();
        let transform = glam::Mat4::from_scale_rotation_translation(
            glam::Vec3::from_array(scale),
            glam::Quat::from_array(rotation),
            glam::Vec3::from_array(translation),
        );

        nodes.push(Node {
            name: node.name().map(|s| s.to_string()),
            children: node.children().map(|c| c.index()).collect(),
            transform,
            mesh: node.mesh().map(|m| m.index()),
            skin: node.skin().map(|s| s.index()),
        });
    }

    // Extract Skins
    let mut skins = Vec::new();
    for skin in document.skins() {
        let reader = skin.reader(|buffer| Some(&buffers[buffer.index()]));
        let inverse_bind_matrices: Vec<glam::Mat4> =
            if let Some(iter) = reader.read_inverse_bind_matrices() {
                iter.map(|m| glam::Mat4::from_cols_array_2d(&m)).collect()
            } else {
                Vec::new()
            };

        let joints: Vec<usize> = skin.joints().map(|j| j.index()).collect();

        skins.push(Skin {
            name: skin.name().map(|s| s.to_string()),
            inverse_bind_matrices,
            joints,
        });
    }

    // Extract Animations
    let mut animations = Vec::new();
    for anim in document.animations() {
        let mut channels = Vec::new();
        let mut samplers = Vec::new();

        for channel in anim.channels() {
            let target = channel.target();
            let property = match target.property() {
                gltf::animation::Property::Translation => Property::Translation,
                gltf::animation::Property::Rotation => Property::Rotation,
                gltf::animation::Property::Scale => Property::Scale,
                _ => continue, // Ignore morph targets for now
            };

            channels.push(Channel {
                target_node: target.node().index(),
                target_property: property,
                sampler_index: channel.sampler().index(),
            });
        }

        for sampler in anim.samplers() {
            let input_accessor = sampler.input();
            let mut input = Vec::new();
            {
                let view = input_accessor.view().unwrap();
                let buffer_index = view.buffer().index();
                let buffer_data = &buffers[buffer_index];
                let offset = view.offset() + input_accessor.offset();
                let stride = view.stride().unwrap_or(input_accessor.size());
                for i in 0..input_accessor.count() {
                    let start = offset + i * stride;
                    let val: f32 = bytemuck::cast_slice(&buffer_data[start..start + 4])[0];
                    input.push(val);
                }
            }

            let output_accessor = sampler.output();
            let mut output = Vec::new();
            {
                let view = output_accessor.view().unwrap();
                let buffer_index = view.buffer().index();
                let buffer_data = &buffers[buffer_index];
                let offset = view.offset() + output_accessor.offset();
                let stride = view.stride().unwrap_or(output_accessor.size());
                for i in 0..output_accessor.count() {
                    let start = offset + i * stride;
                    let end_vec3 = start + 12;
                    let end_vec4 = start + 16;

                    match output_accessor.dimensions() {
                        gltf::accessor::Dimensions::Vec3 => {
                            let v: [f32; 3] = *bytemuck::from_bytes(&buffer_data[start..end_vec3]);
                            output.push(glam::vec4(v[0], v[1], v[2], 0.0));
                        }
                        gltf::accessor::Dimensions::Vec4 => {
                            let v: [f32; 4] = *bytemuck::from_bytes(&buffer_data[start..end_vec4]);
                            output.push(glam::vec4(v[0], v[1], v[2], v[3]));
                        }
                        _ => {}
                    }
                }
            }

            let interpolation = match sampler.interpolation() {
                gltf::animation::Interpolation::Linear => Interpolation::Linear,
                gltf::animation::Interpolation::Step => Interpolation::Step,
                gltf::animation::Interpolation::CubicSpline => Interpolation::CubicSpline,
            };

            samplers.push(AnimationSampler {
                input,
                output,
                interpolation,
            });
        }

        animations.push(Animation {
            name: anim.name().map(|s| s.to_string()),
            channels,
            samplers,
        });
    }

    MeshData {
        vertices,
        indices,
        nodes,
        skins,
        animations,
    }
}

pub fn load_gltf(path: &str, device: &wgpu::Device) -> anyhow::Result<Mesh> {
    let (document, buffers, _) = gltf::import(path)?;
    let data = parse_gltf(&document, &buffers);
    Ok(Mesh::from_data(device, data))
}
