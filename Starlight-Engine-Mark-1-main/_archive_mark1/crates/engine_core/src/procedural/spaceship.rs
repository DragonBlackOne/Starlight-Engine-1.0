use super::mesh_builder::MeshBuilder;
use glam::Vec3;

pub struct ProceduralSpaceship;

impl ProceduralSpaceship {
    pub fn generate(seed: u32, mesh: &mut MeshBuilder) {
        let mut rng = Random::new(seed);

        // Ship style based on seed
        let style = rng.range_int(0, 4);

        match style {
            0 => Self::generate_fighter(&mut rng, mesh),
            1 => Self::generate_bomber(&mut rng, mesh),
            2 => Self::generate_interceptor(&mut rng, mesh),
            _ => Self::generate_cruiser(&mut rng, mesh),
        }
    }

    /// Fast, sleek fighter ship
    fn generate_fighter(rng: &mut Random, mesh: &mut MeshBuilder) {
        let primary = rng.color();
        let accent = rng.color_bright();
        let glass = [0.2, 0.8, 1.0, 0.8];

        // Main fuselage (elongated cone)
        mesh.add_cone(Vec3::new(0.0, 0.0, 0.0), 0.4, 3.0, 8, primary);

        // Cockpit (sphere)
        mesh.add_sphere(Vec3::new(0.0, 0.25, 0.8), 0.3, 12, 8, glass);

        // Wings (wedges)
        mesh.add_wedge(Vec3::new(-1.2, 0.0, -0.3), Vec3::new(1.5, 0.1, 0.8), accent);
        mesh.add_wedge(Vec3::new(1.2, 0.0, -0.3), Vec3::new(1.5, 0.1, 0.8), accent);

        // Engine nacelles (cylinders)
        let engine_color = [1.0, 0.5, 0.1, 1.0];
        mesh.add_cylinder(Vec3::new(-0.6, 0.0, -1.5), 0.2, 0.8, 8, engine_color);
        mesh.add_cylinder(Vec3::new(0.6, 0.0, -1.5), 0.2, 0.8, 8, engine_color);

        // Engine glow (small spheres)
        let glow = [1.0, 0.8, 0.3, 1.0];
        mesh.add_sphere(Vec3::new(-0.6, 0.0, -1.9), 0.15, 8, 6, glow);
        mesh.add_sphere(Vec3::new(0.6, 0.0, -1.9), 0.15, 8, 6, glow);

        // Tail fins
        mesh.add_pyramid(
            Vec3::new(0.0, 0.3, -1.0),
            Vec3::new(0.1, 0.0, 0.4),
            0.5,
            accent,
        );
    }

    /// Heavy bomber with large cargo bay
    fn generate_bomber(rng: &mut Random, mesh: &mut MeshBuilder) {
        let primary = rng.color_dark();
        let accent = rng.color();
        let glass = [0.3, 0.6, 0.9, 0.8];

        // Main hull (large cube)
        mesh.add_cube(Vec3::new(0.0, 0.0, 0.0), Vec3::new(1.5, 0.8, 3.0), primary);

        // Cockpit (half-sphere on top)
        mesh.add_sphere(Vec3::new(0.0, 0.5, 1.0), 0.4, 10, 6, glass);

        // Wings (flat boxes)
        mesh.add_cube(
            Vec3::new(-2.0, 0.0, -0.2),
            Vec3::new(2.5, 0.15, 1.2),
            accent,
        );
        mesh.add_cube(Vec3::new(2.0, 0.0, -0.2), Vec3::new(2.5, 0.15, 1.2), accent);

        // Engine pods (4 cylinders)
        let engine = [0.8, 0.4, 0.1, 1.0];
        for x in [-2.5, -1.5, 1.5, 2.5] {
            mesh.add_cylinder(Vec3::new(x, -0.2, -1.5), 0.25, 1.0, 8, engine);
        }

        // Bomb bay doors (darker cube underneath)
        mesh.add_cube(
            Vec3::new(0.0, -0.5, -0.5),
            Vec3::new(1.0, 0.1, 1.5),
            [0.2, 0.2, 0.25, 1.0],
        );

        // Tail section
        mesh.add_cube(Vec3::new(0.0, 0.3, -1.8), Vec3::new(0.4, 0.6, 0.8), accent);
    }

    /// Fast interceptor with forward-swept wings
    fn generate_interceptor(rng: &mut Random, mesh: &mut MeshBuilder) {
        let primary = rng.color_bright();
        let accent = rng.color_dark();
        let glass = [0.1, 0.9, 0.9, 0.8];

        // Slim fuselage (elongated wedge)
        mesh.add_wedge(Vec3::new(0.0, 0.0, 0.0), Vec3::new(0.6, 0.4, 4.0), primary);

        // Cockpit (elongated sphere)
        mesh.add_sphere(Vec3::new(0.0, 0.3, 1.2), 0.25, 10, 6, glass);

        // Forward-swept wings (rotated wedges)
        mesh.add_wedge(Vec3::new(-1.0, 0.0, 0.8), Vec3::new(1.2, 0.08, 1.0), accent);
        mesh.add_wedge(Vec3::new(1.0, 0.0, 0.8), Vec3::new(1.2, 0.08, 1.0), accent);

        // Canards (small front wings)
        mesh.add_cube(Vec3::new(-0.5, 0.0, 1.5), Vec3::new(0.6, 0.05, 0.3), accent);
        mesh.add_cube(Vec3::new(0.5, 0.0, 1.5), Vec3::new(0.6, 0.05, 0.3), accent);

        // Single large engine
        let engine = [1.0, 0.6, 0.2, 1.0];
        mesh.add_cylinder(Vec3::new(0.0, 0.0, -2.0), 0.35, 1.2, 10, engine);
        mesh.add_sphere(Vec3::new(0.0, 0.0, -2.6), 0.25, 8, 6, [1.0, 0.9, 0.5, 1.0]);

        // Vertical stabilizers
        mesh.add_pyramid(
            Vec3::new(-0.3, 0.2, -1.5),
            Vec3::new(0.05, 0.0, 0.3),
            0.6,
            primary,
        );
        mesh.add_pyramid(
            Vec3::new(0.3, 0.2, -1.5),
            Vec3::new(0.05, 0.0, 0.3),
            0.6,
            primary,
        );
    }

    /// Large capital ship / cruiser
    fn generate_cruiser(rng: &mut Random, mesh: &mut MeshBuilder) {
        let hull = rng.color_dark();
        let bridge = rng.color();
        let glass = [0.3, 0.7, 1.0, 0.7];

        // Main hull (large elongated box)
        mesh.add_cube(Vec3::new(0.0, 0.0, 0.0), Vec3::new(2.0, 1.0, 5.0), hull);

        // Bridge tower
        mesh.add_cube(Vec3::new(0.0, 0.8, 1.0), Vec3::new(0.8, 0.6, 1.0), bridge);
        mesh.add_sphere(Vec3::new(0.0, 1.2, 1.2), 0.3, 8, 6, glass);

        // Side hangars
        mesh.add_cube(Vec3::new(-1.3, -0.2, 0.0), Vec3::new(0.6, 0.6, 2.0), hull);
        mesh.add_cube(Vec3::new(1.3, -0.2, 0.0), Vec3::new(0.6, 0.6, 2.0), hull);

        // Engine array (multiple cylinders)
        let engine = [0.9, 0.5, 0.2, 1.0];
        for x in [-0.6, 0.0, 0.6] {
            for y in [-0.3, 0.3] {
                mesh.add_cylinder(Vec3::new(x, y, -2.5), 0.2, 0.6, 8, engine);
            }
        }

        // Weapon turrets (spheres on top)
        let turret = [0.5, 0.5, 0.6, 1.0];
        mesh.add_sphere(Vec3::new(-0.5, 0.6, 0.0), 0.2, 8, 6, turret);
        mesh.add_sphere(Vec3::new(0.5, 0.6, 0.0), 0.2, 8, 6, turret);
        mesh.add_sphere(Vec3::new(0.0, 0.6, -1.5), 0.25, 8, 6, turret);

        // Antenna array
        mesh.add_cylinder(Vec3::new(0.0, 1.6, 1.0), 0.02, 0.5, 4, [0.8, 0.8, 0.8, 1.0]);
        mesh.add_sphere(Vec3::new(0.0, 1.9, 1.0), 0.05, 6, 4, [1.0, 0.2, 0.2, 1.0]);
    }
}

// Minimal RNG helper
struct Random {
    state: u32,
}

impl Random {
    fn new(seed: u32) -> Self {
        Self {
            state: seed.wrapping_add(1),
        }
    }

    fn next(&mut self) -> u32 {
        self.state = self.state.wrapping_mul(747796405).wrapping_add(2891336453);
        let x = ((self.state >> ((self.state >> 28) + 4)) ^ self.state).wrapping_mul(277803737);
        (x >> 22) ^ x
    }

    fn range(&mut self, min: f32, max: f32) -> f32 {
        let f = (self.next() as f32) / (u32::MAX as f32);
        min + f * (max - min)
    }

    fn range_int(&mut self, min: i32, max: i32) -> i32 {
        let range = (max - min) as u32;
        if range == 0 {
            return min;
        }
        min + (self.next() % range) as i32
    }

    fn color(&mut self) -> [f32; 4] {
        [
            self.range(0.3, 0.9),
            self.range(0.3, 0.9),
            self.range(0.3, 0.9),
            1.0,
        ]
    }

    fn color_bright(&mut self) -> [f32; 4] {
        [
            self.range(0.6, 1.0),
            self.range(0.6, 1.0),
            self.range(0.6, 1.0),
            1.0,
        ]
    }

    fn color_dark(&mut self) -> [f32; 4] {
        [
            self.range(0.2, 0.5),
            self.range(0.2, 0.5),
            self.range(0.2, 0.5),
            1.0,
        ]
    }

    #[allow(dead_code)]
    fn color_variation(&mut self, base: [f32; 4]) -> [f32; 4] {
        let v = self.range(-0.15, 0.15);
        [
            (base[0] + v).clamp(0.0, 1.0),
            (base[1] + v).clamp(0.0, 1.0),
            (base[2] + v).clamp(0.0, 1.0),
            1.0,
        ]
    }
}
