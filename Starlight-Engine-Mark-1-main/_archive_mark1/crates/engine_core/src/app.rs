use bevy_ecs::prelude::*;
use engine_audio::AudioState;
use engine_render::RenderState;
use gilrs::{Event as GilrsEvent, Gilrs};
use glam::{Mat4, Quat, Vec3};
use pyo3::prelude::*;
use std::sync::Mutex;
use std::time::Instant;
use tokio::sync::mpsc::UnboundedReceiver;
use winit::application::ApplicationHandler;
use winit::event::WindowEvent;
use winit::event_loop::{ActiveEventLoop, ControlFlow};
use winit::window::{Window, WindowId};

use crate::assets::{AssetRegistry, SceneRegistry};
use crate::ecs::components::*;
use crate::ecs::systems::*;
use crate::input::{action_update_system, input_reset_system, InputState};
use crate::loader::{AssetData, AsyncAssetLoader};
use crate::network::NetworkState;
use crate::physics::{collision_event_system, physics_step_system, CollisionEvents, PhysicsState};
use crate::vfs::Vfs;

const FIXED_TIMESTEP: f64 = 0.02; // 50 Hz
const MAX_FRAME_TIME: f64 = 0.25;

pub struct GameApp {
    pub world: World,
    pub schedule: Schedule,
    pub fixed_schedule: Schedule,
    pub render_state: Option<RenderState>,
    pub audio_state: Option<AudioState>,
    pub asset_registry: AssetRegistry,
    pub asset_loader_rx: Option<UnboundedReceiver<AssetData>>,
    pub asset_loader_tx: Option<tokio::sync::mpsc::UnboundedSender<crate::loader::LoadRequest>>,
    pub vfs: Vfs,
    pub network_state: Option<NetworkState>,
}

unsafe impl Send for GameApp {}

// Global instance for PyO3 bindings
pub static GAME_APP: Mutex<Option<GameApp>> = Mutex::new(None);
pub static MAIN_CAMERA_ID: Mutex<Option<u64>> = Mutex::new(None);
pub static HEADLESS_RUNTIME: Mutex<Option<tokio::runtime::Runtime>> = Mutex::new(None);

pub struct StarlightRunner {
    pub title: String,
    pub width: u32,
    pub height: u32,
    pub window: Option<Window>,
    pub on_update: PyObject,
    pub on_fixed_update: Option<PyObject>,
    pub on_render: PyObject,
    pub last_time: Instant,
    pub accumulator: f64,
    pub gilrs: Option<Gilrs>,
    pub asset_loader: Option<AsyncAssetLoader>,
    pub runtime: Option<tokio::runtime::Runtime>,
}

impl ApplicationHandler for StarlightRunner {
    fn resumed(&mut self, event_loop: &ActiveEventLoop) {
        if self.gilrs.is_none() {
            match Gilrs::new() {
                Ok(g) => self.gilrs = Some(g),
                Err(e) => eprintln!("Failed to initialize Gilrs: {}", e),
            }
        }
        event_loop.set_control_flow(ControlFlow::Poll);

        let window_attrs = Window::default_attributes()
            .with_title(&self.title)
            .with_inner_size(winit::dpi::PhysicalSize::new(self.width, self.height));

        let window = event_loop.create_window(window_attrs).unwrap();

        let rt = tokio::runtime::Runtime::new().unwrap();

        let vfs = Vfs::new();
        if let Ok(cwd) = std::env::current_dir() {
            let _ = vfs.mount("/", cwd);
        }

        let loader_vfs = vfs.clone();
        let (loader, loader_rx, render_state) = rt.block_on(async {
            let (l, rx) = AsyncAssetLoader::new(loader_vfs);
            let rs = RenderState::new(Some(&window), self.width, self.height).await;
            (l, rx, rs)
        });

        let loader_tx = loader.sender.clone();
        self.asset_loader = Some(loader);
        self.runtime = Some(rt);
        let audio_state = AudioState::new();

        let mut world = World::default();
        let mut schedule = Schedule::default();
        let mut fixed_schedule = Schedule::default();

        world.insert_resource(InputState::default());
        world.insert_resource(PhysicsState::default());
        world.insert_resource(CollisionEvents::default());
        world.insert_resource(SceneRegistry::default());

        schedule.add_systems((
            input_reset_system
                .before(action_update_system)
                .before(camera_movement_system),
            action_update_system,
            camera_movement_system,
            transform_propagation_system,
            apply_deferred,
        ));

        fixed_schedule.add_systems((
            snapshot_previous_state_system.before(physics_step_system),
            physics_step_system,
            collision_event_system.after(physics_step_system),
            sync_transforms_system.after(physics_step_system),
        ));

        // Default Camera
        let cam_id = world
            .spawn((
                Camera {
                    fov: 60.0f32.to_radians(),
                    near: 0.1,
                    far: 1000.0,
                },
                Transform {
                    x: 0.0,
                    y: 5.0,
                    z: 10.0,
                },
                Rotation {
                    x: -0.2,
                    y: 0.0,
                    z: 0.0,
                },
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

        {
            let mut guard = GAME_APP.lock().unwrap();
            *guard = Some(GameApp {
                world,
                schedule,
                fixed_schedule,
                render_state: Some(render_state),
                audio_state: Some(audio_state),
                asset_registry: AssetRegistry::new(),
                asset_loader_rx: Some(loader_rx),
                asset_loader_tx: Some(loader_tx),
                vfs: vfs.clone(),
                network_state: Some(NetworkState::new()),
            });
        }

        self.window = Some(window);
        self.last_time = Instant::now();
        self.accumulator = 0.0;
    }

    fn device_event(
        &mut self,
        _event_loop: &ActiveEventLoop,
        _device_id: winit::event::DeviceId,
        event: winit::event::DeviceEvent,
    ) {
        match event {
            winit::event::DeviceEvent::MouseMotion { delta } => {
                let mut guard = GAME_APP.lock().unwrap();
                if let Some(app) = guard.as_mut() {
                    if let Some(mut input) = app.world.get_resource_mut::<InputState>() {
                        input.mouse_delta.0 += delta.0 as f32;
                        input.mouse_delta.1 += delta.1 as f32;
                    }
                }
            }
            _ => {}
        }
    }

    fn window_event(
        &mut self,
        event_loop: &ActiveEventLoop,
        _window_id: WindowId,
        event: WindowEvent,
    ) {
        match event {
            WindowEvent::CloseRequested => {
                event_loop.exit();
            }
            WindowEvent::Resized(size) => {
                let mut guard = GAME_APP.lock().unwrap();
                if let Some(app) = guard.as_mut() {
                    if let Some(rs) = app.render_state.as_mut() {
                        rs.resize(size.width, size.height);
                    }
                }
            }
            WindowEvent::KeyboardInput { event, .. } => {
                let mut guard = GAME_APP.lock().unwrap();
                if let Some(app) = guard.as_mut() {
                    if let Some(mut input) = app.world.get_resource_mut::<InputState>() {
                        if let winit::keyboard::PhysicalKey::Code(keycode) = event.physical_key {
                            let key_str = format!("{:?}", keycode);
                            let k = key_str.replace("Key", "").to_lowercase();
                            if event.state == winit::event::ElementState::Pressed {
                                if !input.pressed_keys.contains(&k) {
                                    input.just_pressed.insert(k.clone());
                                }
                                input.pressed_keys.insert(k);
                            } else {
                                input.pressed_keys.remove(&k);
                                input.just_released.insert(k);
                            }
                        }
                    }
                }
            }
            WindowEvent::RedrawRequested => {
                if self.window.is_none() {
                    return;
                }

                // Poll Asset Loader
                {
                    let mut guard = GAME_APP.lock().unwrap();
                    if let Some(app) = guard.as_mut() {
                        if let Some(rx) = &mut app.asset_loader_rx {
                            while let Ok(data) = rx.try_recv() {
                                match data {
                                    AssetData::Texture {
                                        uuid,
                                        img,
                                        is_normal_map,
                                    } => {
                                        if let Some(rs) = app.render_state.as_mut() {
                                            rs.update_texture(&uuid, &img, is_normal_map);
                                            app.asset_registry.mark_loaded(&uuid);
                                            println!("Loaded Texture: {}", uuid);
                                        }
                                    }
                                    AssetData::Mesh {
                                        uuid,
                                        vertices,
                                        indices,
                                    } => {
                                        if let Some(rs) = app.render_state.as_mut() {
                                            rs.upload_mesh(&uuid, &vertices, &indices);
                                            app.asset_registry.mark_loaded(&uuid);
                                            println!("Loaded Mesh: {}", uuid);
                                        }
                                    }
                                    AssetData::Skybox { uuid, images } => {
                                        if let Some(rs) = app.render_state.as_mut() {
                                            rs.update_skybox(&images);
                                            app.asset_registry.mark_loaded(&uuid);
                                            println!("Loaded Skybox: {}", uuid);
                                        }
                                    }
                                    AssetData::Scene { uuid, graph } => {
                                        app.world
                                            .resource_mut::<SceneRegistry>()
                                            .graphs
                                            .insert(uuid.clone(), graph);
                                        app.asset_registry.mark_loaded(&uuid);
                                        println!("Loaded Scene: {}", uuid);
                                    }
                                    AssetData::Error { uuid, error } => {
                                        eprintln!("Failed to load asset {}: {}", uuid, error);
                                        app.asset_registry.mark_failed(&uuid);
                                    }
                                }
                            }
                        }
                    }
                }

                // Poll Gilrs
                if let Some(gilrs) = &mut self.gilrs {
                    while let Some(GilrsEvent { id, event, .. }) = gilrs.next_event() {
                        let gamepad_id = id.into();
                        let mut guard = GAME_APP.lock().unwrap();
                        if let Some(app) = guard.as_mut() {
                            if let Some(mut input) = app.world.get_resource_mut::<InputState>() {
                                match event {
                                    gilrs::EventType::Connected => {
                                        input.connected_gamepads.insert(gamepad_id);
                                    }
                                    gilrs::EventType::Disconnected => {
                                        input.connected_gamepads.remove(&gamepad_id);
                                    }
                                    gilrs::EventType::ButtonPressed(button, _) => {
                                        input.gamepad_buttons.insert((gamepad_id, button));
                                    }
                                    gilrs::EventType::ButtonReleased(button, _) => {
                                        input.gamepad_buttons.remove(&(gamepad_id, button));
                                    }
                                    gilrs::EventType::AxisChanged(axis, value, _) => {
                                        input.gamepad_axes.insert((gamepad_id, axis), value);
                                    }
                                    _ => {}
                                }
                            }
                        }
                    }
                }

                let now = Instant::now();
                let mut dt = (now - self.last_time).as_secs_f64();
                self.last_time = now;

                if dt > MAX_FRAME_TIME {
                    dt = MAX_FRAME_TIME;
                }

                self.accumulator += dt;

                while self.accumulator >= FIXED_TIMESTEP {
                    if let Some(on_fixed) = &self.on_fixed_update {
                        Python::with_gil(|py| {
                            let _ = on_fixed.call1(py, (FIXED_TIMESTEP,));
                        });
                    }

                    {
                        let mut guard = GAME_APP.lock().unwrap();
                        if let Some(app) = guard.as_mut() {
                            app.fixed_schedule.run(&mut app.world);
                        }
                    }

                    self.accumulator -= FIXED_TIMESTEP;
                }

                let mut should_exit = false;
                Python::with_gil(|py| {
                    if let Ok(backend) = py.import("starlight.backend") {
                        if let Err(e) = self.on_update.call1(py, (dt, backend, py.None())) {
                            if e.is_instance_of::<pyo3::exceptions::PySystemExit>(py) {
                                should_exit = true;
                            } else {
                                e.print(py);
                            }
                        }
                    }
                });

                if should_exit {
                    event_loop.exit();
                    return;
                }

                {
                    let mut guard = GAME_APP.lock().unwrap();
                    if let Some(app) = guard.as_mut() {
                        app.schedule.run(&mut app.world);
                    }
                }

                {
                    let mut guard = GAME_APP.lock().unwrap();
                    if let Some(app) = guard.as_mut() {
                        // Render logic (simplified for readability, moved from monolithic lib.rs)
                        if let Some(rs) = app.render_state.as_mut() {
                            // Update Camera
                            let mut cam_query = app.world.query::<(
                                &Camera,
                                &Transform,
                                Option<&Rotation>,
                                Option<&GlobalTransform>,
                            )>();

                            if let Some((camera, t, r_opt, gt_opt)) =
                                cam_query.iter(&app.world).next()
                            {
                                let (position, rotation_quat) = if let Some(gt) = gt_opt {
                                    let (_scale, rot, pos) = gt.0.to_scale_rotation_translation();
                                    (pos, rot)
                                } else {
                                    let pos = Vec3::new(t.x, t.y, t.z);
                                    let rot = if let Some(r) = r_opt {
                                        Quat::from_euler(glam::EulerRot::XYZ, r.x, r.y, 0.0)
                                    } else {
                                        Quat::IDENTITY
                                    };
                                    (pos, rot)
                                };

                                let projection = Mat4::perspective_rh(
                                    camera.fov,
                                    self.width as f32 / self.height as f32,
                                    camera.near,
                                    camera.far,
                                );
                                let forward = rotation_quat * Vec3::NEG_Z;
                                let target = position + forward;
                                let view = Mat4::look_at_rh(position, target, Vec3::Y);
                                let view_proj = projection * view;

                                rs.update_camera(
                                    view_proj.to_cols_array_2d(),
                                    [position.x, position.y, position.z],
                                );
                            }

                            rs.clear_instances();

                            let mut query = app.world.query_filtered::<(
                                Entity,
                                &Transform,
                                &Rotation,
                                Option<&GlobalTransform>,
                                Option<&MeshName>,
                                Option<&Material>,
                                Option<&Color>,
                                Option<&Scale>,
                                Option<&AnimationController>,
                            ), Without<Camera>>(
                            );

                            for (entity, t, r, gt_opt, m_name, mat, col_opt, s_opt, anim_ctrl) in
                                query.iter(&app.world)
                            {
                                let model_matrix = if let Some(gt) = gt_opt {
                                    gt.0
                                } else {
                                    let scale = if let Some(s) = s_opt {
                                        Vec3::new(s.x, s.y, s.z)
                                    } else {
                                        Vec3::ONE
                                    };
                                    let rotation =
                                        Quat::from_euler(glam::EulerRot::XYZ, r.x, r.y, r.z);
                                    let translation = Vec3::new(t.x, t.y, t.z);
                                    Mat4::from_scale_rotation_translation(
                                        scale,
                                        rotation,
                                        translation,
                                    )
                                };

                                let mesh = if let Some(m) = m_name { &m.0 } else { "cube" };
                                let (diffuse, normal, metallic, roughness) = if let Some(mt) = mat {
                                    (&mt.diffuse, &mt.normal, mt.metallic, mt.roughness)
                                } else {
                                    (&"default".to_string(), &"flat_normal".to_string(), 0.0, 0.5)
                                };
                                let color = if let Some(c) = col_opt {
                                    [c.r, c.g, c.b, c.a]
                                } else {
                                    [1.0, 1.0, 1.0, 1.0]
                                };

                                let anim_id = if anim_ctrl.is_some() {
                                    Some(format!("{:?}", entity))
                                } else {
                                    None
                                };

                                if color[3] < 0.99 {
                                    rs.add_transparent_instance(
                                        mesh,
                                        diffuse,
                                        normal,
                                        model_matrix.to_cols_array_2d(),
                                        color,
                                        metallic,
                                        roughness,
                                        anim_id,
                                    );
                                } else {
                                    rs.add_instance(
                                        mesh,
                                        diffuse,
                                        normal,
                                        model_matrix.to_cols_array_2d(),
                                        color,
                                        metallic,
                                        roughness,
                                        anim_id,
                                    );
                                }
                            }

                            rs.render();
                        }
                    }
                }

                Python::with_gil(|py| {
                    let _ = self.on_render.call0(py);
                });

                self.window.as_ref().unwrap().request_redraw();
            }
            _ => {}
        }
    }

    fn about_to_wait(&mut self, _event_loop: &ActiveEventLoop) {
        if let Some(window) = &self.window {
            window.request_redraw();
        }
    }
}
