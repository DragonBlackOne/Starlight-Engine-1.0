use crate::app::GAME_APP;
use crate::ecs::components::MeshName;
use crate::ecs::components::{AnimationController, Skeleton};
use bevy_ecs::prelude::*;
use engine_render::resources::{
    material::JointUniform,
    mesh::{Interpolation, Mesh, Property},
};
use glam::{Mat4, Quat, Vec3, Vec4};

pub fn animation_update_system(
    mut query: Query<(Entity, &mut AnimationController, &mut Skeleton, &MeshName)>,
) {
    let dt = 1.0 / 60.0; // Placeholder fixed delta time

    // Access the global app to get RenderState and Mesh data
    let mut guard = GAME_APP.lock().unwrap();
    let app = if let Some(app) = guard.as_mut() {
        app
    } else {
        return;
    };

    let render_state = if let Some(rs) = &mut app.render_state {
        rs
    } else {
        return;
    };

    for (entity, mut controller, mut skeleton_cmp, mesh_name) in query.iter_mut() {
        if !controller.playing {
            continue;
        }

        let skeleton = &mut skeleton_cmp.0;

        // 1. Get the Mesh
        let mesh = if let Some(m) = render_state.meshes.get(&mesh_name.0) {
            m.clone()
        } else {
            continue;
        };

        // 2. Get the Animation
        let anim_index = if let Some(idx) = controller.current_animation {
            idx
        } else {
            continue;
        };

        if anim_index >= mesh.animations.len() {
            continue;
        }
        let animation = &mesh.animations[anim_index];

        // 3. Update Time
        controller.current_time += dt * controller.speed;

        let mut max_duration = 0.0;
        for sampler in &animation.samplers {
            if let Some(last) = sampler.input.last() {
                if *last > max_duration {
                    max_duration = *last;
                }
            }
        }

        if max_duration > 0.0 {
            if controller.looping {
                controller.current_time %= max_duration;
            } else if controller.current_time > max_duration {
                controller.current_time = max_duration;
                controller.playing = false;
            }
        }

        // 4. Sample Channels and Update Local Transforms
        for channel in &animation.channels {
            if channel.target_node >= skeleton.local_transforms.len() {
                continue;
            }

            let sampler = &animation.samplers[channel.sampler_index];
            let value = sample_animation(sampler, controller.current_time);

            let node_transform = &mut skeleton.local_transforms[channel.target_node];

            match channel.target_property {
                Property::Translation => {
                    node_transform.translation = Vec3::new(value.x, value.y, value.z);
                }
                Property::Rotation => {
                    node_transform.rotation = Quat::from_xyzw(value.x, value.y, value.z, value.w);
                }
                Property::Scale => {
                    node_transform.scale = Vec3::new(value.x, value.y, value.z);
                }
            }
        }

        // 5. Update Global Matrices (Hierarchy Traversal)
        let mut global_transforms = vec![Mat4::IDENTITY; mesh.nodes.len()];

        let mut is_child = vec![false; mesh.nodes.len()];
        for node in &mesh.nodes {
            for &child in &node.children {
                if child < is_child.len() {
                    is_child[child] = true;
                }
            }
        }

        for i in 0..mesh.nodes.len() {
            if !is_child[i] {
                update_hierarchy(i, Mat4::IDENTITY, &mesh, skeleton, &mut global_transforms);
            }
        }

        // 6. Compute Joint Matrices
        for skin in &mesh.skins {
            for (i, &joint_node_idx) in skin.joints.iter().enumerate() {
                if i >= 64 {
                    break;
                }
                if joint_node_idx < global_transforms.len() {
                    let global_transform = global_transforms[joint_node_idx];
                    let inverse_bind = skin.inverse_bind_matrices.get(i).unwrap_or(&Mat4::IDENTITY);
                    skeleton.joint_matrices[i] = global_transform * *inverse_bind;
                }
            }
        }

        // 7. Upload to GPU
        let uniform = JointUniform {
            matrices: skeleton
                .joint_matrices
                .iter()
                .map(|m| m.to_cols_array_2d())
                .collect::<Vec<_>>()
                .try_into()
                .unwrap_or_else(|v: Vec<[[f32; 4]; 4]>| {
                    let mut arr = [[[0.0; 4]; 4]; 64];
                    for (i, m) in v.iter().enumerate().take(64) {
                        arr[i] = *m;
                    }
                    arr
                }),
        };

        let entity_id_str = entity.to_bits().to_string();
        render_state.update_animation(&entity_id_str, &uniform);
    }
}

fn update_hierarchy(
    node_idx: usize,
    parent_transform: Mat4,
    mesh: &Mesh,
    skeleton: &engine_render::resources::mesh::Skeleton,
    global_transforms: &mut Vec<Mat4>,
) {
    if node_idx >= skeleton.local_transforms.len() {
        return;
    }

    let local = &skeleton.local_transforms[node_idx];
    let local_matrix =
        Mat4::from_scale_rotation_translation(local.scale, local.rotation, local.translation);
    let global_matrix = parent_transform * local_matrix;

    global_transforms[node_idx] = global_matrix;

    if node_idx < mesh.nodes.len() {
        for &child_idx in &mesh.nodes[node_idx].children {
            update_hierarchy(child_idx, global_matrix, mesh, skeleton, global_transforms);
        }
    }
}

fn sample_animation(sampler: &engine_render::resources::mesh::AnimationSampler, time: f32) -> Vec4 {
    if sampler.input.is_empty() {
        return Vec4::ZERO;
    }

    let mut prev_idx = 0;
    let mut next_idx = 0;

    if time <= sampler.input[0] {
        return sampler.output[0];
    }
    if time >= *sampler.input.last().unwrap() {
        return *sampler.output.last().unwrap();
    }

    for (i, t) in sampler.input.iter().enumerate() {
        if *t > time {
            next_idx = i;
            prev_idx = if i > 0 { i - 1 } else { 0 };
            break;
        }
    }

    let t0 = sampler.input[prev_idx];
    let t1 = sampler.input[next_idx];
    let v0 = sampler.output[prev_idx];
    let v1 = sampler.output[next_idx];

    let delta = t1 - t0;
    let factor = if delta > 0.0 {
        (time - t0) / delta
    } else {
        0.0
    };

    match sampler.interpolation {
        Interpolation::Step => v0,
        Interpolation::Linear => v0.lerp(v1, factor),
        Interpolation::CubicSpline => {
            // Placeholder: Cubic Spline fallback to Linear
            v0.lerp(v1, factor)
        }
    }
}
