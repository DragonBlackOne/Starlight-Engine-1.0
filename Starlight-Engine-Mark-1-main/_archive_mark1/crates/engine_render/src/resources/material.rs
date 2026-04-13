use bytemuck::{Pod, Zeroable};

#[repr(C)]
#[derive(Copy, Clone, Debug, Pod, Zeroable)]
pub struct MaterialUniform {
    pub metallic: f32,
    pub roughness: f32,
    pub _padding: [f32; 2],
}

#[repr(C)]
#[derive(Copy, Clone, Debug, Pod, Zeroable)]
pub struct PointLight {
    pub position: [f32; 4],
    pub color: [f32; 4],
}
impl Default for PointLight {
    fn default() -> Self {
        Self {
            position: [0.0; 4],
            color: [0.0; 4],
        }
    }
}

#[repr(C)]
#[derive(Copy, Clone, Debug, Pod, Zeroable)]
pub struct LightUniform {
    pub direction: [f32; 3],
    pub fog_density: f32,
    pub color: [f32; 3],
    pub ambient: f32,
    pub sun_intensity: f32,
    pub _padding_sun: [f32; 3],
    pub cascade_splits: [f32; 4],
    pub shadow_matrices: [[[f32; 4]; 4]; 4],
    pub point_light_count: u32,
    pub fog_color: [f32; 3],
}

#[repr(C)]
#[derive(Copy, Clone, Debug, Pod, Zeroable)]
pub struct Uniforms {
    pub view_proj: [[f32; 4]; 4],
    pub model: [[f32; 4]; 4],
    pub camera_pos: [f32; 4],
    pub time: f32,
    pub wind_strength: f32,
    pub _padding: [f32; 2],
}
impl Uniforms {
    pub fn new() -> Self {
        use glam::Mat4;
        Self {
            view_proj: Mat4::IDENTITY.to_cols_array_2d(),
            model: Mat4::IDENTITY.to_cols_array_2d(),
            camera_pos: [0.0; 4],
            time: 0.0,
            wind_strength: 0.0,
            _padding: [0.0; 2],
        }
    }
}

#[repr(C)]
#[derive(Copy, Clone, Debug, Pod, Zeroable)]
pub struct PostProcessUniform {
    pub exposure: f32,
    pub gamma: f32,
    pub bloom_intensity: f32,
    pub bloom_threshold: f32,
    pub chromatic_aberration: f32,
    pub time: f32,
    // Volumetric Fog params
    pub fog_density: f32,
    pub fog_height_falloff: f32,
    pub fog_scatter_intensity: f32,
    pub fog_max_distance: f32,
    // Camera data for world-space reconstruction
    pub camera_pos: [f32; 4],
    pub inv_view_proj: [[f32; 4]; 4],
    // Light direction for in-scattering
    pub light_direction: [f32; 4],
    pub fog_color: [f32; 4],
}
impl Default for PostProcessUniform {
    fn default() -> Self {
        use glam::Mat4;
        Self {
            exposure: 1.0,
            gamma: 2.2,
            bloom_intensity: 0.0,
            bloom_threshold: 1.0,
            chromatic_aberration: 0.0,
            time: 0.0,
            fog_density: 0.0,
            fog_height_falloff: 0.05,
            fog_scatter_intensity: 0.3,
            fog_max_distance: 300.0,
            camera_pos: [0.0; 4],
            inv_view_proj: Mat4::IDENTITY.to_cols_array_2d(),
            light_direction: [0.0, -1.0, 0.0, 0.0],
            fog_color: [0.7, 0.75, 0.8, 1.0],
        }
    }
}

#[repr(C)]
#[derive(Copy, Clone, Debug, Pod, Zeroable)]
pub struct JointUniform {
    pub matrices: [[[f32; 4]; 4]; 64], // Support up to 64 joints per mesh
}

impl Default for JointUniform {
    fn default() -> Self {
        use glam::Mat4;
        let mut matrices = [[[0.0; 4]; 4]; 64];
        for i in 0..64 {
            matrices[i] = Mat4::IDENTITY.to_cols_array_2d();
        }
        Self { matrices }
    }
}
