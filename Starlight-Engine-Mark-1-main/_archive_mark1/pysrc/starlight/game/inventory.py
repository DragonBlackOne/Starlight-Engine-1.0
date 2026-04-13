"""Inventory system with items, stacks, and slot management.

Usage:
    inv = Inventory(max_slots=20)
    sword = Item("Iron Sword", item_type="weapon", max_stack=1, stats={"attack": 10})
    potion = Item("Health Potion", item_type="consumable", max_stack=99)
    inv.add_item(sword)
    inv.add_item(potion, count=5)
    inv.use_item(1)  # Use potion at slot 1
"""
from __future__ import annotations
from dataclasses import dataclass, field
from typing import Any, Callable


@dataclass
class Item:
    """Item definition (template)."""
    name: str
    item_type: str = "misc"  # weapon, armor, consumable, material, misc
    max_stack: int = 1
    weight: float = 0.0
    value: int = 0
    icon: str = ""
    description: str = ""
    stats: dict[str, float] = field(default_factory=dict)
    on_use: Callable[["Item"], bool] | None = None
    tags: set[str] = field(default_factory=set)

    @property
    def stackable(self) -> bool:
        return self.max_stack > 1


@dataclass
class ItemStack:
    """An item with a quantity in an inventory slot."""
    item: Item
    count: int = 1

    @property
    def full(self) -> bool:
        return self.count >= self.item.max_stack

    def can_add(self, amount: int = 1) -> int:
        """Returns how many can actually be added."""
        return min(amount, self.item.max_stack - self.count)


class Inventory:
    """Slot-based inventory.

    Example:
        inv = Inventory(max_slots=10)
        inv.add_item(Item("Gold Coin", max_stack=999, item_type="currency"), count=50)
        inv.add_item(Item("Shield", item_type="armor", stats={"defense": 5}))
        print(inv)  # Shows all slots
    """

    def __init__(self, max_slots: int = 20) -> None:
        self.max_slots = max_slots
        self.slots: list[ItemStack | None] = [None] * max_slots
        self._on_change: list[Callable[[], None]] = []

    def add_item(self, item: Item, count: int = 1) -> int:
        """Add items. Returns number of items that didn't fit."""
        remaining = count

        # Try to stack with existing items first
        if item.stackable:
            for i, slot in enumerate(self.slots):
                if slot and slot.item.name == item.name and not slot.full:
                    can_add = slot.can_add(remaining)
                    slot.count += can_add
                    remaining -= can_add
                    if remaining <= 0:
                        self._notify_change()
                        return 0

        # Fill empty slots
        while remaining > 0:
            empty = self._first_empty()
            if empty == -1:
                break
            add_count = min(remaining, item.max_stack)
            self.slots[empty] = ItemStack(item=item, count=add_count)
            remaining -= add_count

        self._notify_change()
        return remaining  # Items that didn't fit

    def remove_item(self, slot_index: int, count: int = 1) -> Item | None:
        """Remove items from a slot. Returns the item if slot becomes empty."""
        if not (0 <= slot_index < self.max_slots):
            return None
        stack = self.slots[slot_index]
        if not stack:
            return None

        stack.count -= count
        item = stack.item
        if stack.count <= 0:
            self.slots[slot_index] = None
        self._notify_change()
        return item

    def use_item(self, slot_index: int) -> bool:
        """Use a consumable item. Returns True if used."""
        stack = self.slots[slot_index] if 0 <= slot_index < self.max_slots else None
        if not stack:
            return False
        if stack.item.on_use and stack.item.on_use(stack.item):
            self.remove_item(slot_index, 1)
            return True
        return False

    def swap_slots(self, a: int, b: int) -> None:
        """Swap two inventory slots."""
        if 0 <= a < self.max_slots and 0 <= b < self.max_slots:
            self.slots[a], self.slots[b] = self.slots[b], self.slots[a]
            self._notify_change()

    def find_item(self, name: str) -> int:
        """Find first slot containing item by name. Returns -1 if not found."""
        for i, slot in enumerate(self.slots):
            if slot and slot.item.name == name:
                return i
        return -1

    def count_item(self, name: str) -> int:
        """Count total items across all stacks."""
        return sum(s.count for s in self.slots if s and s.item.name == name)

    def has_item(self, name: str, count: int = 1) -> bool:
        return self.count_item(name) >= count

    @property
    def used_slots(self) -> int:
        return sum(1 for s in self.slots if s is not None)

    @property
    def free_slots(self) -> int:
        return self.max_slots - self.used_slots

    @property
    def total_weight(self) -> float:
        return sum(s.item.weight * s.count for s in self.slots if s)

    def on_change(self, callback: Callable[[], None]) -> None:
        self._on_change.append(callback)

    def _notify_change(self) -> None:
        for cb in self._on_change:
            cb()

    def _first_empty(self) -> int:
        for i, s in enumerate(self.slots):
            if s is None:
                return i
        return -1

    def to_dict(self) -> list[dict[str, Any] | None]:
        result = []
        for slot in self.slots:
            if slot:
                result.append({"name": slot.item.name, "type": slot.item.item_type,
                               "count": slot.count, "stats": slot.item.stats})
            else:
                result.append(None)
        return result

    def __repr__(self) -> str:
        lines = [f"Inventory ({self.used_slots}/{self.max_slots}):"]
        for i, slot in enumerate(self.slots):
            if slot:
                lines.append(f"  [{i}] {slot.item.name} x{slot.count}")
        return "\n".join(lines)
