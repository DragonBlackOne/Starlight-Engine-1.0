use glam::{Mat4, Vec2, Vec3};
use std::f32::consts::PI;

#[derive(Clone, Debug)]
pub struct Vertex {
    pub position: [f32; 3],
    pub normal: [f32; 3],
    pub tex_coords: [f32; 2],
    pub tangent: [f32; 4],
    pub color: [f32; 4],
}

/// Advanced procedural mesh builder with primitives and transformations.
pub struct MeshBuilder {
    pub vertices: Vec<Vertex>,
    pub indices: Vec<u32>,
}

impl MeshBuilder {
    pub fn new() -> Self {
        Self {
            vertices: Vec::new(),
            indices: Vec::new(),
        }
    }

    // ==================== BASIC METHODS ====================

    pub fn push_vertex(&mut self, pos: Vec3, normal: Vec3, uv: Vec2, color: [f32; 4]) {
        self.vertices.push(Vertex {
            position: pos.to_array(),
            normal: normal.to_array(),
            tex_coords: uv.to_array(),
            tangent: [0.0, 0.0, 0.0, 1.0], // Default tangent
            color,
        });
    }

    pub fn push_index(&mut self, index: u32) {
        self.indices.push(index);
    }

    pub fn vertex_count(&self) -> u32 {
        self.vertices.len() as u32
    }

    // ==================== PRIMITIVES ====================

    /// Adds a quad (4 vertices, 2 triangles)
    pub fn add_quad(&mut self, p1: Vec3, p2: Vec3, p3: Vec3, p4: Vec3, color: [f32; 4]) {
        let idx = self.vertex_count();
        let normal = (p2 - p1).cross(p3 - p1).normalize_or_zero();

        self.push_vertex(p1, normal, Vec2::new(0.0, 0.0), color);
        self.push_vertex(p2, normal, Vec2::new(1.0, 0.0), color);
        self.push_vertex(p3, normal, Vec2::new(1.0, 1.0), color);
        self.push_vertex(p4, normal, Vec2::new(0.0, 1.0), color);

        self.indices
            .extend([idx, idx + 1, idx + 2, idx, idx + 2, idx + 3]);
    }

    /// Adds a triangle (3 vertices)
    pub fn add_triangle(&mut self, p1: Vec3, p2: Vec3, p3: Vec3, color: [f32; 4]) {
        let idx = self.vertex_count();
        let normal = (p2 - p1).cross(p3 - p1).normalize_or_zero();

        self.push_vertex(p1, normal, Vec2::new(0.0, 0.0), color);
        self.push_vertex(p2, normal, Vec2::new(1.0, 0.0), color);
        self.push_vertex(p3, normal, Vec2::new(0.5, 1.0), color);

        self.indices.extend([idx, idx + 1, idx + 2]);
    }

    /// Adds a box/cube
    pub fn add_cube(&mut self, center: Vec3, size: Vec3, color: [f32; 4]) {
        let h = size * 0.5;
        let faces = [
            // Front (+Z)
            (
                Vec3::new(-h.x, -h.y, h.z),
                Vec3::new(h.x, -h.y, h.z),
                Vec3::new(h.x, h.y, h.z),
                Vec3::new(-h.x, h.y, h.z),
            ),
            // Back (-Z)
            (
                Vec3::new(h.x, -h.y, -h.z),
                Vec3::new(-h.x, -h.y, -h.z),
                Vec3::new(-h.x, h.y, -h.z),
                Vec3::new(h.x, h.y, -h.z),
            ),
            // Top (+Y)
            (
                Vec3::new(-h.x, h.y, h.z),
                Vec3::new(h.x, h.y, h.z),
                Vec3::new(h.x, h.y, -h.z),
                Vec3::new(-h.x, h.y, -h.z),
            ),
            // Bottom (-Y)
            (
                Vec3::new(-h.x, -h.y, -h.z),
                Vec3::new(h.x, -h.y, -h.z),
                Vec3::new(h.x, -h.y, h.z),
                Vec3::new(-h.x, -h.y, h.z),
            ),
            // Right (+X)
            (
                Vec3::new(h.x, -h.y, h.z),
                Vec3::new(h.x, -h.y, -h.z),
                Vec3::new(h.x, h.y, -h.z),
                Vec3::new(h.x, h.y, h.z),
            ),
            // Left (-X)
            (
                Vec3::new(-h.x, -h.y, -h.z),
                Vec3::new(-h.x, -h.y, h.z),
                Vec3::new(-h.x, h.y, h.z),
                Vec3::new(-h.x, h.y, -h.z),
            ),
        ];

        for (p1, p2, p3, p4) in faces {
            self.add_quad(center + p1, center + p2, center + p3, center + p4, color);
        }
    }

    /// Adds a UV sphere
    pub fn add_sphere(
        &mut self,
        center: Vec3,
        radius: f32,
        segments: u32,
        rings: u32,
        color: [f32; 4],
    ) {
        let segments = segments.max(4);
        let rings = rings.max(2);
        let base_idx = self.vertex_count();

        // Generate vertices
        for ring in 0..=rings {
            let phi = PI * ring as f32 / rings as f32;
            let y = phi.cos();
            let ring_radius = phi.sin();

            for seg in 0..=segments {
                let theta = 2.0 * PI * seg as f32 / segments as f32;
                let x = ring_radius * theta.cos();
                let z = ring_radius * theta.sin();

                let normal = Vec3::new(x, y, z);
                let pos = center + normal * radius;
                let uv = Vec2::new(seg as f32 / segments as f32, ring as f32 / rings as f32);

                self.push_vertex(pos, normal, uv, color);
            }
        }

        // Generate indices
        for ring in 0..rings {
            for seg in 0..segments {
                let current = base_idx + ring * (segments + 1) + seg;
                let next = current + segments + 1;

                self.indices.extend([current, next, current + 1]);
                self.indices.extend([current + 1, next, next + 1]);
            }
        }
    }

    /// Adds a cylinder with caps
    pub fn add_cylinder(
        &mut self,
        center: Vec3,
        radius: f32,
        height: f32,
        segments: u32,
        color: [f32; 4],
    ) {
        let segments = segments.max(3);
        let half_h = height * 0.5;
        let base_idx = self.vertex_count();

        // Side vertices
        for i in 0..=segments {
            let theta = 2.0 * PI * i as f32 / segments as f32;
            let x = theta.cos();
            let z = theta.sin();
            let normal = Vec3::new(x, 0.0, z);

            // Bottom vertex
            self.push_vertex(
                center + Vec3::new(x * radius, -half_h, z * radius),
                normal,
                Vec2::new(i as f32 / segments as f32, 0.0),
                color,
            );
            // Top vertex
            self.push_vertex(
                center + Vec3::new(x * radius, half_h, z * radius),
                normal,
                Vec2::new(i as f32 / segments as f32, 1.0),
                color,
            );
        }

        // Side indices
        for i in 0..segments {
            let base = base_idx + i * 2;
            self.indices.extend([base, base + 2, base + 1]);
            self.indices.extend([base + 1, base + 2, base + 3]);
        }

        // Top and bottom caps
        let top_center_idx = self.vertex_count();
        self.push_vertex(
            center + Vec3::new(0.0, half_h, 0.0),
            Vec3::Y,
            Vec2::new(0.5, 0.5),
            color,
        );

        let bottom_center_idx = self.vertex_count();
        self.push_vertex(
            center + Vec3::new(0.0, -half_h, 0.0),
            -Vec3::Y,
            Vec2::new(0.5, 0.5),
            color,
        );

        for i in 0..segments {
            let theta1 = 2.0 * PI * i as f32 / segments as f32;
            let theta2 = 2.0 * PI * (i + 1) as f32 / segments as f32;

            // Top cap
            let t1 = self.vertex_count();
            self.push_vertex(
                center + Vec3::new(theta1.cos() * radius, half_h, theta1.sin() * radius),
                Vec3::Y,
                Vec2::ZERO,
                color,
            );
            let t2 = self.vertex_count();
            self.push_vertex(
                center + Vec3::new(theta2.cos() * radius, half_h, theta2.sin() * radius),
                Vec3::Y,
                Vec2::ZERO,
                color,
            );
            self.indices.extend([top_center_idx, t1, t2]);

            // Bottom cap
            let b1 = self.vertex_count();
            self.push_vertex(
                center + Vec3::new(theta1.cos() * radius, -half_h, theta1.sin() * radius),
                -Vec3::Y,
                Vec2::ZERO,
                color,
            );
            let b2 = self.vertex_count();
            self.push_vertex(
                center + Vec3::new(theta2.cos() * radius, -half_h, theta2.sin() * radius),
                -Vec3::Y,
                Vec2::ZERO,
                color,
            );
            self.indices.extend([bottom_center_idx, b2, b1]);
        }
    }

    /// Adds a cone
    pub fn add_cone(
        &mut self,
        center: Vec3,
        radius: f32,
        height: f32,
        segments: u32,
        color: [f32; 4],
    ) {
        let segments = segments.max(3);
        let half_h = height * 0.5;

        // Apex
        let _apex_idx = self.vertex_count();
        self.push_vertex(
            center + Vec3::new(0.0, half_h, 0.0),
            Vec3::Y,
            Vec2::new(0.5, 1.0),
            color,
        );

        // Base center
        let base_center_idx = self.vertex_count();
        self.push_vertex(
            center + Vec3::new(0.0, -half_h, 0.0),
            -Vec3::Y,
            Vec2::new(0.5, 0.5),
            color,
        );

        // Generate cone sides and base
        for i in 0..segments {
            let theta1 = 2.0 * PI * i as f32 / segments as f32;
            let theta2 = 2.0 * PI * (i + 1) as f32 / segments as f32;

            let p1 = center + Vec3::new(theta1.cos() * radius, -half_h, theta1.sin() * radius);
            let p2 = center + Vec3::new(theta2.cos() * radius, -half_h, theta2.sin() * radius);
            let apex = center + Vec3::new(0.0, half_h, 0.0);

            // Side triangle
            let n = (p2 - apex).cross(p1 - apex).normalize_or_zero();
            let i1 = self.vertex_count();
            self.push_vertex(apex, n, Vec2::new(0.5, 1.0), color);
            let i2 = self.vertex_count();
            self.push_vertex(p1, n, Vec2::new(0.0, 0.0), color);
            let i3 = self.vertex_count();
            self.push_vertex(p2, n, Vec2::new(1.0, 0.0), color);
            self.indices.extend([i1, i2, i3]);

            // Base triangle
            let b1 = self.vertex_count();
            self.push_vertex(p1, -Vec3::Y, Vec2::ZERO, color);
            let b2 = self.vertex_count();
            self.push_vertex(p2, -Vec3::Y, Vec2::ZERO, color);
            self.indices.extend([base_center_idx, b2, b1]);
        }
    }

    /// Adds a torus
    pub fn add_torus(
        &mut self,
        center: Vec3,
        major_radius: f32,
        minor_radius: f32,
        segments: u32,
        rings: u32,
        color: [f32; 4],
    ) {
        let segments = segments.max(3);
        let rings = rings.max(3);
        let base_idx = self.vertex_count();

        for ring in 0..=rings {
            let theta = 2.0 * PI * ring as f32 / rings as f32;
            let ring_center =
                Vec3::new(theta.cos() * major_radius, 0.0, theta.sin() * major_radius);

            for seg in 0..=segments {
                let phi = 2.0 * PI * seg as f32 / segments as f32;

                let local_x = phi.cos() * minor_radius;
                let local_y = phi.sin() * minor_radius;

                let outward = Vec3::new(theta.cos(), 0.0, theta.sin());
                let pos = center + ring_center + outward * local_x + Vec3::Y * local_y;
                let normal = (outward * phi.cos() + Vec3::Y * phi.sin()).normalize();

                let uv = Vec2::new(ring as f32 / rings as f32, seg as f32 / segments as f32);
                self.push_vertex(pos, normal, uv, color);
            }
        }

        // Generate indices
        for ring in 0..rings {
            for seg in 0..segments {
                let current = base_idx + ring * (segments + 1) + seg;
                let next = current + segments + 1;

                self.indices.extend([current, next, current + 1]);
                self.indices.extend([current + 1, next, next + 1]);
            }
        }
    }

    /// Adds a pyramid
    pub fn add_pyramid(&mut self, center: Vec3, base_size: Vec3, height: f32, color: [f32; 4]) {
        let h = base_size * 0.5;
        let apex = center + Vec3::new(0.0, height, 0.0);

        let corners = [
            center + Vec3::new(-h.x, 0.0, -h.z),
            center + Vec3::new(h.x, 0.0, -h.z),
            center + Vec3::new(h.x, 0.0, h.z),
            center + Vec3::new(-h.x, 0.0, h.z),
        ];

        // Base
        self.add_quad(corners[0], corners[1], corners[2], corners[3], color);

        // Sides
        for i in 0..4 {
            self.add_triangle(corners[i], corners[(i + 1) % 4], apex, color);
        }
    }

    /// Adds a wedge/ramp
    pub fn add_wedge(&mut self, center: Vec3, size: Vec3, color: [f32; 4]) {
        let h = size * 0.5;

        // 6 corner points
        let p = [
            center + Vec3::new(-h.x, -h.y, -h.z), // 0: bottom left back
            center + Vec3::new(h.x, -h.y, -h.z),  // 1: bottom right back
            center + Vec3::new(h.x, -h.y, h.z),   // 2: bottom right front
            center + Vec3::new(-h.x, -h.y, h.z),  // 3: bottom left front
            center + Vec3::new(-h.x, h.y, h.z),   // 4: top left front
            center + Vec3::new(h.x, h.y, h.z),    // 5: top right front
        ];

        // Bottom
        self.add_quad(p[0], p[1], p[2], p[3], color);
        // Front (tall)
        self.add_quad(p[3], p[2], p[5], p[4], color);
        // Ramp (slope)
        self.add_quad(p[0], p[4], p[5], p[1], color);
        // Left triangle
        self.add_triangle(p[0], p[3], p[4], color);
        // Right triangle
        self.add_triangle(p[1], p[5], p[2], color);
    }

    /// Adds a capsule (cylinder with hemisphere caps)
    pub fn add_capsule(
        &mut self,
        center: Vec3,
        radius: f32,
        height: f32,
        segments: u32,
        color: [f32; 4],
    ) {
        let segments = segments.max(4);
        let half_h = (height * 0.5).max(0.0);

        // Cylinder body
        self.add_cylinder(center, radius, height - radius * 2.0, segments, color);

        // Top hemisphere
        let top_center = center + Vec3::new(0.0, half_h - radius, 0.0);
        for ring in 0..=segments / 2 {
            let phi = PI * 0.5 * ring as f32 / (segments as f32 / 2.0);
            let y = phi.sin();
            let ring_radius = phi.cos();

            for seg in 0..=segments {
                let theta = 2.0 * PI * seg as f32 / segments as f32;
                let normal = Vec3::new(ring_radius * theta.cos(), y, ring_radius * theta.sin());
                let pos = top_center + normal * radius;
                self.push_vertex(pos, normal, Vec2::ZERO, color);
            }
        }

        // Bottom hemisphere (inverted)
        let bottom_center = center - Vec3::new(0.0, half_h - radius, 0.0);
        for ring in 0..=segments / 2 {
            let phi = PI * 0.5 * ring as f32 / (segments as f32 / 2.0);
            let y = -phi.sin();
            let ring_radius = phi.cos();

            for seg in 0..=segments {
                let theta = 2.0 * PI * seg as f32 / segments as f32;
                let normal = Vec3::new(ring_radius * theta.cos(), y, ring_radius * theta.sin());
                let pos = bottom_center + normal * radius;
                self.push_vertex(pos, normal, Vec2::ZERO, color);
            }
        }
    }

    // ==================== TRANSFORMATIONS ====================

    /// Applies a 4x4 transformation matrix to all vertices
    pub fn transform(&mut self, matrix: Mat4) {
        let normal_matrix = matrix.inverse().transpose();
        for v in &mut self.vertices {
            let pos = Vec3::from_array(v.position);
            let normal = Vec3::from_array(v.normal);

            let new_pos = matrix.transform_point3(pos);
            let new_normal = normal_matrix.transform_vector3(normal).normalize_or_zero();

            v.position = new_pos.to_array();
            v.normal = new_normal.to_array();
        }
    }

    /// Translates all vertices by offset
    pub fn translate(&mut self, offset: Vec3) {
        self.transform(Mat4::from_translation(offset));
    }

    /// Rotates around X axis (angle in radians)
    pub fn rotate_x(&mut self, angle: f32) {
        self.transform(Mat4::from_rotation_x(angle));
    }

    /// Rotates around Y axis (angle in radians)
    pub fn rotate_y(&mut self, angle: f32) {
        self.transform(Mat4::from_rotation_y(angle));
    }

    /// Rotates around Z axis (angle in radians)
    pub fn rotate_z(&mut self, angle: f32) {
        self.transform(Mat4::from_rotation_z(angle));
    }

    /// Scales uniformly or non-uniformly
    pub fn scale(&mut self, factor: Vec3) {
        self.transform(Mat4::from_scale(factor));
    }

    /// Mirrors across X axis
    pub fn mirror_x(&mut self) {
        for v in &mut self.vertices {
            v.position[0] = -v.position[0];
            v.normal[0] = -v.normal[0];
        }
        // Flip winding order
        for chunk in self.indices.chunks_exact_mut(3) {
            chunk.swap(1, 2);
        }
    }

    /// Mirrors across Y axis
    pub fn mirror_y(&mut self) {
        for v in &mut self.vertices {
            v.position[1] = -v.position[1];
            v.normal[1] = -v.normal[1];
        }
        for chunk in self.indices.chunks_exact_mut(3) {
            chunk.swap(1, 2);
        }
    }

    /// Mirrors across Z axis
    pub fn mirror_z(&mut self) {
        for v in &mut self.vertices {
            v.position[2] = -v.position[2];
            v.normal[2] = -v.normal[2];
        }
        for chunk in self.indices.chunks_exact_mut(3) {
            chunk.swap(1, 2);
        }
    }

    // ==================== QUALITY METHODS ====================

    /// Merges another mesh into this one
    pub fn merge(&mut self, other: &MeshBuilder) {
        let offset = self.vertex_count();
        self.vertices.extend(other.vertices.iter().cloned());
        self.indices
            .extend(other.indices.iter().map(|i| i + offset));
    }

    /// Recalculates smooth normals (averaged per vertex position)
    pub fn compute_smooth_normals(&mut self) {
        use std::collections::HashMap;

        // Accumulate normals for each unique position
        let mut normal_map: HashMap<[i32; 3], Vec3> = HashMap::new();

        let to_key = |p: [f32; 3]| -> [i32; 3] {
            [
                (p[0] * 1000.0) as i32,
                (p[1] * 1000.0) as i32,
                (p[2] * 1000.0) as i32,
            ]
        };

        // First pass: calculate face normals and accumulate
        for chunk in self.indices.chunks(3) {
            if chunk.len() < 3 {
                continue;
            }

            let p0 = Vec3::from_array(self.vertices[chunk[0] as usize].position);
            let p1 = Vec3::from_array(self.vertices[chunk[1] as usize].position);
            let p2 = Vec3::from_array(self.vertices[chunk[2] as usize].position);

            let face_normal = (p1 - p0).cross(p2 - p0).normalize_or_zero();

            for &idx in chunk {
                let pos = self.vertices[idx as usize].position;
                let key = to_key(pos);
                *normal_map.entry(key).or_insert(Vec3::ZERO) += face_normal;
            }
        }

        // Second pass: assign averaged normals
        for v in &mut self.vertices {
            let key = to_key(v.position);
            if let Some(accumulated) = normal_map.get(&key) {
                v.normal = accumulated.normalize_or_zero().to_array();
            }
        }
    }

    /// Sets color for all vertices
    pub fn set_color(&mut self, color: [f32; 4]) {
        for v in &mut self.vertices {
            v.color = color;
        }
    }

    /// Sets color for vertices in a range
    pub fn set_color_range(&mut self, start: usize, count: usize, color: [f32; 4]) {
        for v in self.vertices.iter_mut().skip(start).take(count) {
            v.color = color;
        }
    }

    /// Computes bitangent-sign tangents for all vertices
    pub fn compute_tangents(&mut self) {
        let vertex_count = self.vertices.len();
        let mut tangents = vec![Vec3::ZERO; vertex_count];
        let mut bitangents = vec![Vec3::ZERO; vertex_count];

        for chunk in self.indices.chunks(3) {
            if chunk.len() < 3 {
                continue;
            }
            let i0 = chunk[0] as usize;
            let i1 = chunk[1] as usize;
            let i2 = chunk[2] as usize;

            let v0 = &self.vertices[i0];
            let v1 = &self.vertices[i1];
            let v2 = &self.vertices[i2];

            let p0 = Vec3::from_array(v0.position);
            let p1 = Vec3::from_array(v1.position);
            let p2 = Vec3::from_array(v2.position);

            let uv0 = Vec2::from_array(v0.tex_coords);
            let uv1 = Vec2::from_array(v1.tex_coords);
            let uv2 = Vec2::from_array(v2.tex_coords);

            let delta_pos1 = p1 - p0;
            let delta_pos2 = p2 - p0;
            let delta_uv1 = uv1 - uv0;
            let delta_uv2 = uv2 - uv0;

            let r = 1.0 / (delta_uv1.x * delta_uv2.y - delta_uv1.y * delta_uv2.x);
            let tangent = (delta_pos1 * delta_uv2.y - delta_pos2 * delta_uv1.y) * r;
            let bitangent = (delta_pos2 * delta_uv1.x - delta_pos1 * delta_uv2.x) * r;

            if !tangent.is_finite() || !bitangent.is_finite() {
                continue;
            }

            tangents[i0] += tangent;
            tangents[i1] += tangent;
            tangents[i2] += tangent;

            bitangents[i0] += bitangent;
            bitangents[i1] += bitangent;
            bitangents[i2] += bitangent;
        }

        for i in 0..vertex_count {
            let n = Vec3::from_array(self.vertices[i].normal);
            let t = tangents[i];
            let b = bitangents[i];

            // Gram-Schmidt orthogonalize
            let t_ortho = (t - n * n.dot(t)).normalize_or_zero();

            // Calculate handedness
            let w = if n.cross(t).dot(b) < 0.0 { -1.0 } else { 1.0 };

            self.vertices[i].tangent = [t_ortho.x, t_ortho.y, t_ortho.z, w];
        }
    }
}

impl Default for MeshBuilder {
    fn default() -> Self {
        Self::new()
    }
}
