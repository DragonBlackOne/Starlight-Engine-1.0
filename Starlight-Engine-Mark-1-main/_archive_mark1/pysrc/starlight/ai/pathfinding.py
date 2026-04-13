"""A* Pathfinding on NavMesh.

Usage:
    pathfinder = Pathfinder(navmesh)
    path = pathfinder.find_path(start_pos, end_pos)
"""
from __future__ import annotations
from dataclasses import dataclass, field
from typing import Any
import heapq
import math
from .navmesh import NavMesh, NavPoly


@dataclass(order=True)
class PathNode:
    f_cost: float
    poly_index: int = field(compare=False)
    parent: Any = field(compare=False, default=None)
    g_cost: float = field(compare=False, default=0.0)
    h_cost: float = field(compare=False, default=0.0)


class Pathfinder:
    """A* Implementation for NavMesh."""
    
    def __init__(self, navmesh: NavMesh) -> None:
        self.navmesh = navmesh

    def find_path(self, start: tuple[float, float, float], 
                  end: tuple[float, float, float]) -> list[tuple[float, float, float]]:
        start_poly = self.navmesh.find_poly(start[0], start[2])
        end_poly = self.navmesh.find_poly(end[0], end[2])
        
        if start_poly == -1 or end_poly == -1:
            return []
            
        if start_poly == end_poly:
            return [end]
            
        open_list: list[PathNode] = []
        heapq.heappush(open_list, PathNode(0.0, start_poly, None, 0.0, self._heuristic(start_poly, end_poly)))
        closed_set = set()
        
        while open_list:
            current = heapq.heappop(open_list)
            
            if current.poly_index == end_poly:
                return self._build_path(current, end)
                
            if current.poly_index in closed_set:
                continue
            closed_set.add(current.poly_index)
            
            poly = self.navmesh.polys[current.poly_index]
            
            for neighbor_idx in poly.neighbors:
                if neighbor_idx in closed_set:
                    continue
                    
                n_poly = self.navmesh.polys[neighbor_idx]
                dist = math.sqrt((poly.center[0]-n_poly.center[0])**2 + (poly.center[1]-n_poly.center[1])**2)
                
                new_g = current.g_cost + dist
                new_h = self._heuristic(neighbor_idx, end_poly)
                heapq.heappush(open_list, PathNode(new_g + new_h, neighbor_idx, current, new_g, new_h))
                
        return []

    def _heuristic(self, a_idx: int, b_idx: int) -> float:
        a = self.navmesh.polys[a_idx].center
        b = self.navmesh.polys[b_idx].center
        return math.sqrt((a[0]-b[0])**2 + (a[1]-b[1])**2)
        
    def _build_path(self, node: PathNode, end_pos: tuple[float,float,float]) -> list[tuple[float,float,float]]:
        points = []
        curr = node
        while curr:
            poly = self.navmesh.polys[curr.poly_index]
            points.append((poly.center[0], 0.0, poly.center[1]))
            curr = curr.parent
        points = points[::-1]
        if points:
             points[-1] = (end_pos[0], 0.0, end_pos[2])
        return points
