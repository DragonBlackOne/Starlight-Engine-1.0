use bevy_ecs::prelude::*;
use gilrs::{Axis, Button};
use std::collections::{HashMap, HashSet};
use std::sync::Arc;

// Input Types
#[derive(Debug, Clone, PartialEq)]
pub enum InputSource {
    Key(String),
    GamepadButton(usize, Button), // Gamepad ID (index), Button
    GamepadAxis(usize, Axis),     // Gamepad ID, Axis
}

#[derive(Debug, Clone)]
pub struct AxisBinding {
    pub source: InputSource,
    pub scale: f32,
    pub deadzone: f32,
}

// Input State Resource
#[derive(Resource)]
pub struct InputState {
    pub pressed_keys: HashSet<String>,
    pub just_pressed: HashSet<String>,
    pub just_released: HashSet<String>,
    pub mouse_delta: (f32, f32),

    // Raw Gamepad State
    pub gamepad_buttons: HashSet<(usize, Button)>, // (id, button)
    pub gamepad_axes: HashMap<(usize, Axis), f32>, // (id, axis) -> value
    pub connected_gamepads: HashSet<usize>,

    // Mappings
    pub action_bindings: HashMap<Arc<str>, Vec<String>>, // Legacy simple string bindings
    pub axis_bindings: HashMap<Arc<str>, Vec<AxisBinding>>, // New Axis Bindings

    // Processed State
    pub active_actions: HashSet<Arc<str>>,
    pub axis_values: HashMap<Arc<str>, f32>,
}

impl Default for InputState {
    fn default() -> Self {
        Self {
            pressed_keys: HashSet::new(),
            just_pressed: HashSet::new(),
            just_released: HashSet::new(),
            mouse_delta: (0.0, 0.0),
            gamepad_buttons: HashSet::new(),
            gamepad_axes: HashMap::new(),
            connected_gamepads: HashSet::new(),
            action_bindings: HashMap::new(),
            axis_bindings: HashMap::new(),
            active_actions: HashSet::new(),
            axis_values: HashMap::new(),
        }
    }
}

pub fn input_reset_system(mut input: ResMut<InputState>) {
    input.just_pressed.clear();
    input.just_released.clear();
    input.mouse_delta = (0.0, 0.0);
}

pub fn action_update_system(mut input: ResMut<InputState>) {
    // 1. Process Boolean Actions (Optimized: No Clone)
    let mut new_active_actions = HashSet::new();

    for (action_name, keys) in &input.action_bindings {
        for key in keys {
            if input.pressed_keys.contains(key) {
                new_active_actions.insert(action_name.clone());
                break;
            }
        }
    }
    input.active_actions = new_active_actions;

    // 2. Process Axis Bindings (Optimized: No Clone)
    let mut new_axis_values = HashMap::new();

    for (action_name, bindings) in &input.axis_bindings {
        let mut total_value = 0.0;

        for binding in bindings {
            let raw_value = match &binding.source {
                InputSource::Key(k) => {
                    if input.pressed_keys.contains(k) {
                        1.0
                    } else {
                        0.0
                    }
                }
                InputSource::GamepadButton(id, btn) => {
                    if input.gamepad_buttons.contains(&(*id, *btn)) {
                        1.0
                    } else {
                        0.0
                    }
                }
                InputSource::GamepadAxis(id, axis) => {
                    *input.gamepad_axes.get(&(*id, *axis)).unwrap_or(&0.0)
                }
            };

            let val_abs = raw_value.abs();
            let processed_value = if val_abs < binding.deadzone {
                0.0
            } else {
                raw_value
            };

            total_value += processed_value * binding.scale;
        }
        new_axis_values.insert(action_name.clone(), total_value);
    }

    input.axis_values = new_axis_values;
}
