use noise::{Fbm, MultiFractal, NoiseFn, Perlin};

pub fn generate_heightmap(
    width: usize,
    height: usize,
    seed: u32,
    scale: f64,
    octaves: usize,
    persistence: f64,
    lacunarity: f64,
) -> Vec<f32> {
    // Configure FBM noise
    let fbm = Fbm::<Perlin>::new(seed)
        .set_octaves(octaves)
        .set_persistence(persistence)
        .set_lacunarity(lacunarity);

    let mut map = Vec::with_capacity(width * height);

    for y in 0..height {
        for x in 0..width {
            // Coordinate scaling
            let nx = (x as f64) / scale;
            let ny = (y as f64) / scale;

            // Get noise value (-1.0 to 1.0)
            let value = fbm.get([nx, ny]);

            // Normalize to 0.0 - 1.0 roughly (FBM can go outside range, clamp it)
            // Or just return raw value. We usually want 0..1 for heightmap texture
            // But for mesh displacement, raw is fine. Let's normalize to 0..1

            // Fbm range depends on octaves/persistence.
            // A simple remap Assuming range [-1, 1] -> [0, 1]
            let normalized = (value + 1.0) * 0.5;
            map.push(normalized as f32);
        }
    }

    map
}
