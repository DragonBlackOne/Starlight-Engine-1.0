"""
Tropical Storm Demo - Demonstração de chuva e tempestade.
Usa o pacote starlight instalado (não pysrc local).
"""
import math
import os
import random

# Usar pacote instalado
import starlight.backend as backend

# Estado global
initialized = False
storm_t = 0.0
entities = []

def on_update(dt, world, input_state):
    global initialized, storm_t, entities

    if not initialized:
        print("[DEMO] Inicializando Tropical Storm...")
        
        # Chão
        floor_id = backend.spawn_floor(0.0, 0.0, 0.0)
        entities.append(floor_id)
        
        # Algumas árvores (cubos coloridos por enquanto)
        FOREST_SIZE = 30.0
        for i in range(20):
            x = (random.random() * FOREST_SIZE) - (FOREST_SIZE / 2.0)
            z = (random.random() * FOREST_SIZE) - (FOREST_SIZE / 2.0)
            tree_id = backend.spawn_static_box(x, 2.0, z, 0.5, 4.0, 0.5, 0.2, 0.6, 0.1)
            entities.append(tree_id)
        
        # Configurar neblina densa
        backend.set_fog(0.04, 0.2, 0.3, 0.4)
        
        # Direção do sol (nublado)
        backend.set_sun_direction(0.2, 0.8, 0.2)
        
        initialized = True
        print("[DEMO] Storm inicializada!")

    storm_t += dt
    
    # Simular gotas de chuva (cubos dinâmicos caindo)
    if random.random() < 0.3:  # 30% chance por frame
        off_x = (random.random() - 0.5) * 40.0
        off_z = (random.random() - 0.5) * 40.0
        # Spawn cubo pequeno azul como gota
        drop_id = backend.spawn_dynamic_cube(off_x, 15.0, off_z, 0.5, 0.5, 0.8)

    # Câmera cinematográfica
    cam_x = math.sin(storm_t * 0.1) * 15.0
    cam_z = math.cos(storm_t * 0.1) * 15.0
    cam_y = 3.0
    
    # Atualizar câmera
    camera_id = backend.get_main_camera_id()
    if camera_id:
        backend.set_transform(camera_id, cam_x, cam_y, cam_z)
        yaw = math.atan2(-cam_x, -cam_z)
        backend.set_rotation(camera_id, 0.0, yaw, 0.0)

def on_fixed_update(dt, world, input_state):
    pass

def on_render():
    pass

if __name__ == "__main__":
    print("Iniciando Tropical Storm Demo...")
    backend.run_engine(
        "Tropical Storm",
        1280,
        720,
        os.path.abspath(__file__),
        on_update,
        on_fixed_update,
        on_render
    )
