use crate::assets::{AssetType, SceneGraph, SceneNode};
use crate::vfs::Vfs;
use glam::{Quat, Vec3};
use std::path::Path;
use tokio::sync::mpsc::{unbounded_channel, UnboundedReceiver, UnboundedSender};

// Data structure sent from Main -> Loader
pub struct LoadRequest {
    pub uuid: String,
    pub path: String,
    pub asset_type: AssetType,
    pub is_normal_map: bool, // For textures
}

// Data structure sent from Loader -> Main
pub enum AssetData {
    Mesh {
        uuid: String,
        vertices: Vec<engine_render::Vertex>,
        indices: Vec<u32>,
    },
    Texture {
        uuid: String,
        img: image::DynamicImage,
        is_normal_map: bool,
    },
    Skybox {
        uuid: String,
        images: Vec<image::RgbaImage>, // 6 images
    },
    Scene {
        uuid: String,
        graph: SceneGraph,
    },
    Error {
        uuid: String,
        error: String,
    },
}

pub struct AsyncAssetLoader {
    pub vfs: Vfs,
    pub sender: UnboundedSender<LoadRequest>,
    pub receiver: Option<UnboundedReceiver<AssetData>>,
}

impl AsyncAssetLoader {
    pub fn new(vfs: Vfs) -> (Self, UnboundedReceiver<AssetData>) {
        let (req_tx, mut req_rx) = unbounded_channel::<LoadRequest>();
        let (res_tx, res_rx) = unbounded_channel::<AssetData>();

        let loader_vfs = vfs.clone();

        tokio::spawn(async move {
            while let Some(req) = req_rx.recv().await {
                let vfs = loader_vfs.clone();
                let res_tx = res_tx.clone();

                tokio::spawn(async move {
                    let result = match req.asset_type {
                        AssetType::Mesh => load_mesh(&vfs, req).await,
                        AssetType::Texture => load_texture(&vfs, req).await,
                        AssetType::Skybox => load_skybox(&vfs, req).await,
                        AssetType::Scene => load_gltf_scene(&vfs, req, res_tx.clone()).await,
                        _ => AssetData::Error {
                            uuid: req.uuid,
                            error: "Unsupported type".into(),
                        },
                    };
                    let _ = res_tx.send(result);
                });
            }
        });

        (
            Self {
                vfs,
                sender: req_tx,
                receiver: None,
            },
            res_rx,
        )
    }

    pub fn load(&self, uuid: String, path: String, asset_type: AssetType, is_normal_map: bool) {
        let _ = self.sender.send(LoadRequest {
            uuid,
            path,
            asset_type,
            is_normal_map,
        });
    }
}

enum BufferSource {
    Bin,
    Uri(String),
}

async fn load_mesh(vfs: &Vfs, req: LoadRequest) -> AssetData {
    // Determine extension from virtual path
    let path_str = req.path.as_str();
    let ext = std::path::Path::new(path_str)
        .extension()
        .and_then(|s| s.to_str())
        .unwrap_or("")
        .to_lowercase();

    if ext == "gltf" || ext == "glb" {
        // Step 1: Read main file
        let bytes = match vfs.read_file(&req.path).await {
            Ok(b) => b,
            Err(e) => {
                return AssetData::Error {
                    uuid: req.uuid,
                    error: e.to_string(),
                }
            }
        };

        // Step 2: Parse to get buffer URIs (lightweight parse)
        let buffer_sources: Result<Vec<BufferSource>, _> = {
            let gltf = gltf::Gltf::from_slice(&bytes);
            match gltf {
                Ok(g) => Ok(g
                    .document
                    .buffers()
                    .map(|b| match b.source() {
                        gltf::buffer::Source::Bin => BufferSource::Bin,
                        gltf::buffer::Source::Uri(u) => BufferSource::Uri(u.to_string()),
                    })
                    .collect()),
                Err(e) => Err(e),
            }
        };

        let buffer_sources = match buffer_sources {
            Ok(bs) => bs,
            Err(e) => {
                return AssetData::Error {
                    uuid: req.uuid,
                    error: e.to_string(),
                }
            }
        };

        // Step 3: Load external buffers
        let base_path = Path::new(&req.path)
            .parent()
            .unwrap_or(Path::new(""))
            .to_path_buf();
        let mut loaded_buffers = Vec::new();

        for source in buffer_sources {
            match source {
                BufferSource::Bin => {
                    loaded_buffers.push(Vec::new()); // Placeholder, will be filled from blob in blocking task
                }
                BufferSource::Uri(uri) => {
                    let uri_path = base_path.join(&uri);
                    let uri_str = uri_path.to_str().unwrap().replace("\\", "/");
                    match vfs.read_file(&uri_str).await {
                        Ok(data) => loaded_buffers.push(data),
                        Err(e) => {
                            return AssetData::Error {
                                uuid: req.uuid,
                                error: format!("Failed to load buffer {}: {}", uri, e),
                            }
                        }
                    }
                }
            }
        }

        // Step 4: Process Geometry in Blocking Task
        let uuid = req.uuid.clone();
        tokio::task::spawn_blocking(move || {
            // Parse again
            let gltf = match gltf::Gltf::from_slice(&bytes) {
                Ok(g) => g,
                Err(e) => {
                    return AssetData::Error {
                        uuid,
                        error: e.to_string(),
                    }
                }
            };

            let blob = gltf.blob.clone();

            // Prepare buffers for reader
            let mut final_buffers = Vec::new();
            for (i, buf_data) in loaded_buffers.iter().enumerate() {
                // Check if this index was Bin
                let is_bin = matches!(
                    gltf.document.buffers().nth(i).unwrap().source(),
                    gltf::buffer::Source::Bin
                );

                if is_bin {
                    if let Some(b) = &blob {
                        final_buffers.push(gltf::buffer::Data(b.to_vec()));
                    } else {
                        // Fallback for valid GLB where binary chunk exists but source might be ambiguous
                        final_buffers.push(gltf::buffer::Data(Vec::new()));
                    }
                } else {
                    final_buffers.push(gltf::buffer::Data(buf_data.clone()));
                }
            }

            // Iterate Meshes (Flattening all meshes into one for now)
            // TODO: Proper scene graph support
            let mut vertices = Vec::new();
            let mut indices = Vec::new();
            let mut offset = 0;

            for mesh in gltf.document.meshes() {
                for primitive in mesh.primitives() {
                    let reader = primitive.reader(|buffer| Some(&final_buffers[buffer.index()]));

                    let positions: Vec<[f32; 3]> = reader
                        .read_positions()
                        .map(|iter| iter.collect())
                        .unwrap_or_default();
                    let normals: Vec<[f32; 3]> = reader
                        .read_normals()
                        .map(|iter| iter.collect())
                        .unwrap_or_default();
                    let tex_coords: Vec<[f32; 2]> = reader
                        .read_tex_coords(0)
                        .map(|iter| iter.into_f32().collect())
                        .unwrap_or_default();
                    let prim_indices: Vec<u32> = reader
                        .read_indices()
                        .map(|iter| iter.into_u32().collect())
                        .unwrap_or_default();

                    let count = positions.len();
                    for i in 0..count {
                        let pos = positions[i];
                        let norm = if i < normals.len() {
                            normals[i]
                        } else {
                            [0.0, 1.0, 0.0]
                        };
                        let tex = if i < tex_coords.len() {
                            tex_coords[i]
                        } else {
                            [0.0, 0.0]
                        };

                        // Apply transform if node has one?
                        // Currently we flatten meshes but ignore node transforms in this flattened mesh.
                        // This means the mesh is in "mesh local space", not "node local space".

                        vertices.push(engine_render::Vertex {
                            position: pos,
                            normal: norm,
                            color: [1.0, 1.0, 1.0],
                            tex_coords: tex,
                            joints: [0; 4],
                            weights: [0.0; 4],
                        });
                    }

                    for idx in prim_indices {
                        indices.push(idx + offset);
                    }
                    offset += count as u32;
                }
            }
            AssetData::Mesh {
                uuid,
                vertices,
                indices,
            }
        })
        .await
        .unwrap_or_else(|e| AssetData::Error {
            uuid: req.uuid,
            error: e.to_string(),
        })
    } else {
        // Legacy OBJ loading via bytes
        match vfs.read_file(&req.path).await {
            Ok(bytes) => {
                let uuid = req.uuid.clone();
                tokio::task::spawn_blocking(move || {
                    let mut cursor = std::io::Cursor::new(bytes);
                    let load_opts = tobj::LoadOptions {
                        single_index: true,
                        triangulate: true,
                        ..Default::default()
                    };

                    match tobj::load_obj_buf(&mut cursor, &load_opts, |_| Ok(Default::default())) {
                        Ok((models, _)) => {
                            let mut vertices = Vec::new();
                            let mut indices = Vec::new();
                            let mut offset = 0;

                            for model in models {
                                let mesh = model.mesh;
                                let count = mesh.positions.len() / 3;

                                for i in 0..count {
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

                                    vertices.push(engine_render::Vertex {
                                        position: [px, py, pz],
                                        normal: [nx, ny, nz],
                                        color: [1.0, 1.0, 1.0],
                                        tex_coords: [tx, ty],
                                        joints: [0; 4],
                                        weights: [0.0; 4],
                                    });
                                }
                                for idx in mesh.indices {
                                    indices.push(idx + offset);
                                }
                                offset += count as u32;
                            }
                            AssetData::Mesh {
                                uuid,
                                vertices,
                                indices,
                            }
                        }
                        Err(e) => AssetData::Error {
                            uuid,
                            error: e.to_string(),
                        },
                    }
                })
                .await
                .unwrap_or_else(|e| AssetData::Error {
                    uuid: req.uuid,
                    error: e.to_string(),
                })
            }
            Err(e) => AssetData::Error {
                uuid: req.uuid,
                error: e.to_string(),
            },
        }
    }
}

async fn load_texture(vfs: &Vfs, req: LoadRequest) -> AssetData {
    match vfs.read_file(&req.path).await {
        Ok(bytes) => {
            let uuid = req.uuid.clone();
            let is_normal = req.is_normal_map;
            let path = req.path.clone();
            tokio::task::spawn_blocking(move || {
                // Check for DDS format
                let ext = std::path::Path::new(&path)
                    .extension()
                    .and_then(|s| s.to_str())
                    .unwrap_or("")
                    .to_lowercase();

                if ext == "dds" {
                    // Parse DDS header (128 bytes min)
                    if bytes.len() < 128 + 4 {
                        return AssetData::Error {
                            uuid,
                            error: "DDS file too small".into(),
                        };
                    }
                    // Verify magic "DDS "
                    if &bytes[0..4] != b"DDS " {
                        return AssetData::Error {
                            uuid,
                            error: "Invalid DDS magic".into(),
                        };
                    }
                    let height = u32::from_le_bytes([bytes[12], bytes[13], bytes[14], bytes[15]]);
                    let width = u32::from_le_bytes([bytes[16], bytes[17], bytes[18], bytes[19]]);
                    let rgb_bit_count =
                        u32::from_le_bytes([bytes[88], bytes[89], bytes[90], bytes[91]]);

                    let header_size = 128; // 4 (magic) + 124 (header)
                    let pixel_data = &bytes[header_size..];

                    // Only handle uncompressed RGBA (32-bit) and RGB (24-bit)
                    let img = if rgb_bit_count == 32 {
                        // RGBA
                        let expected = (width * height * 4) as usize;
                        if pixel_data.len() < expected {
                            return AssetData::Error {
                                uuid,
                                error: format!("DDS data too short for {}x{} RGBA", width, height),
                            };
                        }
                        image::DynamicImage::ImageRgba8(
                            image::RgbaImage::from_raw(
                                width,
                                height,
                                pixel_data[..expected].to_vec(),
                            )
                            .unwrap_or_else(|| image::RgbaImage::new(1, 1)),
                        )
                    } else if rgb_bit_count == 24 {
                        // RGB
                        let expected = (width * height * 3) as usize;
                        if pixel_data.len() < expected {
                            return AssetData::Error {
                                uuid,
                                error: format!("DDS data too short for {}x{} RGB", width, height),
                            };
                        }
                        image::DynamicImage::ImageRgb8(
                            image::RgbImage::from_raw(
                                width,
                                height,
                                pixel_data[..expected].to_vec(),
                            )
                            .unwrap_or_else(|| image::RgbImage::new(1, 1)),
                        )
                    } else {
                        return AssetData::Error {
                            uuid,
                            error: format!("Unsupported DDS format: {} bpp", rgb_bit_count),
                        };
                    };

                    AssetData::Texture {
                        uuid,
                        img,
                        is_normal_map: is_normal,
                    }
                } else {
                    // Standard image formats (PNG, JPG, etc.)
                    match image::load_from_memory(&bytes) {
                        Ok(img) => AssetData::Texture {
                            uuid,
                            img,
                            is_normal_map: is_normal,
                        },
                        Err(e) => AssetData::Error {
                            uuid,
                            error: e.to_string(),
                        },
                    }
                }
            })
            .await
            .unwrap_or_else(|e| AssetData::Error {
                uuid: req.uuid,
                error: e.to_string(),
            })
        }
        Err(e) => AssetData::Error {
            uuid: req.uuid,
            error: e.to_string(),
        },
    }
}

async fn load_skybox(vfs: &Vfs, req: LoadRequest) -> AssetData {
    let faces = ["px.png", "nx.png", "py.png", "ny.png", "pz.png", "nz.png"];
    let mut images = Vec::new();

    let base = if req.path.ends_with('/') {
        req.path.clone()
    } else {
        format!("{}/", req.path)
    };

    for face in faces {
        let full_path = format!("{}{}", base, face);
        match vfs.read_file(&full_path).await {
            Ok(bytes) => match image::load_from_memory(&bytes) {
                Ok(img) => images.push(img.to_rgba8()),
                Err(_) => {
                    images.push(image::ImageBuffer::from_pixel(
                        1,
                        1,
                        image::Rgba([255, 0, 255, 255]),
                    ));
                }
            },
            Err(_) => {
                images.push(image::ImageBuffer::from_pixel(
                    1,
                    1,
                    image::Rgba([255, 0, 255, 255]),
                ));
            }
        }
    }

    AssetData::Skybox {
        uuid: req.uuid,
        images,
    }
}

async fn load_gltf_scene(
    vfs: &Vfs,
    req: LoadRequest,
    sender: UnboundedSender<AssetData>,
) -> AssetData {
    // 1. Read file
    let bytes = match vfs.read_file(&req.path).await {
        Ok(b) => b,
        Err(e) => {
            return AssetData::Error {
                uuid: req.uuid,
                error: e.to_string(),
            }
        }
    };

    // 2. Parse buffers
    let buffer_sources: Result<Vec<BufferSource>, _> = {
        let gltf = gltf::Gltf::from_slice(&bytes);
        match gltf {
            Ok(g) => Ok(g
                .document
                .buffers()
                .map(|b| match b.source() {
                    gltf::buffer::Source::Bin => BufferSource::Bin,
                    gltf::buffer::Source::Uri(u) => BufferSource::Uri(u.to_string()),
                })
                .collect()),
            Err(e) => Err(e),
        }
    };

    let buffer_sources = match buffer_sources {
        Ok(bs) => bs,
        Err(e) => {
            return AssetData::Error {
                uuid: req.uuid,
                error: e.to_string(),
            }
        }
    };

    let base_path = Path::new(&req.path)
        .parent()
        .unwrap_or(Path::new(""))
        .to_path_buf();
    let mut loaded_buffers = Vec::new();

    for source in buffer_sources {
        match source {
            BufferSource::Bin => {
                loaded_buffers.push(Vec::new());
            }
            BufferSource::Uri(uri) => {
                let uri_path = base_path.join(&uri);
                let uri_str = uri_path.to_str().unwrap().replace("\\", "/");
                match vfs.read_file(&uri_str).await {
                    Ok(data) => loaded_buffers.push(data),
                    Err(e) => {
                        return AssetData::Error {
                            uuid: req.uuid,
                            error: format!("Failed buffer {}: {}", uri, e),
                        }
                    }
                }
            }
        }
    }

    // 3. Blocking task
    let uuid = req.uuid.clone();
    tokio::task::spawn_blocking(move || {
        let gltf = match gltf::Gltf::from_slice(&bytes) {
            Ok(g) => g,
            Err(e) => {
                return AssetData::Error {
                    uuid,
                    error: e.to_string(),
                }
            }
        };

        let blob = gltf.blob.clone();

        let mut final_buffers = Vec::new();
        for (i, buf_data) in loaded_buffers.iter().enumerate() {
            let is_bin = matches!(
                gltf.document.buffers().nth(i).unwrap().source(),
                gltf::buffer::Source::Bin
            );

            if is_bin {
                if let Some(b) = &blob {
                    final_buffers.push(gltf::buffer::Data(b.to_vec()));
                } else {
                    final_buffers.push(gltf::buffer::Data(Vec::new()));
                }
            } else {
                final_buffers.push(gltf::buffer::Data(buf_data.clone()));
            }
        }

        // Map GLTF Mesh Index -> Generated UUID
        let mut mesh_map = std::collections::HashMap::new();

        for (i, mesh) in gltf.document.meshes().enumerate() {
            let mesh_uuid = format!("{}/mesh_{}", uuid, i);

            let mut vertices = Vec::new();
            let mut indices = Vec::new();
            let mut offset = 0;

            for primitive in mesh.primitives() {
                let reader = primitive.reader(|buffer| Some(&final_buffers[buffer.index()]));

                let positions: Vec<[f32; 3]> = reader
                    .read_positions()
                    .map(|iter| iter.collect())
                    .unwrap_or_default();
                let normals: Vec<[f32; 3]> = reader
                    .read_normals()
                    .map(|iter| iter.collect())
                    .unwrap_or_default();
                let tex_coords: Vec<[f32; 2]> = reader
                    .read_tex_coords(0)
                    .map(|iter| iter.into_f32().collect())
                    .unwrap_or_default();
                let prim_indices: Vec<u32> = reader
                    .read_indices()
                    .map(|iter| iter.into_u32().collect())
                    .unwrap_or_default();

                let count = positions.len();
                for k in 0..count {
                    let pos = positions[k];
                    let norm = if k < normals.len() {
                        normals[k]
                    } else {
                        [0.0, 1.0, 0.0]
                    };
                    let tex = if k < tex_coords.len() {
                        tex_coords[k]
                    } else {
                        [0.0, 0.0]
                    };
                    vertices.push(engine_render::Vertex {
                        position: pos,
                        normal: norm,
                        color: [1.0, 1.0, 1.0],
                        tex_coords: tex,
                        joints: [0; 4],
                        weights: [0.0; 4],
                    });
                }

                for idx in prim_indices {
                    indices.push(idx + offset);
                }
                offset += count as u32;
            }

            if !vertices.is_empty() {
                let _ = sender.send(AssetData::Mesh {
                    uuid: mesh_uuid.clone(),
                    vertices,
                    indices,
                });
                mesh_map.insert(i, mesh_uuid);
            }
        }

        let mut nodes = Vec::new();
        for node in gltf.document.nodes() {
            let (translation, rotation, scale) = node.transform().decomposed();
            let t_vec = Vec3::from(translation);
            let r_quat = Quat::from_array(rotation);
            let s_vec = Vec3::from(scale);

            let mesh_id = node.mesh().and_then(|m| mesh_map.get(&m.index()).cloned());
            let children: Vec<usize> = node.children().map(|c| c.index()).collect();

            nodes.push(SceneNode {
                name: node.name().unwrap_or("").to_string(),
                translation: t_vec,
                rotation: r_quat,
                scale: s_vec,
                mesh_id,
                children,
            });
        }

        let root_indices: Vec<usize> = gltf
            .document
            .scenes()
            .next()
            .map(|s| s.nodes().map(|n| n.index()).collect())
            .unwrap_or_default();

        AssetData::Scene {
            uuid,
            graph: SceneGraph {
                nodes,
                root_indices,
            },
        }
    })
    .await
    .unwrap_or_else(|e| AssetData::Error {
        uuid: req.uuid,
        error: e.to_string(),
    })
}
