use bevy_ecs::prelude::*;
use glam::Mat4;
use pyo3::prelude::*;
use rapier3d::prelude::{ColliderHandle, RigidBodyHandle};

#[derive(Component, Debug, Clone, Copy)]
#[pyclass]
pub struct Parent(pub u64);

#[pymethods]
impl Parent {
    #[new]
    pub fn new(id: u64) -> Self {
        Parent(id)
    }
}

#[derive(Component, Debug, Clone)]
#[pyclass]
pub struct Transform {
    #[pyo3(get, set)]
    pub x: f32,
    #[pyo3(get, set)]
    pub y: f32,
    #[pyo3(get, set)]
    pub z: f32,
}

#[pymethods]
impl Transform {
    #[new]
    pub fn new(x: f32, y: f32, z: f32) -> Self {
        Self { x, y, z }
    }
    fn __repr__(&self) -> String {
        format!(
            "Transform(x={:.2}, y={:.2}, z={:.2})",
            self.x, self.y, self.z
        )
    }
}

#[derive(Component, Debug, Clone)]
#[pyclass]
pub struct Rotation {
    #[pyo3(get, set)]
    pub x: f32,
    #[pyo3(get, set)]
    pub y: f32,
    #[pyo3(get, set)]
    pub z: f32,
}

#[pymethods]
impl Rotation {
    #[new]
    pub fn new(x: f32, y: f32, z: f32) -> Self {
        Self { x, y, z }
    }
}

#[derive(Component, Debug, Clone)]
#[pyclass]
pub struct Scale {
    #[pyo3(get, set)]
    pub x: f32,
    #[pyo3(get, set)]
    pub y: f32,
    #[pyo3(get, set)]
    pub z: f32,
}

#[pymethods]
impl Scale {
    #[new]
    pub fn new(x: f32, y: f32, z: f32) -> Self {
        Self { x, y, z }
    }
}

#[derive(Component, Debug, Clone, Default)]
pub struct GlobalTransform(pub Mat4);

#[derive(Component, Debug, Clone, Copy)]
#[pyclass]
pub struct Color {
    #[pyo3(get, set)]
    pub r: f32,
    #[pyo3(get, set)]
    pub g: f32,
    #[pyo3(get, set)]
    pub b: f32,
    #[pyo3(get, set)]
    pub a: f32,
}

#[pymethods]
impl Color {
    #[new]
    pub fn new(r: f32, g: f32, b: f32, a: f32) -> Self {
        Color { r, g, b, a }
    }
}

// Internal components for interpolation
#[derive(Component, Debug, Clone)]
pub struct PreviousTransform {
    pub x: f32,
    pub y: f32,
    pub z: f32,
}

#[derive(Component, Debug, Clone)]
pub struct PreviousRotation {
    pub x: f32,
    pub y: f32,
    pub z: f32,
}

#[derive(Component, Debug, Clone)]
#[pyclass]
pub struct MeshName(pub String);

#[derive(Component, Debug, Clone)]
#[pyclass]
pub struct Material {
    #[pyo3(get, set)]
    pub diffuse: String,
    #[pyo3(get, set)]
    pub normal: String,
    #[pyo3(get, set)]
    pub metallic: f32,
    #[pyo3(get, set)]
    pub roughness: f32,
}

#[pymethods]
impl Material {
    #[new]
    #[pyo3(signature = (diffuse, normal="flat_normal".to_string(), metallic=0.0, roughness=0.5))]
    pub fn new(diffuse: String, normal: String, metallic: f32, roughness: f32) -> Self {
        Self {
            diffuse,
            normal,
            metallic,
            roughness,
        }
    }
}

#[derive(Component, Debug, Clone)]
#[pyclass]
pub struct Camera {
    #[pyo3(get, set)]
    pub fov: f32,
    #[pyo3(get, set)]
    pub near: f32,
    #[pyo3(get, set)]
    pub far: f32,
}

#[pymethods]
impl Camera {
    #[new]
    pub fn new(fov: f32, near: f32, far: f32) -> Self {
        Self { fov, near, far }
    }
}

// Physics Components
#[derive(Component)]
pub struct RigidBodyComponent(pub RigidBodyHandle);

#[derive(Component)]
pub struct ColliderComponent(pub ColliderHandle);

#[derive(Component, Debug, Clone)]
#[pyclass]
pub struct CharacterController {
    #[pyo3(get, set)]
    pub speed: f32,
    #[pyo3(get, set)]
    pub max_slope_angle: f32, // Radians
    #[pyo3(get, set)]
    pub offset: f32,
}

#[pymethods]
impl CharacterController {
    #[new]
    pub fn new(speed: f32, max_slope_angle: f32, offset: f32) -> Self {
        Self {
            speed,
            max_slope_angle,
            offset,
        }
    }
}

// Game Attributes
#[derive(Component, Debug, Clone)]
#[pyclass]
pub struct Health {
    #[pyo3(get, set)]
    pub current: f32,
    #[pyo3(get, set)]
    pub max: f32,
}

#[pymethods]
impl Health {
    #[new]
    pub fn new(current: f32, max: f32) -> Self {
        Self { current, max }
    }
}

#[derive(Component, Debug, Clone)]
#[pyclass]
pub struct Mana {
    #[pyo3(get, set)]
    pub current: f32,
    #[pyo3(get, set)]
    pub max: f32,
}

#[pymethods]
impl Mana {
    #[new]
    pub fn new(current: f32, max: f32) -> Self {
        Self { current, max }
    }
}
#[derive(Component, Debug, Clone)]
#[pyclass]
pub struct ParticleEmitter {
    #[pyo3(get, set)]
    pub count: u32,
    #[pyo3(get, set)]
    pub origin: [f32; 3],
    #[pyo3(get, set)]
    pub speed: f32,
    #[pyo3(get, set)]
    pub spread: f32,
    #[pyo3(get, set)]
    pub life_base: f32,
    #[pyo3(get, set)]
    pub size_base: f32,
    #[pyo3(get, set)]
    pub color: [f32; 4],
}

#[pymethods]
impl ParticleEmitter {
    #[new]
    #[pyo3(signature = (count, origin=[0.0; 3], speed=1.0, spread=0.5, life_base=2.0, size_base=0.1, color=[1.0, 1.0, 1.0, 1.0]))]
    pub fn new(
        count: u32,
        origin: [f32; 3],
        speed: f32,
        spread: f32,
        life_base: f32,
        size_base: f32,
        color: [f32; 4],
    ) -> Self {
        Self {
            count,
            origin,
            speed,
            spread,
            life_base,
            size_base,
            color,
        }
    }
}

// Animation Components
#[derive(Component, Debug, Clone)]
#[pyclass]
pub struct AnimationController {
    #[pyo3(get, set)]
    pub current_animation: Option<usize>,
    #[pyo3(get, set)]
    pub current_time: f32,
    #[pyo3(get, set)]
    pub playing: bool,
    #[pyo3(get, set)]
    pub looping: bool,
    #[pyo3(get, set)]
    pub speed: f32,
}

impl Default for AnimationController {
    fn default() -> Self {
        Self {
            current_animation: None,
            current_time: 0.0,
            playing: true,
            looping: true,
            speed: 1.0,
        }
    }
}

#[pymethods]
impl AnimationController {
    #[new]
    #[pyo3(signature = (current_animation=None, playing=true, looping=true, speed=1.0))]
    pub fn new(current_animation: Option<usize>, playing: bool, looping: bool, speed: f32) -> Self {
        Self {
            current_animation,
            current_time: 0.0,
            playing,
            looping,
            speed,
        }
    }

    pub fn play(&mut self, animation_index: usize, looping: bool) {
        self.current_animation = Some(animation_index);
        self.current_time = 0.0;
        self.playing = true;
        self.looping = looping;
    }

    pub fn stop(&mut self) {
        self.playing = false;
    }
}

#[derive(Component, Debug, Clone)]
pub struct Skeleton(pub engine_render::resources::mesh::Skeleton);
