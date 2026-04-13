#[cfg(test)]
mod tests {
    use crate::renderer::state::RenderState;
    use std::time::Instant;

    #[tokio::test]
    async fn benchmark_render_loop() {
        let width = 800;
        let height = 600;

        let mut state = RenderState::new(None, width, height).await;

        let instance_count = 2000;
        let material_count = 100;
        println!(
            "Adding {} instances with {} unique materials...",
            instance_count, material_count
        );

        for i in 0..instance_count {
            let x = (i % 100) as f32;
            let z = (i / 100) as f32;
            let model = [
                [1.0, 0.0, 0.0, 0.0],
                [0.0, 1.0, 0.0, 0.0],
                [0.0, 0.0, 1.0, 0.0],
                [x, 0.0, z, 1.0],
            ];

            let mat_idx = i % material_count;
            let diff_name = format!("diff_{}", mat_idx);
            let norm_name = format!("norm_{}", mat_idx);

            state.add_instance(
                "cube",
                &diff_name,
                &norm_name,
                model,
                [1.0, 1.0, 1.0, 1.0],
                0.0,
                0.5,
                None,
            );
        }

        // Warmup
        state.render();

        let start = Instant::now();
        let frames = 50;
        for _ in 0..frames {
            state.render();
        }
        let duration = start.elapsed();
        let avg = duration / frames as u32;
        println!(
            "BENCHMARK_RESULT: {:?} per frame for {} instances ({} mats)",
            avg, instance_count, material_count
        );
    }
}
