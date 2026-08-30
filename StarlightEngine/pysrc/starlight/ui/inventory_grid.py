"""Grid-based Inventory UI data model.

Usage:
    grid = InventoryGridUI(columns=5, rows=4)
    grid.set_inventory(player_inventory)
    grid.on_slot_click(lambda slot, btn: handle_click(slot, btn))
    # In render loop:
    for cell in grid.cells:
        draw_slot(cell.x, cell.y, cell.item_name, cell.count, cell.selected)
"""
from __future__ import annotations
from dataclasses import dataclass, field
from typing import Callable, Any


@dataclass
class GridCell:
    """Render data for one inventory slot."""
    slot_index: int
    grid_x: int
    grid_y: int
    x: float  # Pixel position
    y: float
    width: float
    height: float
    item_name: str = ""
    item_icon: str = ""
    count: int = 0
    selected: bool = False
    highlighted: bool = False
    empty: bool = True


class InventoryGridUI:
    """Grid-based inventory UI layout manager.

    Example:
        grid = InventoryGridUI(5, 4, cell_size=64, padding=4)
        grid.set_inventory(my_inventory)
        grid.handle_mouse(mx, my, clicked=True)
    """

    def __init__(self, columns: int = 5, rows: int = 4,
                 cell_size: float = 64, padding: float = 4,
                 origin_x: float = 100, origin_y: float = 100) -> None:
        self.columns = columns
        self.rows = rows
        self.cell_size = cell_size
        self.padding = padding
        self.origin_x = origin_x
        self.origin_y = origin_y
        self._inventory: Any = None
        self._selected: int = -1
        self._hovered: int = -1
        self._on_click: Callable[[int, str], None] | None = None
        self._on_swap: Callable[[int, int], None] | None = None
        self._dragging: int = -1

    def set_inventory(self, inventory: Any) -> None:
        """Bind to an Inventory instance."""
        self._inventory = inventory

    def on_slot_click(self, callback: Callable[[int, str], None]) -> None:
        self._on_click = callback

    def on_slot_swap(self, callback: Callable[[int, int], None]) -> None:
        self._on_swap = callback

    @property
    def cells(self) -> list[GridCell]:
        """Get all cells with current item data."""
        result = []
        total = self.columns * self.rows
        for i in range(total):
            gx = i % self.columns
            gy = i // self.columns
            x = self.origin_x + gx * (self.cell_size + self.padding)
            y = self.origin_y + gy * (self.cell_size + self.padding)

            cell = GridCell(
                slot_index=i, grid_x=gx, grid_y=gy,
                x=x, y=y,
                width=self.cell_size, height=self.cell_size,
                selected=(i == self._selected),
                highlighted=(i == self._hovered),
            )

            if self._inventory and i < len(self._inventory.slots):
                stack = self._inventory.slots[i]
                if stack:
                    cell.item_name = stack.item.name
                    cell.item_icon = stack.item.icon
                    cell.count = stack.count
                    cell.empty = False

            result.append(cell)
        return result

    def handle_mouse(self, mx: float, my: float,
                     clicked: bool = False, right_click: bool = False) -> None:
        """Process mouse input."""
        self._hovered = self._slot_at(mx, my)

        if clicked and self._hovered >= 0:
            if self._dragging >= 0:
                # Complete drag swap
                if self._on_swap:
                    self._on_swap(self._dragging, self._hovered)
                elif self._inventory:
                    self._inventory.swap_slots(self._dragging, self._hovered)
                self._dragging = -1
                self._selected = -1
            else:
                self._selected = self._hovered
                if self._on_click:
                    btn = "right" if right_click else "left"
                    self._on_click(self._hovered, btn)

    def start_drag(self, slot: int) -> None:
        self._dragging = slot

    def cancel_drag(self) -> None:
        self._dragging = -1

    @property
    def total_width(self) -> float:
        return self.columns * (self.cell_size + self.padding) - self.padding

    @property
    def total_height(self) -> float:
        return self.rows * (self.cell_size + self.padding) - self.padding

    def _slot_at(self, mx: float, my: float) -> int:
        rx = mx - self.origin_x
        ry = my - self.origin_y
        if rx < 0 or ry < 0:
            return -1
        step = self.cell_size + self.padding
        gx = int(rx / step)
        gy = int(ry / step)
        if gx >= self.columns or gy >= self.rows:
            return -1
        # Check we're within the cell, not the padding
        if (rx % step) > self.cell_size or (ry % step) > self.cell_size:
            return -1
        return gy * self.columns + gx
