use std::fs::File;
use std::io::Read;
use std::path::PathBuf;
use std::sync::{Arc, Mutex};
use tokio::io::AsyncReadExt;
use zip::ZipArchive;

#[derive(Clone)]
enum MountSource {
    Physical(PathBuf),
    Zip(Arc<Mutex<ZipArchive<File>>>),
}

#[derive(Clone)]
pub struct Vfs {
    // We use Vec to allow ordered checking (e.g. overrides).
    // But existing code used HashMap. We can stick to HashMap but prefix matching is arbitrary order.
    // Let's switch to Vec for predictable behavior (reverse order of mount).
    mounts: Arc<Mutex<Vec<(String, MountSource)>>>,
}

impl Vfs {
    pub fn new() -> Self {
        Self {
            mounts: Arc::new(Mutex::new(Vec::new())),
        }
    }

    pub fn mount(&self, virtual_path: &str, physical_path: PathBuf) -> Result<(), std::io::Error> {
        let mut mounts = self.mounts.lock().unwrap();

        // Normalize virtual path to end with / if not empty and not root
        let v_path = if virtual_path.ends_with('/') || virtual_path.is_empty() {
            virtual_path.to_string()
        } else {
            format!("{}/", virtual_path)
        };

        if physical_path.is_file() {
            // Try to open as Zip
            let file = File::open(&physical_path)?;
            let archive = ZipArchive::new(file)
                .map_err(|e| std::io::Error::new(std::io::ErrorKind::Other, e))?;
            mounts.push((v_path, MountSource::Zip(Arc::new(Mutex::new(archive)))));
        } else {
            // Directory
            mounts.push((v_path, MountSource::Physical(physical_path)));
        }
        Ok(())
    }

    // Resolve now serves mostly for physical files. For Zip, it might return None or dummy.
    // We'll keep it for backward compat where possible, but read_file is the main API.
    pub fn resolve(&self, path: &str) -> Option<PathBuf> {
        let mounts = self.mounts.lock().unwrap();

        // Iterate in reverse to give priority to later mounts
        for (prefix, source) in mounts.iter().rev() {
            if path.starts_with(prefix) {
                let relative = path.strip_prefix(prefix).unwrap_or("");
                let relative = relative.trim_start_matches('/');

                match source {
                    MountSource::Physical(root) => {
                        return Some(root.join(relative));
                    }
                    MountSource::Zip(_) => {
                        // Cannot resolve Zip entry to a physical PathBuf easily.
                        // We return None to indicate it's not a physical file on disk?
                        // Or we return a virtual path that `read_file` can recognize?
                        // Current `loader.rs` uses `resolve` to check extension.
                        // Extension check can be done on `path` string directly.
                        // `loader.rs` also uses `resolve` to read via `gltf::import`.
                        // We are refactoring `loader.rs` to not rely on this for reading.
                        return None;
                    }
                }
            }
        }

        // Fallback: Check if it exists relative to CWD
        let p = PathBuf::from(path);
        if p.exists() {
            Some(p)
        } else {
            None
        }
    }

    pub async fn read_file(&self, path: &str) -> Result<Vec<u8>, std::io::Error> {
        // We logic similar to resolve but returning data
        // Clone mounts to release lock
        let mounts = {
            let m = self.mounts.lock().unwrap();
            m.clone()
        };

        for (prefix, source) in mounts.iter().rev() {
            if path.starts_with(prefix) {
                let relative = path.strip_prefix(prefix).unwrap_or("");
                let relative = relative.trim_start_matches('/'); // Zip expects no leading slash usually

                match source {
                    MountSource::Physical(root) => {
                        let full_path = root.join(relative);
                        let mut file = tokio::fs::File::open(&full_path).await?;
                        let mut buffer = Vec::new();
                        file.read_to_end(&mut buffer).await?;
                        return Ok(buffer);
                    }
                    MountSource::Zip(archive) => {
                        let archive = archive.clone();
                        let relative = relative.to_string();
                        // Blocking IO
                        return tokio::task::spawn_blocking(move || {
                            let mut zip = archive.lock().unwrap();
                            let mut file = zip.by_name(&relative).map_err(|e| {
                                std::io::Error::new(std::io::ErrorKind::NotFound, e)
                            })?;
                            let mut buffer = Vec::new();
                            file.read_to_end(&mut buffer)?;
                            Ok(buffer)
                        })
                        .await
                        .map_err(|e| std::io::Error::new(std::io::ErrorKind::Other, e))?;
                    }
                }
            }
        }

        // Fallback for direct paths
        let mut file = tokio::fs::File::open(path).await?;
        let mut buffer = Vec::new();
        file.read_to_end(&mut buffer).await?;
        Ok(buffer)
    }

    /// List all files available under a given virtual path prefix.
    /// Returns virtual paths that can be used with `read_file`.
    pub fn list_files(&self, prefix: &str) -> Vec<String> {
        let mounts = self.mounts.lock().unwrap();
        let mut results = std::collections::HashSet::new();

        for (mount_prefix, source) in mounts.iter() {
            // Only list from mounts whose prefix is compatible
            if !prefix.starts_with(mount_prefix) && !mount_prefix.starts_with(prefix) {
                continue;
            }

            match source {
                MountSource::Physical(root) => {
                    // Walk the physical directory
                    let relative_prefix = if prefix.starts_with(mount_prefix) {
                        prefix.strip_prefix(mount_prefix).unwrap_or("")
                    } else {
                        ""
                    };
                    let search_root = root.join(relative_prefix.trim_start_matches('/'));
                    if search_root.exists() {
                        Self::walk_dir_recursive(&search_root, root, mount_prefix, &mut results);
                    }
                }
                MountSource::Zip(archive) => {
                    let mut zip = archive.lock().unwrap();
                    for i in 0..zip.len() {
                        if let Ok(file) = zip.by_index_raw(i) {
                            let name = file.name().to_string();
                            if !name.ends_with('/') {
                                let virtual_path = format!("{}{}", mount_prefix, name);
                                if virtual_path.starts_with(prefix) {
                                    results.insert(virtual_path);
                                }
                            }
                        }
                    }
                }
            }
        }

        let mut sorted: Vec<String> = results.into_iter().collect();
        sorted.sort();
        sorted
    }

    fn walk_dir_recursive(
        dir: &std::path::Path,
        root: &std::path::Path,
        mount_prefix: &str,
        results: &mut std::collections::HashSet<String>,
    ) {
        if let Ok(entries) = std::fs::read_dir(dir) {
            for entry in entries.flatten() {
                let path = entry.path();
                if path.is_dir() {
                    Self::walk_dir_recursive(&path, root, mount_prefix, results);
                } else if path.is_file() {
                    if let Ok(relative) = path.strip_prefix(root) {
                        let rel_str = relative.to_str().unwrap_or("").replace('\\', "/");
                        results.insert(format!("{}{}", mount_prefix, rel_str));
                    }
                }
            }
        }
    }

    /// Check if a file exists in any mount.
    pub fn file_exists(&self, path: &str) -> bool {
        let mounts = self.mounts.lock().unwrap();

        for (prefix, source) in mounts.iter().rev() {
            if path.starts_with(prefix) {
                let relative = path.strip_prefix(prefix).unwrap_or("");
                let relative = relative.trim_start_matches('/');

                match source {
                    MountSource::Physical(root) => {
                        let full_path = root.join(relative);
                        if full_path.exists() {
                            return true;
                        }
                    }
                    MountSource::Zip(archive) => {
                        let zip = archive.lock().unwrap();
                        if zip.index_for_name(relative).is_some() {
                            return true;
                        }
                    }
                }
            }
        }

        // Fallback
        PathBuf::from(path).exists()
    }
}
