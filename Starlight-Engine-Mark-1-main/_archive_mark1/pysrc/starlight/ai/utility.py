"""Utility-based Decision Making (Utility AI).

Usage:
    system = UtilitySystem()
    action_attack = UtilityAction("Attack", weight=1.0)
    action_attack.scorers.append(Scorer(curve_type="linear", input_func=get_dist_score))
    system.add_action(action_attack)
    best_action = system.select_best()
"""
from __future__ import annotations
from dataclasses import dataclass, field
from typing import Callable
import math


@dataclass
class Scorer:
    """Calculates a score (0.0 to 1.0) based on an input."""
    input_func: Callable[[], float]  # Returns 0..1
    curve_type: str = "linear" # linear, quadratic, logistic, inverse
    weight: float = 1.0

    def evaluate(self) -> float:
        val = self.input_func()
        val = max(0.0, min(1.0, val))
        
        if self.curve_type == "linear":
            score = val
        elif self.curve_type == "quadratic":
            score = val * val
        elif self.curve_type == "inverse":
            score = 1.0 - val
        elif self.curve_type == "logistic":
            score = 1.0 / (1.0 + math.exp(-10.0 * (val - 0.5)))
        else:
            score = val
            
        return score * self.weight


@dataclass
class UtilityAction:
    """An action with potential utility."""
    name: str
    weight: float = 1.0
    scorers: list[Scorer] = field(default_factory=list)
    execute: Callable[[], None] | None = None

    def calculate_utility(self) -> float:
        if not self.scorers:
            return 0.0
        
        # Combined score: often average or product.
        # Here we use weighted average (Modification: Product is also common for "AND" logic)
        # Using a specialized "Compensation" formula is best, but let's stick to Weighted Average for generic use.
        total_score = 0.0
        total_weight = 0.0
        
        for scorer in self.scorers:
            total_score += scorer.evaluate()
            total_weight += scorer.weight
            
        if total_weight == 0:
            return 0.0
            
        avg = total_score / total_weight
        return avg * self.weight


class UtilitySystem:
    """Selects the best action based on utility scores.

    Example:
        us = UtilitySystem()
        us.add_action(attack_action)
        us.add_action(flee_action)
        best = us.select_best()
        if best: best.execute()
    """
    
    def __init__(self) -> None:
        self.actions: list[UtilityAction] = []
        self.last_scores: dict[str, float] = {}

    def add_action(self, action: UtilityAction) -> None:
        self.actions.append(action)

    def select_best(self) -> UtilityAction | None:
        best_action = None
        best_score = -1.0
        
        self.last_scores.clear()
        
        for action in self.actions:
            score = action.calculate_utility()
            self.last_scores[action.name] = score
            if score > best_score:
                best_score = score
                best_action = action
                
        return best_action

    def get_reasoning(self) -> str:
        """Debug string showing scores."""
        items = sorted(self.last_scores.items(), key=lambda x: x[1], reverse=True)
        return ", ".join([f"{k}:{v:.2f}" for k, v in items])
