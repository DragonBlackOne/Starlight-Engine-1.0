use bevy_ecs::prelude::*;
use crate::ecs::components::*;
use crate::physics::PhysicsState;
use glam::{Mat4, Quat, Vec3};
use std::collections::HashMap;

pub fn snapshot_previous_state_system(
    mut query: Query<(
        &Transform,
        &Rotation,
        &mut PreviousTransform,
        &mut PreviousRotation,
    )>,
) {
    for (t, r, mut pt, mut pr) in query.iter_mut() {
        pt.x = t.x;
        pt.y = t.y;
        pt.z = t.z;
        pr.x = r.x;
        pr.y = r.y;
        pr.z = r.z;
    }
}

pub fn sync_transforms_system(
    physics: Res<PhysicsState>,
    mut query: Query<(&mut Transform, &mut Rotation, &RigidBodyComponent)>,
) {
    for (mut transform, mut rotation, rb_handle) in query.iter_mut() {
        if let Some(rb) = physics.rigid_body_set.get(rb_handle.0) {
            let translation = rb.translation();
            let rot = rb.rotation();
            let (roll, pitch, yaw) = rot.euler_angles();

            transform.x = translation.x;
            transform.y = translation.y;
            transform.z = translation.z;
            rotation.x = roll;
            rotation.y = pitch;
            rotation.z = yaw;
        }
    }
}

pub fn transform_propagation_system(
    mut commands: Commands,
    query: Query<(
        Entity,
        &Transform,
        &Rotation,
        Option<&Scale>,
        Option<&Parent>,
    )>,
) {
    let mut hierarchy: HashMap<Entity, Vec<Entity>> = HashMap::new();
    let mut roots: Vec<Entity> = Vec::new();
    let mut entity_data: HashMap<Entity, (Vec3, Quat, Vec3)> = HashMap::new();

    for (e, t, r, s, p) in query.iter() {
        let scale = if let Some(sc) = s {
            Vec3::new(sc.x, sc.y, sc.z)
        } else {
            Vec3::ONE
        };
        let translation = Vec3::new(t.x, t.y, t.z);
        let rotation = Quat::from_euler(glam::EulerRot::XYZ, r.x, r.y, r.z);
        entity_data.insert(e, (translation, rotation, scale));

        if let Some(parent) = p {
            let parent_entity = Entity::from_bits(parent.0);
            hierarchy.entry(parent_entity).or_default().push(e);
        } else {
            roots.push(e);
        }
    }

    let mut queue: Vec<(Entity, Mat4)> = roots.iter().map(|&e| (e, Mat4::IDENTITY)).collect();

    while let Some((entity, parent_mat)) = queue.pop() {
        if let Some(&(t, r, s)) = entity_data.get(&entity) {
            let local_mat = Mat4::from_scale_rotation_translation(s, r, t);
            let global_mat = parent_mat * local_mat;

            commands.entity(entity).insert(GlobalTransform(global_mat));

            if let Some(children) = hierarchy.get(&entity) {
                for &child in children {
                    queue.push((child, global_mat));
                }
            }
        }
    }
}

// Camera movement can be in python, but we had a stub.
pub fn camera_movement_system(_: Query<&mut Transform, With<Camera>>) {
    // Moved to Python
}
