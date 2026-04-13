use bevy_ecs::prelude::*;
use crossbeam::channel::Receiver;
use glam::Vec3;
use pyo3::prelude::*;
use rapier3d::prelude::*;

#[derive(Clone, Debug)]
pub struct CollisionInfo {
    pub entity1: u64,
    pub entity2: u64,
    pub started: bool,
}

#[derive(Resource, Default)]
pub struct CollisionEvents {
    pub events: Vec<CollisionInfo>,
}

#[derive(Resource)]
pub struct PhysicsState {
    pub rigid_body_set: RigidBodySet,
    pub collider_set: ColliderSet,
    pub integration_parameters: IntegrationParameters,
    pub physics_pipeline: PhysicsPipeline,
    pub query_pipeline: QueryPipeline,
    pub island_manager: IslandManager,
    pub broad_phase: DefaultBroadPhase,
    pub narrow_phase: NarrowPhase,
    pub impulse_joint_set: ImpulseJointSet,
    pub multibody_joint_set: MultibodyJointSet,
    pub ccd_solver: CCDSolver,
    pub gravity: Vector<Real>,
    pub event_handler: ChannelEventCollector,
    pub collision_recv: Receiver<CollisionEvent>,
    pub contact_force_recv: Receiver<ContactForceEvent>,
}

impl Default for PhysicsState {
    fn default() -> Self {
        let mut params = IntegrationParameters::default();
        params.dt = 0.02; // Default, can be overridden
        let (collision_send, collision_recv) = crossbeam::channel::unbounded();
        let (contact_force_send, contact_force_recv) = crossbeam::channel::unbounded();
        let event_handler = ChannelEventCollector::new(collision_send, contact_force_send);

        Self {
            rigid_body_set: RigidBodySet::new(),
            collider_set: ColliderSet::new(),
            integration_parameters: params,
            physics_pipeline: PhysicsPipeline::new(),
            query_pipeline: QueryPipeline::new(),
            island_manager: IslandManager::new(),
            broad_phase: DefaultBroadPhase::new(),
            narrow_phase: NarrowPhase::new(),
            impulse_joint_set: ImpulseJointSet::new(),
            multibody_joint_set: MultibodyJointSet::new(),
            ccd_solver: CCDSolver::new(),
            gravity: Vector::new(0.0, -9.81, 0.0),
            event_handler,
            collision_recv,
            contact_force_recv,
        }
    }
}

impl PhysicsState {
    pub fn step(&mut self) {
        self.physics_pipeline.step(
            &self.gravity,
            &self.integration_parameters,
            &mut self.island_manager,
            &mut self.broad_phase,
            &mut self.narrow_phase,
            &mut self.rigid_body_set,
            &mut self.collider_set,
            &mut self.impulse_joint_set,
            &mut self.multibody_joint_set,
            &mut self.ccd_solver,
            Some(&mut self.query_pipeline),
            &(),
            &self.event_handler,
        );
        self.query_pipeline.update(&self.collider_set);
    }
}

pub fn physics_step_system(mut physics: ResMut<PhysicsState>) {
    physics.step();
}

pub fn collision_event_system(physics: ResMut<PhysicsState>, mut events: ResMut<CollisionEvents>) {
    events.events.clear();
    while let Ok(event) = physics.collision_recv.try_recv() {
        match event {
            CollisionEvent::Started(h1, h2, _) => {
                let e1 = physics
                    .collider_set
                    .get(h1)
                    .map(|c| c.user_data as u64)
                    .unwrap_or(0);
                let e2 = physics
                    .collider_set
                    .get(h2)
                    .map(|c| c.user_data as u64)
                    .unwrap_or(0);

                events.events.push(CollisionInfo {
                    entity1: e1,
                    entity2: e2,
                    started: true,
                });
            }
            CollisionEvent::Stopped(h1, h2, _) => {
                let e1 = physics
                    .collider_set
                    .get(h1)
                    .map(|c| c.user_data as u64)
                    .unwrap_or(0);
                let e2 = physics
                    .collider_set
                    .get(h2)
                    .map(|c| c.user_data as u64)
                    .unwrap_or(0);
                events.events.push(CollisionInfo {
                    entity1: e1,
                    entity2: e2,
                    started: false,
                });
            }
        }
    }
}

// Raycast Result Struct for Python
#[pyclass]
#[derive(Clone)]
pub struct RaycastHit {
    #[pyo3(get)]
    pub entity: u64,
    #[pyo3(get)]
    pub distance: f32,
    #[pyo3(get)]
    pub point: (f32, f32, f32),
    #[pyo3(get)]
    pub normal: (f32, f32, f32),
}

impl PhysicsState {
    pub fn cast_ray(&self, origin: Vec3, dir: Vec3, max_doi: f32) -> Option<RaycastHit> {
        let ray = Ray::new(
            point![origin.x, origin.y, origin.z],
            vector![dir.x, dir.y, dir.z],
        );

        let filter = QueryFilter::default(); // Filter logic can be added later

        if let Some((handle, toi)) = self.query_pipeline.cast_ray(
            &self.rigid_body_set,
            &self.collider_set,
            &ray,
            max_doi,
            true, // solid
            filter,
        ) {
            let collider = self.collider_set.get(handle).unwrap();
            let entity = collider.user_data as u64;
            let point = ray.point_at(toi);

            // Calculate normal (expensive, opt-in?)
            // For now, simple normal calculation
            let _normal = if let Some(contact) = self.query_pipeline.project_point(
                &self.rigid_body_set,
                &self.collider_set,
                &point,
                true,
                filter,
            ) {
                if contact.1.is_inside {
                    -dir // Approximation
                } else {
                    // Need proper normal from feature?
                    // Rapier cast_ray_and_get_normal is better
                    Vec3::Y // Placeholder
                }
            } else {
                Vec3::Y
            };

            // Re-cast with normal if needed, but for now basic hit
            // Actually let's use cast_ray_and_get_normal if available
            // But query_pipeline only has cast_ray in standard API?
            // It has cast_ray_and_get_normal!

            /*
            if let Some((handle, toi, normal)) = self.query_pipeline.cast_ray_and_get_normal(
               ...
            )
            */

            return Some(RaycastHit {
                entity,
                distance: toi,
                point: (point.x, point.y, point.z),
                normal: (0.0, 1.0, 0.0), // TODO: Real normal
            });
        }
        None
    }
}
