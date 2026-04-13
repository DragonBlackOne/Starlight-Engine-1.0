
import os
import math
from .. import backend
from ..framework import Entity, Scene

try:
    from ..backend import generate_heightmap
except ImportError:
    print("Warning: generate_heightmap not found in backend")
    def generate_heightmap(w, h, seed, scale, octaves, persistence, lacunarity):
        return [0.0] * (w * h)

class TerrainObject:
    def __init__(self, entity: Entity, heights: list[float], size: int, scale: float, x_offset: float, z_offset: float, height_multiplier: float):
        self.entity = entity
        self.heights = heights
        self.size = size
        self.scale = scale
        self.x_offset = x_offset
        self.z_offset = z_offset
        self.height_multiplier = height_multiplier

    def set_color(self, r: float, g: float, b: float, a: float = 1.0):
        self.entity.set_color(r, g, b, a)
        return self

    def get_height(self, world_x: float, world_z: float) -> float:
        # Transform world to local chunk coords
        local_x = world_x - self.x_offset
        local_z = world_z - self.z_offset
        
        # Check bounds
        if local_x < 0 or local_x > self.scale or local_z < 0 or local_z > self.scale:
            return 0.0 # Out of bounds
            
        # Map to grid coordinates
        step = self.scale / (self.size - 1)
        
        gx = local_x / step
        gz = local_z / step
        
        # Integer parts
        x0 = int(gx)
        z0 = int(gz)
        
        # Clamping
        if x0 >= self.size - 1: x0 = self.size - 2
        if z0 >= self.size - 1: z0 = self.size - 2
        if x0 < 0: x0 = 0
        if z0 < 0: z0 = 0
        
        # Fractions
        fx = gx - x0
        fz = gz - z0
        
        # Bilinear Interpolation
        idx00 = z0 * self.size + x0
        idx10 = z0 * self.size + (x0 + 1)
        idx01 = (z0 + 1) * self.size + x0
        idx11 = (z0 + 1) * self.size + (x0 + 1)
        
        h00 = self.heights[idx00] * self.height_multiplier
        h10 = self.heights[idx10] * self.height_multiplier
        h01 = self.heights[idx01] * self.height_multiplier
        h11 = self.heights[idx11] * self.height_multiplier
        
        # Lerp X top and bottom
        h_top = h00 + (h10 - h00) * fx
        h_bot = h01 + (h11 - h01) * fx
        
        # Lerp Z
        h = h_top + (h_bot - h_top) * fz
        
        return h

class TerrainGenerator:
    def __init__(self, seed: int = 12345):
        self.seed = seed
    
    def generate_chunk(self, scene: Scene, x_offset: float, z_offset: float, size: int = 128, scale: float = 1.0) -> TerrainObject:
        """
        Generates a terrain chunk entity.
        size: Width/Depth in vertices (e.g. 128).
        scale: Physical size of the chunk in world units.
        """
        # Heightmap generation (using Rust backend)
        print(f"Generating heightmap ({size}x{size})...")
        h_scale = 100.0
        # Use simple Perlin/Simplex via backend if available
        heights = generate_heightmap(size, size, self.seed, h_scale, 4, 0.5, 2.0)
        
        # Grid parameters
        step = scale / (size - 1)
        # Tweak multiplier for more dramatic terrain
        height_multiplier = 25.0 
        
        vertices = [] 
        uvs = []      
        normals = []  
        
        print("Calculating vertices...")
        for z in range(size):
            for x in range(size):
                idx = z * size + x
                h = heights[idx] * height_multiplier
                
                # World pos relative to chunk origin
                wx = x * step
                wz = z * step
                
                vertices.append((wx, h, wz))
                uvs.append((x / (size - 1), z / (size - 1)))
        
        print("Calculating normals...")
        for z in range(size):
            for x in range(size):
                idx = z * size + x
                
                # Neighbors interactions for normals...
                # (Skipping full normal recalc logic for brevity in this replace, assuming it was mostly correct)
                # Actually, I should preserve the normal logic or it will look flat.
                # Let's copy the normal logic from previous file content or simplify.
                
                h_l = heights[z * size + (x - 1)] * height_multiplier if x > 0 else heights[idx] * height_multiplier
                h_r = heights[z * size + (x + 1)] * height_multiplier if x < size - 1 else heights[idx] * height_multiplier
                h_d = heights[(z - 1) * size + x] * height_multiplier if z > 0 else heights[idx] * height_multiplier
                h_u = heights[(z + 1) * size + x] * height_multiplier if z < size - 1 else heights[idx] * height_multiplier
                
                nx = -(h_r - h_l) / (2 * step)
                nz = -(h_u - h_d) / (2 * step)
                ny = 1.0
                
                l = math.sqrt(nx*nx + ny*ny + nz*nz)
                normals.append((nx/l, ny/l, nz/l))

        print("Generating indices...")
        indices_list = []
        for z in range(size - 1):
            for x in range(size - 1):
                i0 = z * size + x
                i1 = z * size + (x + 1)
                i2 = (z + 1) * size + x
                i3 = (z + 1) * size + (x + 1)
                
                indices_list.append((i0, i2, i1))
                indices_list.append((i1, i2, i3))

        # Write OBJ
        obj_filename = f"terrain_chunk_{x_offset}_{z_offset}.obj"
        obj_path = os.path.abspath(obj_filename)
        
        print(f"Writing OBJ to {obj_path}...")
        with open(obj_path, "w") as f:
            f.write(f"# Terrain Chunk {x_offset} {z_offset}\n")
            f.write(f"o Terrain\n")
            for v in vertices:
                f.write(f"v {v[0]:.4f} {v[1]:.4f} {v[2]:.4f}\n")
            for u in uvs:
                f.write(f"vt {u[0]:.4f} {u[1]:.4f}\n")
            for n in normals:
                f.write(f"vn {n[0]:.4f} {n[1]:.4f} {n[2]:.4f}\n")
            for t in indices_list:
                idx0, idx1, idx2 = t[0]+1, t[1]+1, t[2]+1
                f.write(f"f {idx0}/{idx0}/{idx0} {idx1}/{idx1}/{idx1} {idx2}/{idx2}/{idx2}\n")

        print("Loading mesh into backend...")
        mesh_id = backend.load_mesh(obj_path)
        
        entity = Entity(f"Terrain_{x_offset}_{z_offset}", x_offset, 0, z_offset)
        entity.set_mesh(mesh_id)
        entity.set_material("default", "flat_normal")
        entity.set_scale(1.0, 1.0, 1.0) 
        
        scene.add(entity)
        
        return TerrainObject(entity, heights, size, scale, x_offset, z_offset, height_multiplier)
