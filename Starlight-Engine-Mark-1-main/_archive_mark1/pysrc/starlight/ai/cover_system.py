"""Tactical Cover System.

Usage:
    cs = CoverSystem()
    cs.add_point(CoverPoint((10, 0, 5), normal=(0, 0, 1), type="full"))
    best_cover = cs.find_cover(seeker_pos=(0, 0, 0), threat_pos=(20, 0, 5))
"""
from __future__ import annotations
from dataclasses import dataclass
from typing import Literal
import math


@dataclass
class CoverPoint:
    """A single cover location."""
    position: tuple[float, float, float]
    normal: tuple[float, float, float] # Direction OUT of cover (towards threat)
    type: str = "full" # "full" (standing), "half" (crouch)
    occupied: bool = False
    
    @property
    def forward(self) -> tuple[float, float, float]:
        return self.normal


class CoverSystem:
    """Manages cover points and queries.

    Example:
        cs = CoverSystem()
        # Populate with level data
        cs.generate_grid(center=(0,0,0), size=(50,50), spacing=2.0)
        # Query
        pt = cs.find_cover(my_pos, enemy_pos)
    """

    def __init__(self) -> None:
        self.points: list[CoverPoint] = []

    def add_point(self, point: CoverPoint) -> None:
        self.points.append(point)

    def find_cover(self, seeker_pos: tuple[float, float, float], 
                   threat_pos: tuple[float, float, float],
                   max_dist: float = 20.0,
                   min_dot: float = 0.5) -> CoverPoint | None:
        """Find best cover protecting against threat."""
        best_pt = None
        best_score = -1.0
        
        # Vector from threat to seeker
        threat_dir_x = seeker_pos[0] - threat_pos[0]
        threat_dir_z = seeker_pos[2] - threat_pos[2]
        dist_threat = math.sqrt(threat_dir_x**2 + threat_dir_z**2)
        if dist_threat > 0.001:
            threat_dir_x /= dist_threat
            threat_dir_z /= dist_threat
            
        for pt in self.points:
            if pt.occupied:
                continue
                
            # Distance check
            dx = pt.position[0] - seeker_pos[0]
            dz = pt.position[2] - seeker_pos[2]
            dist_sq = dx*dx + dz*dz
            if dist_sq > max_dist * max_dist:
                continue
                
            # Dot check: Cover normal should oppose threat direction
            # If cover faces +Z, it protects against threats from +Z
            dot = pt.normal[0] * threat_dir_x + pt.normal[2] * threat_dir_z # No, wait
            
            # Correct logic:
            # Threat is at T. Seeker is at S. Cover is at C.
            # Cover Normal N points OUT of the wall.
            # The vector (T - C) should potentially oppose N?
            # Actually simplest check: Does the wall stand betwen C and T?
            # N dot (T - C) > 0 means T is "in front" of the wall.
            
            # Vector from Cover to Threat
            ctx = threat_pos[0] - pt.position[0]
            ctz = threat_pos[2] - pt.position[2]
            ct_len = math.sqrt(ctx*ctx + ctz*ctz)
            if ct_len > 0.001:
                ctx /= ct_len
                ctz /= ct_len
                
            dot_protection = pt.normal[0] * ctx + pt.normal[2] * ctz
            
            # If dot > 0, threat is in front of cover (Exposed)
            # If dot < 0, threat is behind cover (Protected? No, normal points AWAY from wall)
            # Usually Normal points OUT into open space.
            # So if Threat is in direction of Normal, cover is useless?
            # Wait. If I hide behind a wall, the wall normal points to me? Or to the enemy?
            # Convention: Normal points AWAY from the wall surface.
            # If I am BEHIND the wall, and enemy is IN FRONT, I am hidden.
            # So Threat should be in direction of Normal.
            
            if dot_protection > min_dot:
                 # Threat is roughly in direction of normal -> GOOD cover?
                 # Example: Wall faces North (0,0,1). Threat is North. I am South of wall.
                 # Threat - Cover = (0,0,1). Normal = (0,0,1). Dot = 1. Good.
                 
                 score = dot_protection * 10.0 - (dist_sq / 100.0) # Prefer better angle + closer
                 if score > best_score:
                     best_score = score
                     best_pt = pt
                     
        return best_pt

    def release_cover(self, point: CoverPoint) -> None:
        point.occupied = False
        
    def claim_cover(self, point: CoverPoint) -> None:
        point.occupied = True
