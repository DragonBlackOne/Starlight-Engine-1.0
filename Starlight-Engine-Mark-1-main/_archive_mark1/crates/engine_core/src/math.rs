use glam::{Vec3, Vec4};
use numpy::*;
use pyo3::prelude::*;

#[pyclass(subclass)]
#[derive(Clone, Debug)]
pub struct Frustum {
    planes: [Vec4; 6],
}

#[pymethods]
impl Frustum {
    #[new]
    pub fn new(mvp: [f32; 16]) -> Self {
        // mvp is expected to be column-major 4x4 matrix flattened
        // m[col][row] -> m[col * 4 + row]
        // In python: mvp[0][0] is index 0. mvp[3][3] is index 15.
        // The Gribb-Hartmann extraction expects M * v convention (OpenGL).

        let m = mvp;
        let mut planes = [Vec4::ZERO; 6];

        // Left:   row4 + row1
        planes[0] = Vec4::new(m[3] + m[0], m[7] + m[4], m[11] + m[8], m[15] + m[12]);
        // Right:  row4 - row1
        planes[1] = Vec4::new(m[3] - m[0], m[7] - m[4], m[11] - m[8], m[15] - m[12]);
        // Bottom: row4 + row2
        planes[2] = Vec4::new(m[3] + m[1], m[7] + m[5], m[11] + m[9], m[15] + m[13]);
        // Top:    row4 - row2
        planes[3] = Vec4::new(m[3] - m[1], m[7] - m[5], m[11] - m[9], m[15] - m[13]);
        // Near:   row4 + row3
        planes[4] = Vec4::new(m[3] + m[2], m[7] + m[6], m[11] + m[10], m[15] + m[14]);
        // Far:    row4 - row3
        planes[5] = Vec4::new(m[3] - m[2], m[7] - m[6], m[11] - m[10], m[15] - m[14]);

        // Normalize
        for i in 0..6 {
            let len = Vec3::new(planes[i].x, planes[i].y, planes[i].z).length();
            if len > 0.0 {
                planes[i] /= len;
            }
        }

        Frustum { planes }
    }

    pub fn is_sphere_visible(&self, center: [f32; 3], radius: f32) -> bool {
        let c = Vec3::from(center);
        for plane in &self.planes {
            let p_norm = Vec3::new(plane.x, plane.y, plane.z);
            if p_norm.dot(c) + plane.w < -radius {
                return false;
            }
        }
        true
    }

    pub fn is_point_visible(&self, point: [f32; 3]) -> bool {
        let p = Vec3::from(point);
        for plane in &self.planes {
            let p_norm = Vec3::new(plane.x, plane.y, plane.z);
            if p_norm.dot(p) + plane.w < 0.0 {
                return false;
            }
        }
        true
    }

    /// Batch visibility test for multiple spheres.
    /// Input: numpy array of shape (N, 4) where each row is [x, y, z, radius].
    /// Output: numpy array of shape (N,) with boolean visibility results.
    pub fn is_spheres_visible_batch<'py>(
        &self,
        py: Python<'py>,
        spheres: PyReadonlyArray2<'py, f32>,
    ) -> PyResult<Bound<'py, numpy::PyArray1<bool>>> {
        use rayon::prelude::*;

        let arr = spheres.as_array();
        let n = arr.nrows();

        // Pre-extract plane data for cache efficiency
        let planes_data: [(Vec3, f32); 6] = std::array::from_fn(|i| {
            (
                Vec3::new(self.planes[i].x, self.planes[i].y, self.planes[i].z),
                self.planes[i].w,
            )
        });

        // Parallel visibility check
        let results: Vec<bool> = (0..n)
            .into_par_iter()
            .map(|i| {
                let row = arr.row(i);
                let center = Vec3::new(row[0], row[1], row[2]);
                let radius = row[3];

                for (p_norm, w) in &planes_data {
                    if p_norm.dot(center) + w < -radius {
                        return false;
                    }
                }
                true
            })
            .collect();

        Ok(numpy::PyArray1::from_vec(py, results))
    }
}
