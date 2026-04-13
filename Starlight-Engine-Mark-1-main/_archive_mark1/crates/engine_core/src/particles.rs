use glam::Vec3;
use numpy::*;
use pyo3::prelude::*;
use rayon::prelude::*;

struct XorShift32 {
    state: u32,
}

impl XorShift32 {
    fn new(seed: u32) -> Self {
        Self {
            state: if seed == 0 { 123456789 } else { seed },
        }
    }

    fn next_f32(&mut self) -> f32 {
        let mut x = self.state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        self.state = x;
        (x as f32) / (u32::MAX as f32)
    }

    // Range -1.0 to 1.0
    fn next_f32_signed(&mut self) -> f32 {
        self.next_f32() * 2.0 - 1.0
    }
}

#[pyclass]
pub struct ParticleSystem {
    // GPU Data: [x, y, z, size, r, g, b, a] * max_particles
    gpu_data: Vec<f32>,

    // Sim Data
    velocities: Vec<Vec3>,
    life: Vec<f32>,
    max_life: Vec<f32>,

    max_particles: usize,
    head: usize,
    filled_count: usize,
    rng: XorShift32,
}

#[pymethods]
impl ParticleSystem {
    #[new]
    pub fn new(max_particles: usize) -> Self {
        ParticleSystem {
            gpu_data: vec![0.0; max_particles * 8],
            velocities: vec![Vec3::ZERO; max_particles],
            life: vec![0.0; max_particles],
            max_life: vec![1.0; max_particles],
            max_particles,
            head: 0,
            filled_count: 0,
            rng: XorShift32::new(1992),
        }
    }

    #[pyo3(signature = (x, y, z, count, r, g, b, a, speed, spread, life_base, size_base, life_variance=None, size_variance=None))]
    #[allow(clippy::too_many_arguments)]
    pub fn emit(
        &mut self,
        x: f32,
        y: f32,
        z: f32,
        count: usize,
        r: f32,
        g: f32,
        b: f32,
        a: f32,
        speed: f32,
        spread: f32,
        life_base: f32,
        size_base: f32,
        life_variance: Option<f32>,
        size_variance: Option<f32>,
    ) {
        let count = count.min(self.max_particles);
        let mut spawn_idx = self.head;
        let l_var = life_variance.unwrap_or(0.2);
        let s_var = size_variance.unwrap_or(0.2);

        for _ in 0..count {
            // Sim Data
            let life = life_base * (1.0 + self.rng.next_f32_signed() * l_var);
            let size = size_base * (1.0 + self.rng.next_f32_signed() * s_var);

            self.life[spawn_idx] = life;
            self.max_life[spawn_idx] = life;

            // Random Direction Cube
            let vx = self.rng.next_f32_signed();
            let vy = self.rng.next_f32_signed();
            let vz = self.rng.next_f32_signed();
            // Normalize locally to avoid glam method dependency confusion if needed,
            // but Vec3::normalize_or_zero should exist in glam 0.24+.
            let dir = Vec3::new(vx, vy, vz).normalize_or_zero();
            self.velocities[spawn_idx] = dir * speed * spread;

            // GPU Data
            let base = spawn_idx * 8;
            self.gpu_data[base] = x;
            self.gpu_data[base + 1] = y;
            self.gpu_data[base + 2] = z;
            self.gpu_data[base + 3] = size;
            self.gpu_data[base + 4] = r;
            self.gpu_data[base + 5] = g;
            self.gpu_data[base + 6] = b;
            self.gpu_data[base + 7] = a;

            spawn_idx = (spawn_idx + 1) % self.max_particles;
        }

        self.head = spawn_idx;
        self.filled_count = (self.filled_count + count).min(self.max_particles);
    }

    pub fn update(&mut self, dt: f32, gx: f32, gy: f32) {
        if self.filled_count == 0 {
            return;
        }

        // Rayon parallel iteration over chunks
        self.gpu_data
            .par_chunks_exact_mut(8)
            .zip(self.velocities.par_iter_mut())
            .zip(self.life.par_iter_mut())
            .zip(self.max_life.par_iter())
            .for_each(|(((gpu, vel), life), max_life)| {
                if *life > 0.0 {
                    // Physics
                    *life -= dt;

                    // Apply Gravity
                    vel.x += gx * dt;
                    vel.y += gy * dt;

                    // Pos += Vel * dt
                    gpu[0] += vel.x * dt;
                    gpu[1] += vel.y * dt;
                    gpu[2] += vel.z * dt;

                    // Alpha calc
                    let ratio = (*life / *max_life).clamp(0.0, 1.0);
                    gpu[7] = ratio;
                } else {
                    gpu[7] = 0.0;
                }
            });
    }

    pub fn get_buffer_raw(&self) -> &[f32] {
        &self.gpu_data
    }

    pub fn get_buffer<'py>(&self, py: Python<'py>) -> PyResult<Bound<'py, PyArray1<f32>>> {
        // Safe conversion using Bound API
        let arr = PyArray1::from_slice(py, &self.gpu_data);
        Ok(arr)
    }

    pub fn get_life<'py>(&self, py: Python<'py>) -> PyResult<Bound<'py, PyArray1<f32>>> {
        let arr = PyArray1::from_slice(py, &self.life);
        Ok(arr)
    }

    pub fn get_max_life<'py>(&self, py: Python<'py>) -> PyResult<Bound<'py, PyArray1<f32>>> {
        let arr = PyArray1::from_slice(py, &self.max_life);
        Ok(arr)
    }

    pub fn get_count(&self) -> usize {
        self.max_particles
    }

    pub fn get_active_count(&self) -> usize {
        self.filled_count
    }

    pub fn get_head(&self) -> usize {
        self.head
    }
}
