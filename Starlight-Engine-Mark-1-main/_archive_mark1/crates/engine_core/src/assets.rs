use std::collections::HashMap;
use uuid::Uuid;
use bevy_ecs::prelude::Resource;
use glam::{Vec3, Quat};

#[derive(Clone, Debug)]
pub enum AssetType {
    Mesh,
    Texture,
    Skybox,
    Sound,
    Scene,
}

#[derive(Clone, Debug, PartialEq)]
pub enum LoadState {
    Unloaded,
    Loading,
    Loaded,
    Failed,
}

#[derive(Clone, Debug)]
pub struct AssetInfo {
    pub path: String,
    pub asset_type: AssetType,
    pub state: LoadState,
}

pub struct AssetRegistry {
    // Maps File Path -> UUID String
    path_to_uuid: HashMap<String, String>,
    // Maps UUID String -> Asset Info
    uuid_to_info: HashMap<String, AssetInfo>,
}

impl AssetRegistry {
    pub fn new() -> Self {
        let mut registry = Self {
            path_to_uuid: HashMap::new(),
            uuid_to_info: HashMap::new(),
        };

        // Register Default Assets
        // These keys are what the RenderState expects for defaults.
        // We map them to themselves as "paths" or special reserved IDs.
        // For compatibility, we ensure "cube", "default", "flat_normal" exist.

        registry.register_reserved("cube", AssetType::Mesh);
        registry.register_reserved("default", AssetType::Texture);
        registry.register_reserved("flat_normal", AssetType::Texture);

        registry
    }

    fn register_reserved(&mut self, id: &str, asset_type: AssetType) {
        self.path_to_uuid.insert(id.to_string(), id.to_string());
        self.uuid_to_info.insert(id.to_string(), AssetInfo {
            path: "internal".to_string(),
            asset_type,
            state: LoadState::Loaded,
        });
    }

    pub fn get_or_create_id(&mut self, path: &str, asset_type: AssetType) -> String {
        if let Some(id) = self.path_to_uuid.get(path) {
            return id.clone();
        }

        let id = Uuid::new_v4().to_string();
        self.path_to_uuid.insert(path.to_string(), id.clone());
        self.uuid_to_info.insert(id.clone(), AssetInfo {
            path: path.to_string(),
            asset_type,
            state: LoadState::Unloaded,
        });

        id
    }

    pub fn mark_loading(&mut self, id: &str) {
        if let Some(info) = self.uuid_to_info.get_mut(id) {
            info.state = LoadState::Loading;
        }
    }

    pub fn mark_loaded(&mut self, id: &str) {
        if let Some(info) = self.uuid_to_info.get_mut(id) {
            info.state = LoadState::Loaded;
        }
    }

    pub fn mark_failed(&mut self, id: &str) {
        if let Some(info) = self.uuid_to_info.get_mut(id) {
            info.state = LoadState::Failed;
        }
    }

    pub fn get_state(&self, id: &str) -> LoadState {
        self.uuid_to_info.get(id).map(|i| i.state.clone()).unwrap_or(LoadState::Failed)
    }

    pub fn get_path(&self, id: &str) -> Option<&String> {
        self.uuid_to_info.get(id).map(|info| &info.path)
    }
}

// Scene Graph Data Structures
#[derive(Clone, Debug)]
pub struct SceneNode {
    pub name: String,
    pub translation: Vec3,
    pub rotation: Quat,
    pub scale: Vec3,
    pub mesh_id: Option<String>,
    pub children: Vec<usize>,
}

#[derive(Clone, Debug)]
pub struct SceneGraph {
    pub nodes: Vec<SceneNode>,
    pub root_indices: Vec<usize>,
}

#[derive(Resource, Default)]
pub struct SceneRegistry {
    pub graphs: HashMap<String, SceneGraph>,
}
