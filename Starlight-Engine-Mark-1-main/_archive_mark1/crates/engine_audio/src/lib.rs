use kira::{
    manager::{backend::DefaultBackend, AudioManager, AudioManagerSettings},
    sound::{
        static_sound::{StaticSoundData, StaticSoundHandle, StaticSoundSettings},
        Region,
    },
    tween::Tween,
};
use std::collections::HashMap;
use std::sync::{Arc, Mutex};

pub struct AudioState {
    manager: Arc<Mutex<Option<AudioManager<DefaultBackend>>>>,
    handles: Arc<Mutex<HashMap<u64, StaticSoundHandle>>>,
    next_id: Arc<Mutex<u64>>,
}

impl AudioState {
    pub fn new() -> Self {
        let manager = match AudioManager::<DefaultBackend>::new(AudioManagerSettings::default()) {
            Ok(m) => Some(m),
            Err(e) => {
                eprintln!(
                    "Audio System Warning: Failed to initialize Kira audio: {}",
                    e
                );
                None
            }
        };

        Self {
            manager: Arc::new(Mutex::new(manager)),
            handles: Arc::new(Mutex::new(HashMap::new())),
            next_id: Arc::new(Mutex::new(0)),
        }
    }

    pub fn play_sound(&self, path: &str, looped: bool, volume: f64, panning: f64) -> u64 {
        let settings = StaticSoundSettings::new()
            .loop_region(if looped { Some(Region::from(..)) } else { None })
            .volume(volume)
            .panning(panning);

        if let Ok(sound_data) = StaticSoundData::from_file(path, settings) {
            let mut val_manager = self.manager.lock().unwrap();
            if let Some(manager) = val_manager.as_mut() {
                if let Ok(handle) = manager.play(sound_data) {
                    let mut id_guard = self.next_id.lock().unwrap();
                    let id = *id_guard;
                    *id_guard += 1;

                    self.handles.lock().unwrap().insert(id, handle);
                    return id;
                }
            }
        } else {
            eprintln!("Failed to load sound file '{}'", path);
        }
        u64::MAX // Error code
    }

    pub fn set_volume(&self, id: u64, volume: f64) {
        if let Some(handle) = self.handles.lock().unwrap().get_mut(&id) {
            let _ = handle.set_volume(volume, Tween::default());
        }
    }

    pub fn set_panning(&self, id: u64, panning: f64) {
        if let Some(handle) = self.handles.lock().unwrap().get_mut(&id) {
            let _ = handle.set_panning(panning, Tween::default());
        }
    }

    pub fn stop(&self, id: u64) {
        let mut handles = self.handles.lock().unwrap();
        if let Some(mut handle) = handles.remove(&id) {
            let _ = handle.stop(Tween::default());
        }
    }

    pub fn clean_finished(&self) {
        // Optional: Remove handles that are finished playing to prevent leak
        // Kira handles report status.
        // For now, manual cleaning or just let them live until stop is called (if looped).
        // One-shot sounds will leak handles in this map. Ideally we check status.
    }
}
