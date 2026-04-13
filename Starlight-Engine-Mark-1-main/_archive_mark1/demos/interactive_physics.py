"""
Starlight Engine - Interactive Physics Showcase
Demonstração interativa com:
- Física de corpos rígidos
- Áudio espacial
- Controles de câmera
- Spawning dinâmico

Controles:
- WASD: Mover câmera
- Mouse: Olhar
- SPACE: Spawn cubo dinâmico
- F: Aplicar força no cubo selecionado
- R: Reset cena
"""
import math
import os
import random

import starlight.backend as backend
from starlight.components import RigidBody

# Estado global
initialized = False
time_elapsed = 0.0
camera_yaw = 0.0
camera_pitch = 0.0
camera_pos = [0.0, 5.0, 15.0]
dynamic_cubes = []
selected_cube = None

def on_update(dt, world, input_state):
    global initialized, time_elapsed, camera_yaw, camera_pitch, camera_pos
    global dynamic_cubes, selected_cube

    if not initialized:
        print("=" * 50)
        print("  STARLIGHT ENGINE - INTERACTIVE PHYSICS DEMO")
        print("=" * 50)
        print("Controles:")
        print("  WASD   - Mover câmera")
        print("  Mouse  - Olhar")
        print("  SPACE  - Spawn cubo dinâmico")
        print("  F      - Aplicar impulso no último cubo")
        print("  R      - Reset cena")
        print("=" * 50)
        
        # Chão grande
        backend.spawn_floor(0.0, 0.0, 0.0)
        
        # Paredes (cubos estáticos)
        backend.spawn_static_box(-20.0, 2.0, 0.0, 1.0, 4.0, 20.0, 0.5, 0.5, 0.5)  # Esquerda
        backend.spawn_static_box(20.0, 2.0, 0.0, 1.0, 4.0, 20.0, 0.5, 0.5, 0.5)   # Direita
        backend.spawn_static_box(0.0, 2.0, -20.0, 20.0, 4.0, 1.0, 0.5, 0.5, 0.5)  # Fundo
        
        # Rampa
        backend.spawn_static_box(0.0, 1.0, 10.0, 5.0, 0.3, 5.0, 0.3, 0.6, 0.3)
        
        # Alguns cubos iniciais
        for i in range(5):
            x = (random.random() - 0.5) * 10.0
            cube_id = backend.spawn_dynamic_cube(x, 5.0 + i * 2.0, 0.0, 
                                                  random.random(), 
                                                  random.random(), 
                                                  random.random())
            dynamic_cubes.append(cube_id)
        
        # Neblina leve
        backend.set_fog(0.01, 0.7, 0.8, 0.9)
        
        # Sol
        backend.set_sun_direction(0.5, 0.8, 0.3)
        
        initialized = True
        print(f"[DEMO] Cena inicializada com {len(dynamic_cubes)} cubos dinâmicos!")

    time_elapsed += dt
    
    # ===== INPUT DE CÂMERA =====
    # Mouse look
    mouse_dx, mouse_dy = backend.get_mouse_delta()
    sensitivity = 0.002
    camera_yaw += mouse_dx * sensitivity
    camera_pitch -= mouse_dy * sensitivity
    camera_pitch = max(-1.4, min(1.4, camera_pitch))  # Clamp pitch
    
    # WASD movement
    speed = 10.0 * dt
    forward_x = math.cos(camera_yaw) * math.cos(camera_pitch)
    forward_z = math.sin(camera_yaw) * math.cos(camera_pitch)
    right_x = math.sin(camera_yaw)
    right_z = -math.cos(camera_yaw)
    
    if backend.is_key_down("W"):
        camera_pos[0] += forward_x * speed
        camera_pos[2] += forward_z * speed
    if backend.is_key_down("S"):
        camera_pos[0] -= forward_x * speed
        camera_pos[2] -= forward_z * speed
    if backend.is_key_down("A"):
        camera_pos[0] -= right_x * speed
        camera_pos[2] -= right_z * speed
    if backend.is_key_down("D"):
        camera_pos[0] += right_x * speed
        camera_pos[2] += right_z * speed
    
    # Atualizar câmera
    camera_id = backend.get_main_camera_id()
    if camera_id:
        backend.set_transform(camera_id, camera_pos[0], camera_pos[1], camera_pos[2])
        backend.set_rotation(camera_id, camera_pitch, camera_yaw, 0.0)
    
    # ===== SPAWN CUBO =====
    if backend.is_key_just_pressed("Space"):
        # Spawn na frente da câmera
        spawn_x = camera_pos[0] + forward_x * 3.0
        spawn_y = camera_pos[1]
        spawn_z = camera_pos[2] + forward_z * 3.0
        
        cube_id = backend.spawn_dynamic_cube(
            spawn_x, spawn_y, spawn_z,
            random.random(), random.random(), random.random()
        )
        dynamic_cubes.append(cube_id)
        selected_cube = cube_id
        print(f"[SPAWN] Cubo {cube_id} criado! Total: {len(dynamic_cubes)}")
    
    # ===== APLICAR FORÇA =====
    if backend.is_key_just_pressed("F") and selected_cube:
        rb = RigidBody(selected_cube)
        # Impulso para cima e na direção da câmera
        rb.apply_impulse(forward_x * 50.0, 100.0, forward_z * 50.0)
        print(f"[IMPULSE] Aplicado ao cubo {selected_cube}!")
    
    # ===== RESET =====
    if backend.is_key_just_pressed("R"):
        print("[RESET] Reiniciando cena...")
        dynamic_cubes.clear()
        selected_cube = None
        camera_pos = [0.0, 5.0, 15.0]
        camera_yaw = 0.0
        camera_pitch = 0.0

def on_fixed_update(dt, world, input_state):
    # Física é automática
    pass

def on_render():
    pass

if __name__ == "__main__":
    print("Iniciando Interactive Physics Demo...")
    backend.set_cursor_grab(True)
    backend.set_cursor_visible(False)
    backend.run_engine(
        "Starlight - Interactive Physics",
        1280,
        720,
        os.path.abspath(__file__),
        on_update,
        on_fixed_update,
        on_render
    )
