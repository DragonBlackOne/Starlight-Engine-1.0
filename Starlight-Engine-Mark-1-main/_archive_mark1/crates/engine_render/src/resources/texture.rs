use image::GenericImageView;

pub struct Texture {
    pub texture: wgpu::Texture,
    pub view: wgpu::TextureView,
    pub sampler: wgpu::Sampler,
}

impl Texture {
    pub fn from_image(
        device: &wgpu::Device,
        queue: &wgpu::Queue,
        img: &image::DynamicImage,
        label: Option<&str>,
        is_normal_map: bool,
    ) -> Result<Self, image::ImageError> {
        let dimensions = img.dimensions();
        let rgba = img.to_rgba8();

        let size = wgpu::Extent3d {
            width: dimensions.0,
            height: dimensions.1,
            depth_or_array_layers: 1,
        };

        let texture = device.create_texture(&wgpu::TextureDescriptor {
            label,
            size,
            mip_level_count: 1,
            sample_count: 1,
            dimension: wgpu::TextureDimension::D2,
            format: if is_normal_map {
                wgpu::TextureFormat::Rgba8Unorm
            } else {
                wgpu::TextureFormat::Rgba8UnormSrgb
            },
            usage: wgpu::TextureUsages::TEXTURE_BINDING | wgpu::TextureUsages::COPY_DST,
            view_formats: &[],
        });

        queue.write_texture(
            wgpu::ImageCopyTexture {
                aspect: wgpu::TextureAspect::All,
                texture: &texture,
                mip_level: 0,
                origin: wgpu::Origin3d::ZERO,
            },
            &rgba,
            wgpu::ImageDataLayout {
                offset: 0,
                bytes_per_row: Some(4 * dimensions.0),
                rows_per_image: Some(dimensions.1),
            },
            size,
        );

        let view = texture.create_view(&wgpu::TextureViewDescriptor::default());
        let sampler = device.create_sampler(&wgpu::SamplerDescriptor {
            address_mode_u: wgpu::AddressMode::Repeat,
            address_mode_v: wgpu::AddressMode::Repeat,
            address_mode_w: wgpu::AddressMode::Repeat,
            mag_filter: wgpu::FilterMode::Linear,
            min_filter: wgpu::FilterMode::Linear,
            mipmap_filter: wgpu::FilterMode::Linear,
            ..Default::default()
        });

        Ok(Self {
            texture,
            view,
            sampler,
        })
    }

    pub fn from_dds(
        device: &wgpu::Device,
        queue: &wgpu::Queue,
        dds: &ddsfile::Dds,
        label: Option<&str>,
        is_normal_map: bool,
    ) -> Result<Self, anyhow::Error> {
        let width = dds.get_width();
        let height = dds.get_height();
        let mip_level_count = dds.get_num_mipmap_levels();

        let format = match dds.get_dxgi_format() {
            Some(ddsfile::DxgiFormat::BC1_UNorm) => {
                if is_normal_map {
                    wgpu::TextureFormat::Bc1RgbaUnorm
                } else {
                    wgpu::TextureFormat::Bc1RgbaUnormSrgb
                }
            }
            Some(ddsfile::DxgiFormat::BC3_UNorm) => {
                if is_normal_map {
                    wgpu::TextureFormat::Bc3RgbaUnorm
                } else {
                    wgpu::TextureFormat::Bc3RgbaUnormSrgb
                }
            }
            Some(ddsfile::DxgiFormat::BC7_UNorm) => {
                if is_normal_map {
                    wgpu::TextureFormat::Bc7RgbaUnorm
                } else {
                    wgpu::TextureFormat::Bc7RgbaUnormSrgb
                }
            }
            Some(fmt) => return Err(anyhow::anyhow!("Unsupported DDS DXGI format: {:?}", fmt)),
            None => return Err(anyhow::anyhow!("DDS must have DXGI format (DX10+ header)")),
        };

        let size = wgpu::Extent3d {
            width,
            height,
            depth_or_array_layers: 1,
        };

        let texture = device.create_texture(&wgpu::TextureDescriptor {
            label,
            size,
            mip_level_count,
            sample_count: 1,
            dimension: wgpu::TextureDimension::D2,
            format,
            usage: wgpu::TextureUsages::TEXTURE_BINDING | wgpu::TextureUsages::COPY_DST,
            view_formats: &[],
        });

        // WGPU BC formats have a 4x4 block size
        // Block size in bytes: BC1 = 8, BC3/BC7 = 16
        let block_size = match format {
            wgpu::TextureFormat::Bc1RgbaUnorm | wgpu::TextureFormat::Bc1RgbaUnormSrgb => 8,
            _ => 16,
        };

        let mut offset = 0;
        let data = dds
            .get_data(0)
            .map_err(|e| anyhow::anyhow!("Failed to read DDS data: {}", e))?;

        for mip in 0..mip_level_count {
            let mip_width = std::cmp::max(1, width >> mip);
            let mip_height = std::cmp::max(1, height >> mip);

            // Number of blocks
            let blocks_x = (mip_width + 3) / 4;
            let blocks_y = (mip_height + 3) / 4;
            let bytes_per_row = blocks_x * block_size;
            let mip_bytes = bytes_per_row * blocks_y;

            let mip_data = &data[offset as usize..(offset + mip_bytes) as usize];

            queue.write_texture(
                wgpu::ImageCopyTexture {
                    aspect: wgpu::TextureAspect::All,
                    texture: &texture,
                    mip_level: mip,
                    origin: wgpu::Origin3d::ZERO,
                },
                mip_data,
                wgpu::ImageDataLayout {
                    offset: 0,
                    bytes_per_row: Some(bytes_per_row),
                    rows_per_image: Some(blocks_y),
                },
                wgpu::Extent3d {
                    width: mip_width,
                    height: mip_height,
                    depth_or_array_layers: 1,
                },
            );

            offset += mip_bytes;
        }

        let view = texture.create_view(&wgpu::TextureViewDescriptor::default());
        let sampler = device.create_sampler(&wgpu::SamplerDescriptor {
            address_mode_u: wgpu::AddressMode::Repeat,
            address_mode_v: wgpu::AddressMode::Repeat,
            address_mode_w: wgpu::AddressMode::Repeat,
            mag_filter: wgpu::FilterMode::Linear,
            min_filter: wgpu::FilterMode::Linear,
            mipmap_filter: wgpu::FilterMode::Linear,
            ..Default::default()
        });

        Ok(Self {
            texture,
            view,
            sampler,
        })
    }
}
