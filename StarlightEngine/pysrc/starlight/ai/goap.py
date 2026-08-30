"""Goal Oriented Action Planning (GOAP).

Usage:
    planner = GoapPlanner()
    action = GoapAction("Eat", cost=1, preconditions={"hungry": True}, effects={"hungry": False})
    planner.add_action(action)
    plan = planner.plan(start_state={"hungry": True}, goal_state={"hungry": False})
"""
from __future__ import annotations
from dataclasses import dataclass, field
from typing import Any, Callable
import heapq


@dataclass
class GoapAction:
    """An atomic action the AI can perform."""
    name: str
    cost: int = 1
    preconditions: dict[str, Any] = field(default_factory=dict)
    effects: dict[str, Any] = field(default_factory=dict)
    
    # Execution logic
    check_procedural_precondition: Callable[[], bool] | None = None
    perform: Callable[[float], bool] | None = None  # Returns True when done


@dataclass
class GoapNode:
    parent: GoapNode | None
    running_cost: int
    state: dict[str, Any]
    action: GoapAction | None

    def __lt__(self, other: GoapNode):
        return self.running_cost < other.running_cost


class GoapPlanner:
    """Plans a sequence of actions to reach a goal state."""
    
    def __init__(self) -> None:
        self.actions: list[GoapAction] = []

    def add_action(self, action: GoapAction) -> None:
        self.actions.append(action)

    def plan(self, start_state: dict[str, Any], goal_state: dict[str, Any]) -> list[GoapAction] | None:
        """A* Search for a plan."""
        # Clean start state
        current_state = start_state.copy()
        
        # Check if goal is already met
        if self._in_state(goal_state, current_state):
            return []

        # Open list: (cost, node)
        open_list: list[tuple[int, GoapNode]] = []
        heapq.heappush(open_list, (0, GoapNode(None, 0, current_state, None)))
        
        visited = set()

        while open_list:
            _, current_node = heapq.heappop(open_list)
            
            # Check goal
            if self._in_state(goal_state, current_node.state):
                return self._build_path(current_node)

            # State hash for visited set
            state_tuple = tuple(sorted(current_node.state.items()))
            if state_tuple in visited:
                continue
            visited.add(state_tuple)

            # Expand neighbors
            for action in self.actions:
                if not self._in_state(action.preconditions, current_node.state):
                    continue
                
                # Apply procedural check if needed
                if action.check_procedural_precondition and not action.check_procedural_precondition():
                    continue

                new_state = current_node.state.copy()
                new_state.update(action.effects)
                
                new_cost = current_node.running_cost + action.cost
                new_node = GoapNode(current_node, new_cost, new_state, action)
                heapq.heappush(open_list, (new_cost, new_node))

        return None  # No plan found

    def _in_state(self, test: dict[str, Any], state: dict[str, Any]) -> bool:
        """Check if 'test' subset matches 'state'."""
        for k, v in test.items():
            if k not in state or state[k] != v:
                return False
        return True

    def _build_path(self, node: GoapNode) -> list[GoapAction]:
        path = []
        curr = node
        while curr.action:
            path.append(curr.action)
            curr = curr.parent  # type: ignore
        return path[::-1] # Reverse


class GoapAgent:
    """Helper to execute a plan."""
    def __init__(self, planner: GoapPlanner) -> None:
        self.planner = planner
        self.current_plan: list[GoapAction] = []
        self.current_action_index: int = 0
    
    def update(self, dt: float, current_state: dict[str, Any], goal_state: dict[str, Any]) -> None:
        # Replan if no plan or plan finished
        if self.current_action_index >= len(self.current_plan):
            plan = self.planner.plan(current_state, goal_state)
            if plan:
                print(f"[GOAP] New plan: {[a.name for a in plan]}")
                self.current_plan = plan
                self.current_action_index = 0
            else:
                return # Idle / No plan possible

        action = self.current_plan[self.current_action_index]
        if action.perform:
            done = action.perform(dt)
            if done:
                self.current_action_index += 1
