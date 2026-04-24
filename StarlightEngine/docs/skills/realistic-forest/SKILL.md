---
name: realistic-forest
description: Advanced procedural forest generation with Poisson Disk Sampling, vegetation layers, and realistic lighting
---

# Realistic Forest Generation Skill

This skill provides advanced techniques for creating photorealistic forests in the Starlight Engine.

## Core Concepts (from Industry Research)

### 1. Tree Placement Algorithms

Use **Poisson Disk Sampling** instead of random placement:
- Ensures minimum distance between trees (no overlapping trunks)
- Creates natural-looking distribution
- Allows density variation across terrain

```c
def poisson_disk_sample(width, height, min_dist, k=30):
    """Generate points using Poisson Disk Sampling."""
    cell_size = min_dist / math.sqrt(2)
    cols = int(width / cell_size) + 1
    rows = int(height / cell_size) + 1
    grid = [[None for _ in range(cols)] for _ in range(rows)]
    points = []
    active = []
    
    # Start with random point
    x0, y0 = random.random() * width, random.random() * height
    points.append((x0, y0))
    active.append((x0, y0))
    grid[int(y0 / cell_size)][int(x0 / cell_size)] = (x0, y0)
    
    while active:
        idx = random.randint(0, len(active) - 1)
        px, py = active[idx]
        found = False
        
        for _ in range(k):
            angle = random.random() * 2 * math.pi
            r = min_dist + random.random() * min_dist
            nx = px + r * math.cos(angle)
            ny = py + r * math.sin(angle)
            
            if 0 <= nx < width and 0 <= ny < height:
                col, row = int(nx / cell_size), int(ny / cell_size)
                valid = True
                
                for dr in range(-2, 3):
                    for dc in range(-2, 3):
                        r2, c2 = row + dr, col + dc
                        if 0 <= r2 < rows and 0 <= c2 < cols:
                            if grid[r2][c2]:
                                ox, oy = grid[r2][c2]
                                if (nx - ox)**2 + (ny - oy)**2 < min_dist**2:
                                    valid = False
                                    break
                    if not valid:
                        break
                
                if valid:
                    points.append((nx, ny))
                    active.append((nx, ny))
                    grid[row][col] = (nx, ny)
                    found = True
                    break
        
        if not found:
            active.pop(idx)
    
    return points
```

### 2. Vegetation Layers

A realistic forest has multiple layers:

| Layer | Elements | Height Range | Density |
|-------|----------|--------------|---------|
| **Canopy** | Large tree crowns | 15-30m | Medium |
| **Understory** | Small trees, saplings | 5-15m | Low |
| **Shrub** | Bushes, ferns | 0.5-5m | High |
| **Ground Cover** | Grass, moss, mushrooms | 0-0.5m | Very High |

### 3. Tree Variation Parameters

For each tree, vary:
- **Scale**: 0.7 - 1.3 of base size
- **Rotation**: Random 0-360° on Y axis
- **Lean angle**: 0-5° toward light source
- **Species**: Mix 3-5 different tree types
- **Age variation**: Affects trunk thickness and height

### 4. Lighting for Forests

```c
# Golden Hour lighting (most photorealistic)
backend.set_sun_direction(0.3, -0.4, 0.5)        # Low sun angle
backend.set_sun_color(1.0, 0.85, 0.65, 3.5)      # Warm golden, HDR intensity
backend.set_ambient_intensity(0.08)                # Lower ambient for contrast
backend.set_fog(0.015, 0.6, 0.55, 0.45)           # Warm volumetric fog
```

> **NOTE**: `set_sun_color` takes 4 arguments: `(r, g, b, intensity)`. The intensity is an HDR multiplier.

### Framework Alternative:
```c
from`starlight.framework` import Scene
scene = Scene()
scene.set_sun(direction=(0.3, -0.4, 0.5), color=(1.0, 0.85, 0.65), intensity=3.5)
scene.set_ambient(0.08)
scene.set_fog(0.015, (0.6, 0.55, 0.45))
```

### 5. Undergrowth Details

Add small details for realism:
- Ferns at tree bases
- Mushroom clusters on damp areas
- Fallen logs
- Moss-covered rocks
- Scattered leaves on ground

## Implementation Template

```c
class RealisticForest:
    def __init__(self):
        self.tree_variants = []
        self.undergrowth = []
        self.ground_details = []
        
    def load_assets(self):
        """Load all forest textures and meshes."""
        self.grass_tex = backend.load_texture("assets/textures/forest_grass.png")
        self.bark_tex = backend.load_texture("assets/textures/oak_bark.png")
        self.leaves_tex = backend.load_texture("assets/textures/oak_leaves.png")
        self.fern_tex = backend.load_texture("assets/textures/fern_plant.png")
        self.rock_tex = backend.load_texture("assets/textures/moss_rock.png")
        
        # Load tree model (OBJ)
        self.tree_mesh = backend.load_mesh("assets/models/pine_tree.obj")
        
    def generate_forest(self, center_x, center_z, radius, density=0.5):
        """Generate a forest patch with Poisson Disk Sampling."""
        min_dist = 4.0 / density
        points = poisson_disk_sample(radius * 2, radius * 2, min_dist)
        
        trees = []
        for px, pz in points:
            x = center_x + px - radius
            z = center_z + pz - radius
            
            dist = math.sqrt((x - center_x)**2 + (z - center_z)**2)
            if dist > radius:
                continue
            
            scale = 0.7 + random.random() * 0.6
            rot_y = random.random() * 6.28
            
            tree = self.spawn_tree(x, 0, z, scale, rot_y)
            trees.append(tree)
            
            if random.random() < 0.4:
                self.spawn_fern(x + random.uniform(-2, 2), z + random.uniform(-2, 2))
        
        return trees
    
    def spawn_tree(self, x, y, z, scale, rotation):
        """Spawn a tree entity with texture and transforms."""
        tree_id = backend.spawn_entity(x, y, z)
        backend.set_mesh(tree_id, self.tree_mesh)
        backend.set_material_textures(tree_id, self.bark_tex, "flat_normal")
        backend.set_scale(tree_id, scale, scale * 1.2, scale)
        backend.set_rotation(tree_id, 0, rotation, 0)
        return tree_id
    
    def spawn_fern(self, x, z):
        """Spawn undergrowth fern."""
        fern_id = backend.spawn_entity(x, 0.1, z)
        backend.set_mesh(fern_id, "cube")  # Replace with fern mesh when available
        backend.set_material_textures(fern_id, self.fern_tex, "flat_normal")
        backend.set_scale(fern_id, 0.3, 0.3, 0.3)
        backend.set_rotation(fern_id, 0, random.random() * 6.28, 0)
        return fern_id
```

## Best Practices

### DO:
- ✅ Use Poisson Disk Sampling for natural distribution
- ✅ Vary tree scale, rotation, and species
- ✅ Add multiple vegetation layers
- ✅ Use warm lighting with low sun angle and HDR intensity > 2.0
- ✅ Include ground clutter (ferns, mushrooms, rocks)
- ✅ Apply different textures to trunk vs leaves

### DON'T:
- ❌ Place trees on a regular grid
- ❌ Use identical scale for all trees
- ❌ Forget undergrowth and ground cover
- ❌ Use harsh white lighting (always tint warm or cool)
- ❌ Call `set_sun_color` with only 3 args (intensity is required)
- ❌ Ignore LOD for distant trees


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
