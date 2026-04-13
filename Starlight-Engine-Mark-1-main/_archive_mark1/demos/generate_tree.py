import math
import os

def generate_tree_obj(filename):
    vertices = []
    normals = []
    uvs = []
    indices = []

    # Helper to add vertex
    def add_vertex(x, y, z, nx, ny, nz, u, v):
        vertices.append((x, y, z))
        normals.append((nx, ny, nz))
        uvs.append((u, v))
        return len(vertices) - 1

    # Trunk (Cylinder)
    segments = 8
    height = 2.0
    radius = 0.2

    # Bottom ring
    for i in range(segments):
        angle = (i / segments) * math.pi * 2
        x = math.cos(angle) * radius
        z = math.sin(angle) * radius
        add_vertex(x, 0, z, x, 0, z, i/segments, 0)

    # Top ring
    for i in range(segments):
        angle = (i / segments) * math.pi * 2
        x = math.cos(angle) * (radius * 0.6) # Taper
        z = math.sin(angle) * (radius * 0.6)
        add_vertex(x, height, z, x, 0, z, i/segments, 1)

    # Trunk Indices
    for i in range(segments):
        next_i = (i + 1) % segments
        # Quads as two triangles
        # Bottom: i, Top: i + segments
        # i, next_i, next_i + segments
        # i, next_i + segments, i + segments

        v0 = i
        v1 = next_i
        v2 = next_i + segments
        v3 = i + segments

        indices.extend([v0, v1, v2])
        indices.extend([v0, v2, v3])

    # Foliage (Cone)
    foliage_start_idx = len(vertices)
    cone_height = 3.0
    cone_radius = 1.5
    cone_y_base = height * 0.8

    # Cone Base vertices
    for i in range(segments):
        angle = (i / segments) * math.pi * 2
        x = math.cos(angle) * cone_radius
        z = math.sin(angle) * cone_radius
        # Normal points out and up slightly
        nx = math.cos(angle)
        nz = math.sin(angle)
        add_vertex(x, cone_y_base, z, nx, 0.5, nz, i/segments, 0)

    # Cone Tip
    tip_idx = add_vertex(0, cone_y_base + cone_height, 0, 0, 1, 0, 0.5, 1)

    # Cone Indices
    for i in range(segments):
        next_i = (i + 1) % segments
        v0 = foliage_start_idx + i
        v1 = foliage_start_idx + next_i
        v2 = tip_idx
        indices.extend([v0, v1, v2])

        # Base cap (optional, skip for speed)

    # Write OBJ
    with open(filename, "w") as f:
        f.write("# Simple Tree\n")
        for v in vertices:
            f.write(f"v {v[0]:.4f} {v[1]:.4f} {v[2]:.4f}\n")
        for vt in uvs:
            f.write(f"vt {vt[0]:.4f} {vt[1]:.4f}\n")
        for vn in normals:
            f.write(f"vn {vn[0]:.4f} {vn[1]:.4f} {vn[2]:.4f}\n")

        for i in range(0, len(indices), 3):
            # OBJ indices are 1-based
            f.write(f"f {indices[i]+1}/{indices[i]+1}/{indices[i]+1} {indices[i+1]+1}/{indices[i+1]+1}/{indices[i+1]+1} {indices[i+2]+1}/{indices[i+2]+1}/{indices[i+2]+1}\n")

if __name__ == "__main__":
    os.makedirs("assets/models", exist_ok=True)
    generate_tree_obj("assets/models/tree.obj")
    print("Generated assets/models/tree.obj")
