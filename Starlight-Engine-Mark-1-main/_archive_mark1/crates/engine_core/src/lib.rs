use bevy_ecs::prelude::*;
use glam::Vec3;
use pyo3::exceptions::{PyIOError, PyRuntimeError, PyValueError};
use pyo3::prelude::*;
use pyo3::types::PyModule;
use rapier3d::control::{CharacterLength, KinematicCharacterController};
use rapier3d::prelude as rapier;
use std::sync::Arc;

pub mod animation;
pub mod app;
pub mod assets;
pub mod ecs;
pub mod input;
pub mod loader;
pub mod math;
pub mod network;
pub mod particles;
pub mod physics;
pub mod procedural;
pub mod vfs;

use app::{GameApp, StarlightRunner, GAME_APP, HEADLESS_RUNTIME, MAIN_CAMERA_ID};
use assets::{AssetType, LoadState, SceneRegistry};
use ecs::components::*;
use engine_render::Vertex;
use input::{AxisBinding, InputSource, InputState};
use network::NetworkState;
use physics::{CollisionEvents, PhysicsState};

use gilrs::{Axis, Button};

// Helper to convert Vec3 to Rapier Vector
fn vec3_to_vector(v: Vec3) -> rapier::Vector<rapier::Real> {
    rapier::Vector::new(v.x, v.y, v.z)
}

#[pyfunction]
#[pyo3(signature = (title, width, height, _script_path, on_update, on_fixed_update, on_render))]
fn run_engine(
    title: String,
    width: u32,
    height: u32,
    _script_path: String,
    on_update: PyObject,
    on_fixed_update: Option<PyObject>,
    on_render: PyObject,
) -> PyResult<()> {
    let event_loop = winit::event_loop::EventLoop::new().unwrap();
    let mut runner = StarlightRunner {
        title,
        width,
        height,
        window: None,
        on_update,
        on_fixed_update,
        on_render,
        last_time: std::time::Instant::now(),
        accumulator: 0.0,
        gilrs: None,
        asset_loader: None,
        runtime: None,
    };
    let result = event_loop.run_app(&mut runner);

    // Cleanup globals
    {
        let mut guard = GAME_APP.lock().unwrap();
        *guard = None;
    }
    {
        let mut cam = MAIN_CAMERA_ID.lock().unwrap();
        *cam = None;
    }

    match result {
        Ok(_) => Ok(()),
        Err(e) => Err(PyRuntimeError::new_err(format!("Event loop error: {}", e))),
    }
}

#[pyfunction]
fn init_headless(width: u32, height: u32) -> PyResult<()> {
    let rt = tokio::runtime::Runtime::new().unwrap();
    let _guard = rt.enter();

    let vfs = vfs::Vfs::new();
    if let Ok(cwd) = std::env::current_dir() {
        let _ = vfs.mount("/", cwd);
    }
    let (loader, loader_rx) = loader::AsyncAssetLoader::new(vfs.clone());
    let loader_tx = loader.sender.clone();

    let render_state =
        rt.block_on(async { engine_render::RenderState::new(None, width, height).await });
    let audio_state = engine_audio::AudioState::new();

    *HEADLESS_RUNTIME.lock().unwrap() = Some(rt);

    let mut world = World::default();
    let mut schedule = Schedule::default();
    let mut fixed_schedule = Schedule::default();

    world.insert_resource(InputState::default());
    world.insert_resource(PhysicsState::default());
    world.insert_resource(SceneRegistry::default());

    schedule.add_systems((
        input::input_reset_system
            .before(input::action_update_system)
            .before(ecs::systems::camera_movement_system),
        input::action_update_system,
        input::action_update_system,
        ecs::systems::camera_movement_system,
        animation::animation_update_system,
        ecs::systems::transform_propagation_system,
        ecs::systems::transform_propagation_system,
        apply_deferred,
    ));
    fixed_schedule.add_systems((
        ecs::systems::snapshot_previous_state_system.before(physics::physics_step_system),
        physics::physics_step_system,
        ecs::systems::sync_transforms_system,
    ));

    // Spawn camera
    let cam_id = world
        .spawn((
            Camera::new(60.0f32.to_radians(), 0.1, 1000.0),
            Transform::new(0.0, 5.0, 10.0),
            Rotation::new(-0.2, 0.0, 0.0),
            PreviousTransform {
                x: 0.0,
                y: 5.0,
                z: 10.0,
            },
            PreviousRotation {
                x: -0.2,
                y: 0.0,
                z: 0.0,
            },
        ))
        .id();

    {
        let mut cam_guard = MAIN_CAMERA_ID.lock().unwrap();
        *cam_guard = Some(cam_id.to_bits());
    }

    let app = GameApp {
        world,
        schedule,
        fixed_schedule,
        render_state: Some(render_state),
        audio_state: Some(audio_state),
        asset_registry: assets::AssetRegistry::new(),
        asset_loader_rx: Some(loader_rx),
        asset_loader_tx: Some(loader_tx),
        vfs,
        network_state: Some(NetworkState::new()),
    };
    let mut guard = GAME_APP.lock().unwrap();
    *guard = Some(app);
    Ok(())
}

#[pyfunction]
#[pyo3(signature = (entity_id, count, origin=[0.0; 3]))]
fn add_particle_emitter(entity_id: u64, count: u32, origin: [f32; 3]) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let entity = Entity::from_bits(entity_id);
        app.world.entity_mut(entity).insert(ParticleEmitter::new(
            count,
            origin,
            1.0,
            0.5,
            2.0,
            0.1,
            [1.0, 1.0, 1.0, 1.0],
        ));
        Ok(())
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

// --- Bindings ---

#[pyfunction]
fn get_main_camera_id() -> PyResult<u64> {
    let guard = MAIN_CAMERA_ID.lock().unwrap();
    if let Some(id) = *guard {
        Ok(id)
    } else {
        Err(PyRuntimeError::new_err("Camera not initialized"))
    }
}

#[pyfunction]
fn set_cursor_grab(_grab: bool) -> PyResult<()> {
    Ok(())
}
#[pyfunction]
fn set_cursor_visible(_visible: bool) -> PyResult<()> {
    Ok(())
}

#[pyfunction]
#[pyo3(signature = (path, looped=false, volume=1.0, panning=0.5))]
fn play_sound(path: String, looped: bool, volume: f64, panning: f64) -> PyResult<u64> {
    let guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_ref() {
        if let Some(audio) = app.audio_state.as_ref() {
            Ok(audio.play_sound(&path, looped, volume, panning))
        } else {
            Err(PyRuntimeError::new_err("Audio system not initialized"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn set_sound_volume(id: u64, volume: f64) -> PyResult<()> {
    let guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_ref() {
        if let Some(audio) = app.audio_state.as_ref() {
            audio.set_volume(id, volume);
            Ok(())
        } else {
            Err(PyRuntimeError::new_err("Audio system not initialized"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn set_sound_panning(id: u64, panning: f64) -> PyResult<()> {
    let guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_ref() {
        if let Some(audio) = app.audio_state.as_ref() {
            audio.set_panning(id, panning);
            Ok(())
        } else {
            Err(PyRuntimeError::new_err("Audio system not initialized"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn stop_sound(id: u64) -> PyResult<()> {
    let guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_ref() {
        if let Some(audio) = app.audio_state.as_ref() {
            audio.stop(id);
            Ok(())
        } else {
            Err(PyRuntimeError::new_err("Audio system not initialized"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn load_mesh(path: String) -> PyResult<String> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let id = app.asset_registry.get_or_create_id(&path, AssetType::Mesh);
        if app.asset_registry.get_state(&id) != LoadState::Unloaded {
            return Ok(id);
        }
        app.asset_registry.mark_loading(&id);
        if let Some(tx) = &app.asset_loader_tx {
            let _ = tx.send(loader::LoadRequest {
                uuid: id.clone(),
                path: path.clone(),
                asset_type: AssetType::Mesh,
                is_normal_map: false,
            });
        }
        Ok(id)
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
#[pyo3(signature = (path, is_normal=None))]
fn load_texture(path: String, is_normal: Option<bool>) -> PyResult<String> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let id = app
            .asset_registry
            .get_or_create_id(&path, AssetType::Texture);
        if app.asset_registry.get_state(&id) != LoadState::Unloaded {
            return Ok(id);
        }
        app.asset_registry.mark_loading(&id);
        if let Some(tx) = &app.asset_loader_tx {
            let _ = tx.send(loader::LoadRequest {
                uuid: id.clone(),
                path: path.clone(),
                asset_type: AssetType::Texture,
                is_normal_map: is_normal.unwrap_or(false),
            });
        }
        Ok(id)
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn load_skybox(path: String) -> PyResult<String> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let id = app
            .asset_registry
            .get_or_create_id(&path, AssetType::Skybox);
        if app.asset_registry.get_state(&id) != LoadState::Unloaded {
            return Ok(id);
        }
        app.asset_registry.mark_loading(&id);
        if let Some(tx) = &app.asset_loader_tx {
            let _ = tx.send(loader::LoadRequest {
                uuid: id.clone(),
                path: path.clone(),
                asset_type: AssetType::Skybox,
                is_normal_map: false,
            });
        }
        Ok(id)
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn is_asset_loaded(id: String) -> PyResult<bool> {
    let guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_ref() {
        Ok(app.asset_registry.get_state(&id) == LoadState::Loaded)
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn load_scene(path: String) -> PyResult<String> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let id = app.asset_registry.get_or_create_id(&path, AssetType::Scene);
        if app.asset_registry.get_state(&id) != LoadState::Unloaded {
            return Ok(id);
        }
        app.asset_registry.mark_loading(&id);
        if let Some(tx) = &app.asset_loader_tx {
            let _ = tx.send(loader::LoadRequest {
                uuid: id.clone(),
                path: path.clone(),
                asset_type: AssetType::Scene,
                is_normal_map: false,
            });
        }
        Ok(id)
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
#[pyo3(signature = (scene_id, parent_id=None))]
fn spawn_scene(scene_id: String, parent_id: Option<u64>) -> PyResult<Vec<u64>> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        if app.asset_registry.get_state(&scene_id) != LoadState::Loaded {
            return Err(PyValueError::new_err(format!(
                "Scene {} not loaded",
                scene_id
            )));
        }

        let graph = {
            if let Some(registry) = app.world.get_resource::<SceneRegistry>() {
                if let Some(g) = registry.graphs.get(&scene_id) {
                    g.clone()
                } else {
                    return Err(PyValueError::new_err("Scene data missing from registry"));
                }
            } else {
                return Err(PyValueError::new_err("SceneRegistry not found"));
            }
        };

        let mut node_entities = Vec::with_capacity(graph.nodes.len());
        let mut root_ids = Vec::new();

        for node in &graph.nodes {
            let (rx, ry, rz) = node.rotation.to_euler(glam::EulerRot::XYZ);

            let mut entity_cmd = app.world.spawn((
                Transform::new(node.translation.x, node.translation.y, node.translation.z),
                Rotation::new(rx, ry, rz),
                PreviousTransform {
                    x: node.translation.x,
                    y: node.translation.y,
                    z: node.translation.z,
                },
                PreviousRotation {
                    x: rx,
                    y: ry,
                    z: rz,
                },
                Scale::new(node.scale.x, node.scale.y, node.scale.z),
            ));

            if let Some(mesh_id) = &node.mesh_id {
                entity_cmd.insert(MeshName(mesh_id.clone()));
                entity_cmd.insert(Material {
                    diffuse: "default".to_string(),
                    normal: "flat_normal".to_string(),
                    metallic: 0.0,
                    roughness: 0.5,
                });
            }

            node_entities.push(entity_cmd.id());
        }

        for (i, node) in graph.nodes.iter().enumerate() {
            let entity = node_entities[i];

            for &child_idx in &node.children {
                if child_idx < node_entities.len() {
                    let child_entity = node_entities[child_idx];
                    app.world
                        .entity_mut(child_entity)
                        .insert(Parent::new(entity.to_bits()));
                }
            }
        }

        for &root_idx in &graph.root_indices {
            if root_idx < node_entities.len() {
                let root_entity = node_entities[root_idx];
                root_ids.push(root_entity.to_bits());

                if let Some(pid) = parent_id {
                    app.world.entity_mut(root_entity).insert(Parent::new(pid));
                }
            }
        }

        Ok(root_ids)
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

// Entity Manipulation
#[pyfunction]
fn spawn_entity(x: f32, y: f32, z: f32) -> PyResult<u64> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let id = app
            .world
            .spawn((
                Transform::new(x, y, z),
                Rotation::new(0.0, 0.0, 0.0),
                PreviousTransform { x, y, z },
                PreviousRotation {
                    x: 0.0,
                    y: 0.0,
                    z: 0.0,
                },
                Scale::new(1.0, 1.0, 1.0),
            ))
            .id();
        Ok(id.to_bits())
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn despawn_entity(entity_id: u64) -> PyResult<bool> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let entity = Entity::from_bits(entity_id);

        // Simple check if entity exists
        if app.world.get_entity(entity).is_none() {
            return Ok(false);
        }

        // Cleanup Physics (RigidBodies)
        // Note: Colliders attached to RigidBodies are removed automatically by Rapier when RB is removed.
        // But we store handles in components.
        let mut rb_handle = None;
        if let Some(c) = app.world.get::<RigidBodyComponent>(entity) {
            rb_handle = Some(c.0);
        }

        if let Some(handle) = rb_handle {
            let mut physics = app.world.resource_mut::<PhysicsState>();
            let PhysicsState {
                ref mut rigid_body_set,
                ref mut island_manager,
                ref mut collider_set,
                ref mut impulse_joint_set,
                ref mut multibody_joint_set,
                ..
            } = *physics;

            rigid_body_set.remove(
                handle,
                island_manager,
                collider_set,
                impulse_joint_set,
                multibody_joint_set,
                true,
            );
        }

        // If it has a collider but no rigid body (static collider roughly), we might need direct removal
        // but for now let's assume standard behavior.

        let success = app.world.despawn(entity);
        Ok(success)
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn get_transform(entity_id: u64) -> PyResult<Transform> {
    let guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_ref() {
        let entity = Entity::from_bits(entity_id);
        if let Some(t) = app.world.get::<Transform>(entity) {
            Ok(t.clone())
        } else {
            Err(PyValueError::new_err("No Transform"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn set_transform(entity_id: u64, x: f32, y: f32, z: f32) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let entity = Entity::from_bits(entity_id);
        if let Some(mut transform) = app.world.get_mut::<Transform>(entity) {
            transform.x = x;
            transform.y = y;
            transform.z = z;
            let rb_handle = app.world.get::<RigidBodyComponent>(entity).map(|c| c.0);
            if let Some(handle) = rb_handle {
                let mut physics = app.world.resource_mut::<PhysicsState>();
                let PhysicsState {
                    ref mut rigid_body_set,
                    ref mut island_manager,
                    ref mut collider_set,
                    ref mut impulse_joint_set,
                    ref mut multibody_joint_set,
                    ..
                } = *physics;

                rigid_body_set.remove(
                    handle,
                    island_manager,
                    collider_set,
                    impulse_joint_set,
                    multibody_joint_set,
                    true,
                );
            }
            Ok(())
        } else {
            Err(PyValueError::new_err("Entity has no Transform"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn set_rotation(entity_id: u64, x: f32, y: f32, z: f32) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let entity = Entity::from_bits(entity_id);

        let rb_handle = if let Some(mut rotation) = app.world.get_mut::<Rotation>(entity) {
            rotation.x = x;
            rotation.y = y;
            rotation.z = z;
            app.world.get::<RigidBodyComponent>(entity).map(|c| c.0)
        } else {
            return Err(PyValueError::new_err("Entity has no Rotation"));
        };

        if let Some(handle) = rb_handle {
            let mut physics = app.world.resource_mut::<PhysicsState>();
            let PhysicsState {
                ref mut rigid_body_set,
                ref mut island_manager,
                ref mut collider_set,
                ref mut impulse_joint_set,
                ref mut multibody_joint_set,
                ..
            } = *physics;

            rigid_body_set.remove(
                handle,
                island_manager,
                collider_set,
                impulse_joint_set,
                multibody_joint_set,
                true,
            );
        }
        Ok(())
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn set_scale(entity_id: u64, sx: f32, sy: f32, sz: f32) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let entity = Entity::from_bits(entity_id);
        if let Some(mut s) = app.world.get_mut::<Scale>(entity) {
            s.x = sx;
            s.y = sy;
            s.z = sz;
        } else {
            app.world.entity_mut(entity).insert(Scale::new(sx, sy, sz));
        }
        Ok(())
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn set_color(entity_id: u64, r: f32, g: f32, b: f32, a: f32) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let entity = Entity::from_bits(entity_id);
        if let Some(mut c) = app.world.get_mut::<Color>(entity) {
            c.r = r;
            c.g = g;
            c.b = b;
            c.a = a;
        } else {
            app.world.entity_mut(entity).insert(Color::new(r, g, b, a));
        }
        Ok(())
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn set_mesh(entity_id: u64, mesh_id: String) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let entity = Entity::from_bits(entity_id);
        if let Some(mut m) = app.world.get_mut::<MeshName>(entity) {
            m.0 = mesh_id;
        } else {
            app.world.entity_mut(entity).insert(MeshName(mesh_id));
        }
        Ok(())
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn set_material_textures(entity_id: u64, diffuse_id: String, normal_id: String) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let entity = Entity::from_bits(entity_id);
        if let Some(mut m) = app.world.get_mut::<Material>(entity) {
            m.diffuse = diffuse_id;
            m.normal = normal_id;
        } else {
            app.world.entity_mut(entity).insert(Material {
                diffuse: diffuse_id,
                normal: normal_id,
                metallic: 0.0,
                roughness: 0.5,
            });
        }
        Ok(())
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn set_material_params(entity_id: u64, metallic: f32, roughness: f32) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let entity = Entity::from_bits(entity_id);
        if let Some(mut m) = app.world.get_mut::<Material>(entity) {
            m.metallic = metallic;
            m.roughness = roughness;
        } else {
            app.world.entity_mut(entity).insert(Material {
                diffuse: "default".to_string(),
                normal: "flat_normal".to_string(),
                metallic,
                roughness,
            });
        }
        Ok(())
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn set_parent(child_id: u64, parent_id: u64) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let child = Entity::from_bits(child_id);
        let parent = Entity::from_bits(parent_id);
        app.world
            .entity_mut(child)
            .insert(Parent::new(parent.to_bits()));
        Ok(())
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn get_all_entities() -> PyResult<Vec<u64>> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let mut e = Vec::new();
        for ent in app.world.query::<Entity>().iter(&app.world) {
            e.push(ent.to_bits());
        }
        Ok(e)
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

// Spawners (Legacy wrappers, should be moved to Python factories eventually)
#[pyfunction]
#[pyo3(signature = (x, y, z, r=None, g=None, b=None, friction=None, restitution=None))]
fn spawn_dynamic_cube(
    x: f32,
    y: f32,
    z: f32,
    r: Option<f32>,
    g: Option<f32>,
    b: Option<f32>,
    friction: Option<f32>,
    restitution: Option<f32>,
) -> PyResult<u64> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let mut physics = app.world.resource_mut::<PhysicsState>();
        let rigid_body = rapier::RigidBodyBuilder::dynamic()
            .translation(vec3_to_vector(Vec3::new(x, y, z)))
            .build();
        let collider = rapier::ColliderBuilder::cuboid(0.5, 0.5, 0.5)
            .friction(friction.unwrap_or(0.5))
            .restitution(restitution.unwrap_or(0.7))
            .build();
        let rb_handle = physics.rigid_body_set.insert(rigid_body);
        drop(physics);

        let id = app
            .world
            .spawn((
                Transform::new(x, y, z),
                Rotation::new(0.0, 0.0, 0.0),
                PreviousTransform { x, y, z },
                PreviousRotation {
                    x: 0.0,
                    y: 0.0,
                    z: 0.0,
                },
                Scale::new(1.0, 1.0, 1.0),
                Color::new(r.unwrap_or(1.0), g.unwrap_or(1.0), b.unwrap_or(1.0), 1.0),
                MeshName("cube".to_string()),
                RigidBodyComponent(rb_handle),
            ))
            .id();

        let mut physics = app.world.resource_mut::<PhysicsState>();
        let PhysicsState {
            ref mut collider_set,
            ref mut rigid_body_set,
            ..
        } = *physics;
        let co_handle = collider_set.insert_with_parent(collider, rb_handle, rigid_body_set);

        collider_set.get_mut(co_handle).unwrap().user_data = id.to_bits() as u128;
        drop(physics);

        app.world
            .entity_mut(id)
            .insert(ColliderComponent(co_handle));
        Ok(id.to_bits())
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
#[pyo3(signature = (x, y, z, sx, sy, sz, r=None, g=None, b=None))]
fn spawn_static_box(
    x: f32,
    y: f32,
    z: f32,
    sx: f32,
    sy: f32,
    sz: f32,
    r: Option<f32>,
    g: Option<f32>,
    b: Option<f32>,
) -> PyResult<u64> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let mut physics = app.world.resource_mut::<PhysicsState>();
        let rigid_body = rapier::RigidBodyBuilder::fixed()
            .translation(vec3_to_vector(Vec3::new(x, y, z)))
            .build();
        let collider = rapier::ColliderBuilder::cuboid(sx, sy, sz).build();
        let rb_handle = physics.rigid_body_set.insert(rigid_body);
        drop(physics);

        let id = app
            .world
            .spawn((
                Transform::new(x, y, z),
                Rotation::new(0.0, 0.0, 0.0),
                PreviousTransform { x, y, z },
                PreviousRotation {
                    x: 0.0,
                    y: 0.0,
                    z: 0.0,
                },
                Scale::new(sx * 2.0, sy * 2.0, sz * 2.0),
                Color::new(r.unwrap_or(1.0), g.unwrap_or(1.0), b.unwrap_or(1.0), 1.0),
                MeshName("cube".to_string()),
                RigidBodyComponent(rb_handle),
            ))
            .id();

        let mut physics = app.world.resource_mut::<PhysicsState>();
        let PhysicsState {
            ref mut collider_set,
            ref mut rigid_body_set,
            ..
        } = *physics;
        let co_handle = collider_set.insert_with_parent(collider, rb_handle, rigid_body_set);
        collider_set.get_mut(co_handle).unwrap().user_data = id.to_bits() as u128;
        drop(physics);

        app.world
            .entity_mut(id)
            .insert(ColliderComponent(co_handle));
        Ok(id.to_bits())
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
#[pyo3(signature = (y, r=None, g=None, b=None))]
fn spawn_floor(y: f32, r: Option<f32>, g: Option<f32>, b: Option<f32>) -> PyResult<u64> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let mut physics = app.world.resource_mut::<PhysicsState>();
        let rigid_body = rapier::RigidBodyBuilder::fixed()
            .translation(vec3_to_vector(Vec3::new(0.0, y, 0.0)))
            .build();
        let collider = rapier::ColliderBuilder::cuboid(10.0, 0.1, 10.0).build();
        let rb_handle = physics.rigid_body_set.insert(rigid_body);
        drop(physics);

        let id = app
            .world
            .spawn((
                Transform::new(0.0, y, 0.0),
                Rotation::new(0.0, 0.0, 0.0),
                PreviousTransform { x: 0.0, y, z: 0.0 },
                PreviousRotation {
                    x: 0.0,
                    y: 0.0,
                    z: 0.0,
                },
                Scale::new(20.0, 0.2, 20.0),
                Color::new(r.unwrap_or(1.0), g.unwrap_or(1.0), b.unwrap_or(1.0), 1.0),
                MeshName("cube".to_string()),
                RigidBodyComponent(rb_handle),
            ))
            .id();

        let mut physics = app.world.resource_mut::<PhysicsState>();
        let PhysicsState {
            ref mut collider_set,
            ref mut rigid_body_set,
            ..
        } = *physics;
        let co_handle = collider_set.insert_with_parent(collider, rb_handle, rigid_body_set);
        collider_set.get_mut(co_handle).unwrap().user_data = id.to_bits() as u128;
        drop(physics);

        app.world
            .entity_mut(id)
            .insert(ColliderComponent(co_handle));
        Ok(id.to_bits())
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
#[pyo3(signature = (x, y, z, seed=0))]
fn spawn_procedural_ship(x: f32, y: f32, z: f32, seed: u32) -> PyResult<u64> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let mut builder = procedural::mesh_builder::MeshBuilder::new();
        procedural::spaceship::ProceduralSpaceship::generate(seed, &mut builder);
        let vertices: Vec<Vertex> = builder
            .vertices
            .iter()
            .map(|v| Vertex {
                position: v.position,
                normal: v.normal,
                color: [v.color[0], v.color[1], v.color[2]],
                tex_coords: v.tex_coords,
                joints: [0; 4],
                weights: [0.0; 4],
            })
            .collect();
        let indices: Vec<u32> = builder.indices;
        if let Some(render_state) = &mut app.render_state {
            render_state.upload_mesh("procedural_ship", &vertices, &indices);
        }
        let id = app
            .world
            .spawn((
                Transform::new(x, y, z),
                Rotation::new(0.0, 0.0, 0.0),
                PreviousTransform { x, y, z },
                PreviousRotation {
                    x: 0.0,
                    y: 0.0,
                    z: 0.0,
                },
                Scale::new(1.0, 1.0, 1.0),
                Color::new(1.0, 1.0, 1.0, 1.0),
            ))
            .id();
        Ok(id.to_bits())
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
#[pyo3(signature = (x, y, z, speed=5.0, max_slope=0.78, offset=0.1))]
fn spawn_character(
    x: f32,
    y: f32,
    z: f32,
    speed: f32,
    max_slope: f32,
    offset: f32,
) -> PyResult<u64> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let mut physics = app.world.resource_mut::<PhysicsState>();
        let rigid_body = rapier::RigidBodyBuilder::kinematic_position_based()
            .translation(vec3_to_vector(Vec3::new(x, y, z)))
            .build();
        let collider = rapier::ColliderBuilder::capsule_y(0.5, 0.4)
            .friction(0.0)
            .restitution(0.0)
            .build();
        let rb_handle = physics.rigid_body_set.insert(rigid_body);
        drop(physics);

        let id = app
            .world
            .spawn((
                Transform::new(x, y, z),
                Rotation::new(0.0, 0.0, 0.0),
                PreviousTransform { x, y, z },
                PreviousRotation {
                    x: 0.0,
                    y: 0.0,
                    z: 0.0,
                },
                Scale::new(0.8, 1.8, 0.8),
                CharacterController::new(speed, max_slope, offset),
                RigidBodyComponent(rb_handle),
                Color::new(0.0, 1.0, 0.0, 1.0),
                MeshName("cube".to_string()),
                Material {
                    diffuse: "default".to_string(),
                    normal: "flat_normal".to_string(),
                    metallic: 0.0,
                    roughness: 0.5,
                },
            ))
            .id();

        let mut physics = app.world.resource_mut::<PhysicsState>();
        let PhysicsState {
            ref mut collider_set,
            ref mut rigid_body_set,
            ..
        } = *physics;
        let co_handle = collider_set.insert_with_parent(collider, rb_handle, rigid_body_set);
        collider_set.get_mut(co_handle).unwrap().user_data = id.to_bits() as u128;
        drop(physics);

        app.world
            .entity_mut(id)
            .insert(ColliderComponent(co_handle));
        Ok(id.to_bits())
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn move_character(entity_id: u64, dx: f32, dy: f32, dz: f32) -> PyResult<(f32, f32, f32)> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let entity = Entity::from_bits(entity_id);
        let (max_slope, offset, current_pos_vec) = {
            if let Ok((cc, t)) = app
                .world
                .query::<(&CharacterController, &Transform)>()
                .get(&app.world, entity)
            {
                (cc.max_slope_angle, cc.offset, Vec3::new(t.x, t.y, t.z))
            } else {
                return Err(PyValueError::new_err(
                    "Entity missing CharacterController or Transform",
                ));
            }
        };
        let collider_handle = if let Some(c) = app.world.get::<ColliderComponent>(entity) {
            c.0
        } else {
            return Err(PyValueError::new_err("Entity missing Collider"));
        };

        let physics = app.world.resource_mut::<PhysicsState>();
        let PhysicsState {
            ref query_pipeline,
            ref collider_set,
            ref rigid_body_set,
            ..
        } = *physics;

        let mut controller = KinematicCharacterController::default();
        controller.max_slope_climb_angle = max_slope as rapier::Real;
        controller.offset = CharacterLength::Absolute(offset as rapier::Real);

        let desired_translation = rapier::Vector::new(dx, dy, dz);
        let collider = collider_set.get(collider_handle).unwrap();
        let shape = collider.shape();
        let character_pos =
            rapier::Isometry::translation(current_pos_vec.x, current_pos_vec.y, current_pos_vec.z);

        let effective_movement = controller.move_shape(
            0.02, // FIXED_DT
            rigid_body_set,
            collider_set,
            query_pipeline,
            shape,
            &character_pos,
            desired_translation,
            rapier::QueryFilter::default().exclude_collider(collider_handle),
            |_| {},
        );

        let translation_delta = effective_movement.translation;
        let new_pos = current_pos_vec
            + Vec3::new(
                translation_delta.x,
                translation_delta.y,
                translation_delta.z,
            );
        drop(physics);

        let mut t = app.world.get_mut::<Transform>(entity).unwrap();
        t.x = new_pos.x;
        t.y = new_pos.y;
        t.z = new_pos.z;

        if let Some(rb_handle) = app.world.get::<RigidBodyComponent>(entity).map(|rb| rb.0) {
            let mut physics = app.world.resource_mut::<PhysicsState>();
            if let Some(rb) = physics.rigid_body_set.get_mut(rb_handle) {
                rb.set_translation(vec3_to_vector(new_pos), true);
            }
        }
        Ok((new_pos.x, new_pos.y, new_pos.z))
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

// Physics Controls
#[pyfunction]
fn apply_impulse(entity_id: u64, ix: f32, iy: f32, iz: f32) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let entity = Entity::from_bits(entity_id);
        let rb_handle = app
            .world
            .get::<RigidBodyComponent>(entity)
            .ok_or_else(|| PyValueError::new_err("Entity has no RigidBodyComponent"))?
            .0;
        let mut physics = app.world.resource_mut::<PhysicsState>();
        if let Some(rb) = physics.rigid_body_set.get_mut(rb_handle) {
            rb.apply_impulse(
                rapier::Vector::new(ix as rapier::Real, iy as rapier::Real, iz as rapier::Real),
                true,
            );
        }
        Ok(())
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn apply_force(entity_id: u64, fx: f32, fy: f32, fz: f32) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let entity = Entity::from_bits(entity_id);
        let rb_handle = app
            .world
            .get::<RigidBodyComponent>(entity)
            .ok_or_else(|| PyValueError::new_err("Entity has no RigidBodyComponent"))?
            .0;
        let mut physics = app.world.resource_mut::<PhysicsState>();
        if let Some(rb) = physics.rigid_body_set.get_mut(rb_handle) {
            rb.add_force(
                rapier::Vector::new(fx as rapier::Real, fy as rapier::Real, fz as rapier::Real),
                true,
            );
        }
        Ok(())
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn get_velocity(entity_id: u64) -> PyResult<(f32, f32, f32)> {
    let guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_ref() {
        let entity = Entity::from_bits(entity_id);
        let rb_handle = app
            .world
            .get::<RigidBodyComponent>(entity)
            .ok_or_else(|| PyValueError::new_err("Entity has no RigidBodyComponent"))?
            .0;
        let physics = app.world.resource::<PhysicsState>();
        if let Some(rb) = physics.rigid_body_set.get(rb_handle) {
            let vel = rb.linvel();
            Ok((vel.x, vel.y, vel.z))
        } else {
            Err(PyValueError::new_err("RigidBody not found"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn set_velocity(entity_id: u64, vx: f32, vy: f32, vz: f32) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let entity = Entity::from_bits(entity_id);
        let rb_handle = app
            .world
            .get::<RigidBodyComponent>(entity)
            .ok_or_else(|| PyValueError::new_err("Entity has no RigidBodyComponent"))?
            .0;
        let mut physics = app.world.resource_mut::<PhysicsState>();
        if let Some(rb) = physics.rigid_body_set.get_mut(rb_handle) {
            rb.set_linvel(
                rapier::Vector::new(vx as rapier::Real, vy as rapier::Real, vz as rapier::Real),
                true,
            );
        }
        Ok(())
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn get_rotation(entity_id: u64) -> PyResult<Rotation> {
    let guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_ref() {
        let entity = Entity::from_bits(entity_id);
        if let Some(r) = app.world.get::<Rotation>(entity) {
            Ok(r.clone())
        } else {
            Err(PyValueError::new_err("No Rotation"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
#[pyo3(signature = (ox, oy, oz, dx, dy, dz, max_toi, solid, ignore_entity_id=None))]
fn raycast(
    ox: f32,
    oy: f32,
    oz: f32,
    dx: f32,
    dy: f32,
    dz: f32,
    max_toi: f32,
    solid: bool,
    ignore_entity_id: Option<u64>,
) -> PyResult<Option<(u64, f32, f32, f32, f32, f32, f32, f32)>> {
    let guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_ref() {
        let physics = app.world.resource::<PhysicsState>();
        let ray = rapier::Ray::new(
            rapier::Point::new(ox, oy, oz),
            rapier::Vector::new(dx, dy, dz),
        );

        let mut filter = rapier::QueryFilter::default();
        if let Some(ignore_id) = ignore_entity_id {
            if let Some(rb_comp) = app
                .world
                .get::<RigidBodyComponent>(Entity::from_bits(ignore_id))
            {
                filter = filter.exclude_rigid_body(rb_comp.0);
            }
            if let Some(col_comp) = app
                .world
                .get::<ColliderComponent>(Entity::from_bits(ignore_id))
            {
                filter = filter.exclude_collider(col_comp.0);
            }
        }

        if let Some((handle, intersection)) = physics.query_pipeline.cast_ray_and_get_normal(
            &physics.rigid_body_set,
            &physics.collider_set,
            &ray,
            max_toi,
            solid,
            filter,
        ) {
            if let Some(collider) = physics.collider_set.get(handle) {
                let entity_id = collider.user_data as u64;
                let hit_point = ray.point_at(intersection.time_of_impact);
                return Ok(Some((
                    entity_id,
                    intersection.time_of_impact,
                    hit_point.x,
                    hit_point.y,
                    hit_point.z,
                    intersection.normal.x,
                    intersection.normal.y,
                    intersection.normal.z,
                )));
            }
        }
        Ok(None)
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
#[pyo3(signature = (ox, oy, oz, dx, dy, dz, max_toi, solid))]
fn raycast_all(
    ox: f32,
    oy: f32,
    oz: f32,
    dx: f32,
    dy: f32,
    dz: f32,
    max_toi: f32,
    solid: bool,
) -> PyResult<Vec<(u64, f32, f32, f32, f32, f32, f32, f32)>> {
    let guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_ref() {
        let physics = app.world.resource::<PhysicsState>();
        let ray = rapier::Ray::new(
            rapier::Point::new(ox, oy, oz),
            rapier::Vector::new(dx, dy, dz),
        );
        let filter = rapier::QueryFilter::default();
        let mut results = Vec::new();

        physics.query_pipeline.intersections_with_ray(
            &physics.rigid_body_set,
            &physics.collider_set,
            &ray,
            max_toi,
            solid,
            filter,
            |handle, intersection| {
                if let Some(collider) = physics.collider_set.get(handle) {
                    let entity_id = collider.user_data as u64;
                    let hit_point = ray.point_at(intersection.time_of_impact);
                    results.push((
                        entity_id,
                        intersection.time_of_impact,
                        hit_point.x,
                        hit_point.y,
                        hit_point.z,
                        intersection.normal.x,
                        intersection.normal.y,
                        intersection.normal.z,
                    ));
                }
                true // Continue to find all intersections
            },
        );

        Ok(results)
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

/// Returns detailed collision events: Vec<(entity_a, entity_b, contact_x, contact_y, contact_z, normal_x, normal_y, normal_z, depth)>
#[pyfunction]
fn get_collision_events_detailed() -> PyResult<Vec<(u64, u64, f32, f32, f32, f32, f32, f32, f32)>> {
    let guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_ref() {
        let physics = app.world.resource::<PhysicsState>();
        let mut results = Vec::new();

        physics.narrow_phase.contact_pairs().for_each(|pair| {
            let entity_a = physics
                .collider_set
                .get(pair.collider1)
                .map(|c| c.user_data as u64)
                .unwrap_or(0);
            let entity_b = physics
                .collider_set
                .get(pair.collider2)
                .map(|c| c.user_data as u64)
                .unwrap_or(0);

            for manifold in pair.manifolds.iter() {
                let normal = manifold.data.normal;
                for point in manifold.points.iter() {
                    let local_p = point.local_p1;
                    results.push((
                        entity_a, entity_b, local_p.x, local_p.y, local_p.z, normal.x, normal.y,
                        normal.z, point.dist,
                    ));
                }
            }
        });

        Ok(results)
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn set_gravity(x: f32, y: f32, z: f32) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let mut physics = app.world.resource_mut::<PhysicsState>();
        physics.gravity = rapier::Vector::new(x, y, z);
        Ok(())
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

// --- ECS Dynamic Components Adders ---
#[pyfunction]
fn add_component_health(entity_id: u64, current: f32, max: f32) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let e = Entity::from_bits(entity_id);
        if let Some(mut cmds) = app.world.get_entity_mut(e) {
            cmds.insert(Health::new(current, max));
            Ok(())
        } else {
            Err(PyRuntimeError::new_err("Entity not found"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn add_component_mana(entity_id: u64, current: f32, max: f32) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let e = Entity::from_bits(entity_id);
        if let Some(mut cmds) = app.world.get_entity_mut(e) {
            cmds.insert(Mana::new(current, max));
            Ok(())
        } else {
            Err(PyRuntimeError::new_err("Entity not found"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

// Input
#[pyfunction]
fn bind_action(name: String, key: String) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        if let Some(mut input) = app.world.get_resource_mut::<InputState>() {
            let entry = input
                .action_bindings
                .entry(Arc::from(name))
                .or_insert(Vec::new());
            let k = key.to_lowercase();
            if !entry.contains(&k) {
                entry.push(k);
            }
        }
        Ok(())
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn is_action_down(name: String) -> PyResult<bool> {
    let guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_ref() {
        if let Some(input) = app.world.get_resource::<InputState>() {
            return Ok(input.active_actions.contains(name.as_str()));
        }
    }
    Ok(false)
}

#[pyfunction]
fn bind_axis(
    name: String,
    source_type: String,
    source_code: String,
    scale: f32,
    deadzone: f32,
) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        if let Some(mut input) = app.world.get_resource_mut::<InputState>() {
            let entry = input
                .axis_bindings
                .entry(Arc::from(name))
                .or_insert(Vec::new());
            let source = match source_type.as_str() {
                "Key" => InputSource::Key(source_code.to_lowercase()),
                "GamepadButton" => {
                    let btn = match source_code.as_str() {
                        "South" => Button::South,
                        "East" => Button::East,
                        "North" => Button::North,
                        "West" => Button::West,
                        "C" => Button::C,
                        "Z" => Button::Z,
                        "LeftTrigger" => Button::LeftTrigger,
                        "LeftTrigger2" => Button::LeftTrigger2,
                        "RightTrigger" => Button::RightTrigger,
                        "RightTrigger2" => Button::RightTrigger2,
                        "Select" => Button::Select,
                        "Start" => Button::Start,
                        "Mode" => Button::Mode,
                        "LeftThumb" => Button::LeftThumb,
                        "RightThumb" => Button::RightThumb,
                        "DPadUp" => Button::DPadUp,
                        "DPadDown" => Button::DPadDown,
                        "DPadLeft" => Button::DPadLeft,
                        "DPadRight" => Button::DPadRight,
                        _ => {
                            return Err(PyValueError::new_err(format!(
                                "Unknown button: {}",
                                source_code
                            )))
                        }
                    };
                    InputSource::GamepadButton(0, btn)
                }
                "GamepadAxis" => {
                    let axis = match source_code.as_str() {
                        "LeftStickX" => Axis::LeftStickX,
                        "LeftStickY" => Axis::LeftStickY,
                        "RightStickX" => Axis::RightStickX,
                        "RightStickY" => Axis::RightStickY,
                        "LeftZ" => Axis::LeftZ,
                        "RightZ" => Axis::RightZ,
                        "DPadX" => Axis::DPadX,
                        "DPadY" => Axis::DPadY,
                        _ => {
                            return Err(PyValueError::new_err(format!(
                                "Unknown axis: {}",
                                source_code
                            )))
                        }
                    };
                    InputSource::GamepadAxis(0, axis)
                }
                _ => return Err(PyValueError::new_err("Unknown source type")),
            };
            entry.push(AxisBinding {
                source,
                scale,
                deadzone,
            });
        }
        Ok(())
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn get_axis(name: String) -> PyResult<f32> {
    let guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_ref() {
        if let Some(input) = app.world.get_resource::<InputState>() {
            return Ok(*input.axis_values.get(name.as_str()).unwrap_or(&0.0));
        }
    }
    Ok(0.0)
}

#[pyfunction]
fn is_gamepad_connected(id: usize) -> PyResult<bool> {
    let guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_ref() {
        if let Some(input) = app.world.get_resource::<InputState>() {
            return Ok(input.connected_gamepads.contains(&id));
        }
    }
    Ok(false)
}

#[pyfunction]
fn get_collision_events() -> PyResult<Vec<(u64, u64, bool)>> {
    let guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_ref() {
        if let Some(events) = app.world.get_resource::<CollisionEvents>() {
            let list = events
                .events
                .iter()
                .map(|e| (e.entity1, e.entity2, e.started))
                .collect();
            return Ok(list);
        }
    }
    Ok(Vec::new())
}

#[pyfunction]
fn mount_vfs(virtual_path: String, disk_path: String) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        match app
            .vfs
            .mount(&virtual_path, std::path::PathBuf::from(disk_path))
        {
            Ok(_) => Ok(()),
            Err(e) => Err(PyIOError::new_err(e.to_string())),
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

// Deprecated or simple passthroughs

#[pyfunction]
fn vfs_list_files(prefix: String) -> PyResult<Vec<String>> {
    let guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_ref() {
        Ok(app.vfs.list_files(&prefix))
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn vfs_file_exists(path: String) -> PyResult<bool> {
    let guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_ref() {
        Ok(app.vfs.file_exists(&path))
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn key_down(key: String) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        if let Some(mut i) = app.world.get_resource_mut::<InputState>() {
            let k = key.to_lowercase();
            if !i.pressed_keys.contains(&k) {
                i.just_pressed.insert(k.clone());
            }
            i.pressed_keys.insert(k);
        }
        Ok(())
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}
#[pyfunction]
fn key_up(key: String) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        if let Some(mut i) = app.world.get_resource_mut::<InputState>() {
            let k = key.to_lowercase();
            i.pressed_keys.remove(&k);
            i.just_released.insert(k);
        }
        Ok(())
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}
#[pyfunction]
fn is_key_down(key: String) -> PyResult<bool> {
    let guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_ref() {
        if let Some(input) = app.world.get_resource::<InputState>() {
            return Ok(input.pressed_keys.contains(&key.to_lowercase()));
        }
    }
    Ok(false)
}
#[pyfunction]
fn is_key_just_pressed(key: String) -> PyResult<bool> {
    let guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_ref() {
        if let Some(input) = app.world.get_resource::<InputState>() {
            return Ok(input.just_pressed.contains(&key.to_lowercase()));
        }
    }
    Ok(false)
}
#[pyfunction]
fn get_mouse_delta() -> PyResult<(f32, f32)> {
    let guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_ref() {
        if let Some(input) = app.world.get_resource::<InputState>() {
            return Ok(input.mouse_delta);
        }
    }
    Ok((0.0, 0.0))
}

// Rendering Properties (Stubbed or Simple)
#[pyfunction]
fn set_material(metallic: f32, roughness: f32) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        if let Some(rs) = app.render_state.as_mut() {
            rs.set_material(metallic, roughness);
            Ok(())
        } else {
            Err(PyRuntimeError::new_err("Render state not active"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}
#[pyfunction]
fn capture_screenshot(filename: String) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        if let Some(rs) = app.render_state.as_mut() {
            rs.request_screenshot(filename);
            Ok(())
        } else {
            Err(PyRuntimeError::new_err("Render state not active"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}
#[pyfunction]
fn set_point_light(
    index: usize,
    x: f32,
    y: f32,
    z: f32,
    r: f32,
    g: f32,
    b: f32,
    i: f32,
) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        if let Some(rs) = app.render_state.as_mut() {
            rs.set_point_light(index, [x, y, z], [r, g, b], i);
            Ok(())
        } else {
            Err(PyRuntimeError::new_err("Render state not active"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}
#[pyfunction]
fn set_skybox_color(r: f32, g: f32, b: f32) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        if let Some(rs) = app.render_state.as_mut() {
            rs.set_skybox_color(r, g, b);
            Ok(())
        } else {
            Err(PyRuntimeError::new_err("Render state not active"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}
#[pyfunction]
fn set_fog(density: f32, r: f32, g: f32, b: f32) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        if let Some(rs) = app.render_state.as_mut() {
            rs.set_fog(density, r, g, b);
            Ok(())
        } else {
            Err(PyRuntimeError::new_err("Render state not active"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn get_vram_usage() -> PyResult<usize> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        if let Some(rs) = app.render_state.as_mut() {
            Ok(rs.get_vram_usage())
        } else {
            Ok(0)
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn recompile_shader(source: String) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        if let Some(rs) = app.render_state.as_mut() {
            match rs.recompile_shader(&source) {
                Ok(_) => Ok(()),
                Err(e) => Err(PyRuntimeError::new_err(format!(
                    "Failed to recompile shader: {}",
                    e
                ))),
            }
        } else {
            Err(PyRuntimeError::new_err("Render state not initialized"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn set_sun_direction(x: f32, y: f32, z: f32) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        if let Some(rs) = app.render_state.as_mut() {
            rs.set_sun_direction(x, y, z);
            Ok(())
        } else {
            Err(PyRuntimeError::new_err("Render state not active"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn set_sun_color(r: f32, g: f32, b: f32, intensity: f32) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        if let Some(rs) = app.render_state.as_mut() {
            rs.set_sun_color(r, g, b, intensity);
            Ok(())
        } else {
            Err(PyRuntimeError::new_err("Render state not active"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn set_ambient_intensity(intensity: f32) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        if let Some(rs) = app.render_state.as_mut() {
            rs.set_ambient_intensity(intensity);
            Ok(())
        } else {
            Err(PyRuntimeError::new_err("Render state not active"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn set_wind_strength(strength: f32) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        if let Some(rs) = app.render_state.as_mut() {
            rs.set_wind_strength(strength);
            Ok(())
        } else {
            Err(PyRuntimeError::new_err("Renderer not initialized"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn rotate_camera(dx: f32, dy: f32) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        let mut q = app.world.query::<(&Camera, &mut Rotation)>();
        if let Some((_, mut r)) = q.iter_mut(&mut app.world).next() {
            r.y += dx;
            r.x += dy;
        }
        Ok(())
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn update_game() -> PyResult<()> {
    // Variable update
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        app.schedule.run(&mut app.world);
        Ok(())
    } else {
        Err(PyRuntimeError::new_err("Engine"))
    }
}

#[pyfunction]
fn step_fixed() -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        app.fixed_schedule.run(&mut app.world);
        Ok(())
    } else {
        Err(PyRuntimeError::new_err("Engine"))
    }
}
#[pyfunction]
fn render_frame() -> PyResult<()> {
    // Deprecated manual render
    Ok(())
}

#[pyfunction]
fn render_viewport_f32() -> PyResult<Vec<f32>> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        if let Some(rs) = app.render_state.as_mut() {
            Ok(rs.render_to_f32())
        } else {
            Err(PyRuntimeError::new_err("Render state not active"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

// --- Network Bindings ---

#[pyfunction]
fn start_server(port: u16) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        if let Some(net) = app.network_state.as_mut() {
            net.start_server(port).map_err(PyRuntimeError::new_err)
        } else {
            Err(PyRuntimeError::new_err("Network not initialized"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn connect_client(ip: String, port: u16) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        if let Some(net) = app.network_state.as_mut() {
            net.connect_client(&ip, port)
                .map_err(PyRuntimeError::new_err)
        } else {
            Err(PyRuntimeError::new_err("Network not initialized"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
#[pyo3(signature = (data, target=None))]
fn send_message(data: Vec<u8>, target: Option<String>) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        if let Some(net) = app.network_state.as_mut() {
            net.send_message(&data, target.as_deref())
                .map_err(PyRuntimeError::new_err)
        } else {
            Err(PyRuntimeError::new_err("Network not initialized"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
fn get_messages() -> PyResult<Vec<(String, Vec<u8>)>> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        if let Some(net) = app.network_state.as_mut() {
            net.update(); // Poll socket
            let mut msgs = Vec::new();
            while let Some(msg) = net.pop_message() {
                msgs.push((msg.from, msg.data));
            }
            Ok(msgs)
        } else {
            Err(PyRuntimeError::new_err("Network not initialized"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pymodule]
fn backend(m: &Bound<'_, PyModule>) -> PyResult<()> {
    m.add_class::<Health>()?;
    m.add_class::<Mana>()?;
    m.add_class::<Transform>()?;
    m.add_class::<Rotation>()?;
    m.add_class::<Scale>()?;
    m.add_class::<Color>()?;
    m.add_class::<Parent>()?;
    m.add_class::<Camera>()?;
    m.add_class::<MeshName>()?;
    m.add_class::<Material>()?;
    m.add_class::<CharacterController>()?;
    m.add_class::<particles::ParticleSystem>()?;
    m.add_class::<ParticleEmitter>()?;

    // Physics Extras
    m.add_class::<physics::RaycastHit>()?;

    m.add_function(wrap_pyfunction!(run_engine, m)?)?;
    m.add_function(wrap_pyfunction!(init_headless, m)?)?;
    m.add_function(wrap_pyfunction!(spawn_entity, m)?)?;
    m.add_function(wrap_pyfunction!(despawn_entity, m)?)?;
    m.add_function(wrap_pyfunction!(add_particle_emitter, m)?)?;
    m.add_function(wrap_pyfunction!(spawn_dynamic_cube, m)?)?;
    m.add_function(wrap_pyfunction!(spawn_static_box, m)?)?;
    m.add_function(wrap_pyfunction!(spawn_floor, m)?)?;
    m.add_function(wrap_pyfunction!(spawn_procedural_ship, m)?)?;
    m.add_function(wrap_pyfunction!(spawn_character, m)?)?;
    m.add_function(wrap_pyfunction!(load_mesh, m)?)?;
    m.add_function(wrap_pyfunction!(load_texture, m)?)?;
    m.add_function(wrap_pyfunction!(load_skybox, m)?)?;
    m.add_function(wrap_pyfunction!(load_scene, m)?)?;
    m.add_function(wrap_pyfunction!(spawn_scene, m)?)?;
    m.add_function(wrap_pyfunction!(play_sound, m)?)?;
    m.add_function(wrap_pyfunction!(set_sound_volume, m)?)?;
    m.add_function(wrap_pyfunction!(set_sound_panning, m)?)?;
    m.add_function(wrap_pyfunction!(stop_sound, m)?)?;
    m.add_function(wrap_pyfunction!(get_vram_usage, m)?)?;
    m.add_function(wrap_pyfunction!(recompile_shader, m)?)?;
    m.add_function(wrap_pyfunction!(get_transform, m)?)?;
    m.add_function(wrap_pyfunction!(set_transform, m)?)?;
    m.add_function(wrap_pyfunction!(set_rotation, m)?)?;
    m.add_function(wrap_pyfunction!(set_scale, m)?)?;
    m.add_function(wrap_pyfunction!(set_color, m)?)?;
    m.add_function(wrap_pyfunction!(set_point_light, m)?)?;
    m.add_function(wrap_pyfunction!(set_mesh, m)?)?;
    m.add_function(wrap_pyfunction!(set_material_textures, m)?)?;
    m.add_function(wrap_pyfunction!(set_material_params, m)?)?;
    m.add_function(wrap_pyfunction!(set_parent, m)?)?;
    m.add_function(wrap_pyfunction!(get_all_entities, m)?)?;
    m.add_function(wrap_pyfunction!(add_gizmo_line, m)?)?;
    m.add_function(wrap_pyfunction!(update_game, m)?)?;
    m.add_function(wrap_pyfunction!(step_fixed, m)?)?;
    m.add_function(wrap_pyfunction!(render_viewport_f32, m)?)?;
    m.add_function(wrap_pyfunction!(render_frame, m)?)?;
    m.add_function(wrap_pyfunction!(set_material, m)?)?;
    m.add_function(wrap_pyfunction!(set_point_light, m)?)?;
    m.add_function(wrap_pyfunction!(set_skybox_color, m)?)?;
    m.add_function(wrap_pyfunction!(set_fog, m)?)?;
    m.add_function(wrap_pyfunction!(set_sun_direction, m)?)?;
    m.add_function(wrap_pyfunction!(set_sun_color, m)?)?;
    m.add_function(wrap_pyfunction!(set_ambient_intensity, m)?)?;
    m.add_function(wrap_pyfunction!(set_wind_strength, m)?)?;
    m.add_function(wrap_pyfunction!(rotate_camera, m)?)?;
    m.add_function(wrap_pyfunction!(key_down, m)?)?;
    m.add_function(wrap_pyfunction!(key_up, m)?)?;
    m.add_function(wrap_pyfunction!(is_key_down, m)?)?;
    m.add_function(wrap_pyfunction!(is_key_just_pressed, m)?)?;
    m.add_function(wrap_pyfunction!(get_mouse_delta, m)?)?;
    m.add_function(wrap_pyfunction!(set_gravity, m)?)?;
    m.add_function(wrap_pyfunction!(get_main_camera_id, m)?)?;
    m.add_function(wrap_pyfunction!(set_cursor_grab, m)?)?;
    m.add_function(wrap_pyfunction!(set_cursor_visible, m)?)?;
    m.add_function(wrap_pyfunction!(capture_screenshot, m)?)?;
    m.add_function(wrap_pyfunction!(bind_action, m)?)?;
    m.add_function(wrap_pyfunction!(is_action_down, m)?)?;
    m.add_function(wrap_pyfunction!(bind_axis, m)?)?;
    m.add_function(wrap_pyfunction!(get_axis, m)?)?;
    m.add_function(wrap_pyfunction!(is_gamepad_connected, m)?)?;
    m.add_function(wrap_pyfunction!(get_collision_events, m)?)?;
    m.add_function(wrap_pyfunction!(mount_vfs, m)?)?;
    m.add_function(wrap_pyfunction!(vfs_list_files, m)?)?;
    m.add_function(wrap_pyfunction!(vfs_file_exists, m)?)?;
    m.add_function(wrap_pyfunction!(move_character, m)?)?;
    m.add_function(wrap_pyfunction!(apply_impulse, m)?)?;
    m.add_function(wrap_pyfunction!(apply_force, m)?)?;
    m.add_function(wrap_pyfunction!(get_velocity, m)?)?;
    m.add_function(wrap_pyfunction!(set_velocity, m)?)?;
    m.add_function(wrap_pyfunction!(get_rotation, m)?)?;
    m.add_function(wrap_pyfunction!(is_asset_loaded, m)?)?;
    m.add_function(wrap_pyfunction!(raycast, m)?)?;
    m.add_function(wrap_pyfunction!(set_post_process_params, m)?)?;
    m.add_function(wrap_pyfunction!(start_server, m)?)?;
    m.add_function(wrap_pyfunction!(connect_client, m)?)?;
    m.add_function(wrap_pyfunction!(send_message, m)?)?;
    m.add_function(wrap_pyfunction!(get_messages, m)?)?;
    m.add_function(wrap_pyfunction!(generate_heightmap, m)?)?;
    m.add_function(wrap_pyfunction!(raycast_all, m)?)?;
    m.add_function(wrap_pyfunction!(get_collision_events_detailed, m)?)?;
    m.add_function(wrap_pyfunction!(add_component_health, m)?)?;
    m.add_function(wrap_pyfunction!(add_component_mana, m)?)?;
    Ok(())
}

#[pyfunction]
fn add_gizmo_line(
    start_x: f32,
    start_y: f32,
    start_z: f32,
    end_x: f32,
    end_y: f32,
    end_z: f32,
    r: f32,
    g: f32,
    b: f32,
) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        if let Some(render_state) = &mut app.render_state {
            render_state.add_gizmo_line(
                [start_x, start_y, start_z],
                [end_x, end_y, end_z],
                [r, g, b],
            );
            Ok(())
        } else {
            Err(PyRuntimeError::new_err("Render state not initialized"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}

#[pyfunction]
#[pyo3(signature = (width, height, seed=0, scale=50.0, octaves=4, persistence=0.5, lacunarity=2.0))]
fn generate_heightmap(
    width: usize,
    height: usize,
    seed: u32,
    scale: f64,
    octaves: usize,
    persistence: f64,
    lacunarity: f64,
) -> PyResult<Vec<f32>> {
    Ok(procedural::terrain::generate_heightmap(
        width,
        height,
        seed,
        scale,
        octaves,
        persistence,
        lacunarity,
    ))
}

#[pyfunction]
fn set_post_process_params(
    exposure: f32,
    gamma: f32,
    bloom_intensity: f32,
    bloom_threshold: f32,
    chromatic_aberration: f32,
    time: f32,
) -> PyResult<()> {
    let mut guard = GAME_APP.lock().unwrap();
    if let Some(app) = guard.as_mut() {
        if let Some(render_state) = &mut app.render_state {
            render_state.set_post_process_params(
                exposure,
                gamma,
                bloom_intensity,
                bloom_threshold,
                chromatic_aberration,
                time,
            );
            Ok(())
        } else {
            Err(PyRuntimeError::new_err("Render state not initialized"))
        }
    } else {
        Err(PyRuntimeError::new_err("Engine not initialized"))
    }
}
