"""NavMesh Data Structure with automatic generation and A* pathfinding.

Usage:
    nm = NavMesh()
    nm.add_poly([(0,0), (10,0), (10,10), (0,10)])
    nm.bake()
    path = nm.find_path(0, 0, 9, 9)

    # Auto-generate from heightmap:
    nm = NavMesh()
    nm.generate_from_heightmap(heightmap, width, height, cell_size=1.0, max_slope=0.6)
    nm.bake()
"""
from __future__ import annotations
from dataclasses import dataclass, field
from typing import Any
import heapq
import math


@dataclass
class NavPoly:
    """A convex polygon in the NavMesh."""
    vertices: list[tuple[float, float]]
    center: tuple[float, float]
    neighbors: list[int]  # Indices of neighbor polys
    id: int = -1

    def contains(self, x: float, z: float) -> bool:
        """Point-in-polygon via ray casting algorithm."""
        n = len(self.vertices)
        inside = False
        j = n - 1
        for i in range(n):
            xi, zi = self.vertices[i]
            xj, zj = self.vertices[j]
            if ((zi > z) != (zj > z)) and (x < (xj - xi) * (z - zi) / (zj - zi + 1e-10) + xi):
                inside = not inside
            j = i
        return inside


class NavMesh:
    """Navigation Mesh container with generation and pathfinding."""

    def __init__(self) -> None:
        self.polys: list[NavPoly] = []

    def add_poly(self, vertices: list[tuple[float, float]]) -> None:
        cx = sum(v[0] for v in vertices) / len(vertices)
        cz = sum(v[1] for v in vertices) / len(vertices)
        self.polys.append(NavPoly(vertices, (cx, cz), [], id=len(self.polys)))

    def generate_from_heightmap(
        self,
        heightmap: list[float],
        width: int,
        height: int,
        cell_size: float = 1.0,
        max_slope: float = 0.6,
    ) -> None:
        """Generate walkable navmesh polygons from a heightmap grid.

        Args:
            heightmap: Flat list of height values (width * height).
            width: Number of columns.
            height: Number of rows.
            cell_size: World-space size of each cell.
            max_slope: Maximum height difference between adjacent cells to be walkable.
        """
        self.polys.clear()

        def get_h(x: int, z: int) -> float:
            if 0 <= x < width and 0 <= z < height:
                return heightmap[z * width + x]
            return float('inf')

        for z in range(height - 1):
            for x in range(width - 1):
                # Check if this quad is walkable (all 4 corners have similar height)
                h00 = get_h(x, z)
                h10 = get_h(x + 1, z)
                h01 = get_h(x, z + 1)
                h11 = get_h(x + 1, z + 1)

                max_diff = max(
                    abs(h00 - h10), abs(h00 - h01),
                    abs(h10 - h11), abs(h01 - h11),
                    abs(h00 - h11), abs(h10 - h01),
                )

                if max_diff <= max_slope:
                    wx = x * cell_size
                    wz = z * cell_size
                    self.add_poly([
                        (wx, wz),
                        (wx + cell_size, wz),
                        (wx + cell_size, wz + cell_size),
                        (wx, wz + cell_size),
                    ])

    def bake(self) -> None:
        """Build neighbor connectivity between polygons."""
        for p in self.polys:
            p.neighbors.clear()

        for i, p1 in enumerate(self.polys):
            for j, p2 in enumerate(self.polys):
                if i >= j:
                    continue
                if self._share_edge(p1, p2):
                    p1.neighbors.append(j)
                    p2.neighbors.append(i)

    def find_poly(self, x: float, z: float) -> int:
        """Find which polygon contains the given point."""
        for i, p in enumerate(self.polys):
            if p.contains(x, z):
                return i
        # Fallback: nearest center
        best = -1
        best_dist = float('inf')
        for i, p in enumerate(self.polys):
            dx = x - p.center[0]
            dz = z - p.center[1]
            d = dx * dx + dz * dz
            if d < best_dist:
                best_dist = d
                best = i
        return best

    def find_path(self, sx: float, sz: float, gx: float, gz: float) -> list[tuple[float, float]]:
        """A* pathfinding over the nav mesh polygons.

        Returns a list of (x, z) waypoints from start to goal.
        """
        start_poly = self.find_poly(sx, sz)
        goal_poly = self.find_poly(gx, gz)

        if start_poly < 0 or goal_poly < 0:
            return []
        if start_poly == goal_poly:
            return [(gx, gz)]

        # A* on polygon graph
        open_set: list[tuple[float, int]] = [(0.0, start_poly)]
        came_from: dict[int, int] = {}
        g_score: dict[int, float] = {start_poly: 0.0}

        goal_center = self.polys[goal_poly].center

        while open_set:
            _, current = heapq.heappop(open_set)

            if current == goal_poly:
                # Reconstruct path
                path = [(gx, gz)]
                node = goal_poly
                while node in came_from:
                    node = came_from[node]
                    if node != start_poly:
                        path.append(self.polys[node].center)
                path.reverse()
                return path

            for neighbor in self.polys[current].neighbors:
                nc = self.polys[neighbor].center
                cc = self.polys[current].center
                edge_cost = math.sqrt((nc[0] - cc[0]) ** 2 + (nc[1] - cc[1]) ** 2)
                tentative_g = g_score[current] + edge_cost

                if tentative_g < g_score.get(neighbor, float('inf')):
                    came_from[neighbor] = current
                    g_score[neighbor] = tentative_g
                    h = math.sqrt((nc[0] - goal_center[0]) ** 2 + (nc[1] - goal_center[1]) ** 2)
                    f = tentative_g + h
                    heapq.heappush(open_set, (f, neighbor))

        return []  # No path found

    def _share_edge(self, p1: NavPoly, p2: NavPoly) -> bool:
        shared = 0
        for v1 in p1.vertices:
            for v2 in p2.vertices:
                dist = (v1[0] - v2[0]) ** 2 + (v1[1] - v2[1]) ** 2
                if dist < 0.01:
                    shared += 1
        return shared >= 2
