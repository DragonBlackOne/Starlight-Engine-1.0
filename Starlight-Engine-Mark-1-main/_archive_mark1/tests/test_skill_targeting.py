import unittest
import sys
import os
import time

# Ensure we can import starlight
sys.path.insert(0, os.path.abspath("pysrc"))

import starlight.backend as backend
from starlight.entity import Entity
from starlight.skills.manager import SkillManager
from starlight.skills.base import Skill
from starlight.input import Input
from starlight.keys import Keys

class MockSkill(Skill):
    def __init__(self):
        super().__init__("MockSkill", 1.0)
        self.last_target = None
        self.last_target_entity = None

    def cast(self, caster: Entity, target: list[float] | None = None, **kwargs) -> bool:
        self.last_target = target
        self.last_target_entity = kwargs.get('target_entity')
        return True

class MockInput:
    def __init__(self):
        self.pressed_keys = set()
        self.just_pressed_keys = set()

    def is_key_just_pressed(self, key):
        return key in self.just_pressed_keys

class TestSkillTargeting(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        # Initialize headless engine
        backend.init_headless(800, 600)

    def test_raycast_targeting(self):
        # Spawn caster at origin
        caster_id = backend.spawn_dynamic_cube(0.0, 0.0, 0.0)
        caster = Entity(caster_id)

        # Set rotation to look at -Z (default is 0,0,0 which is looking -Z if we assume standard OpenGL but let's be explicit if needed)
        # backend.set_rotation(caster_id, 0.0, 0.0, 0.0)
        # By default spawn_dynamic_cube has rotation 0,0,0.
        # In get_forward_vector, 0,0,0 results in (0,0,-1).

        # Spawn target at (0, 0.5, -5.0) - slightly up to match eye level +0.5
        # Caster eye level is at Y=0.5. Raycast from 0.5.
        # Target needs to be hit.
        # Target size is 1x1x1 (cube). Center at (0, 0, -5).
        # Ray goes from (0, 0.5, 0) direction (0, 0, -1).
        # It should hit (0, 0.5, -4.5) (front face of target).

        target_id = backend.spawn_static_box(0.0, 0.0, -5.0, 0.5, 0.5, 0.5) # 1x1x1 box
        target_entity = Entity(target_id)

        # Step physics to update colliders
        backend.step_fixed()
        backend.step_fixed()

        # Verify manual raycast
        hit = backend.raycast(0.0, 0.5, 0.0, 0.0, 0.0, -1.0, 100.0, True, caster_id)
        self.assertIsNotNone(hit, "Manual raycast failed")
        if hit:
            self.assertEqual(hit[0], target_id, "Raycast hit wrong entity")

        # Test 2: Rotated Caster
        # Rotate caster 90 degrees around Y (yaw) to look at +X (if -Z is forward and we rotate +90 deg Y)
        # -Z -> +X?
        # Rotation(0, pi/2, 0)
        import math
        backend.set_rotation(caster_id, 0.0, math.pi / 2.0, 0.0)

        # Spawn target at (5.0, 0.0, 0.0) - roughly where +X is
        # Wait, if -Z is forward, +90 deg Y (turning left?) makes it look at -X?
        # Or +X?
        # In standard math:
        # x' = x cos t + z sin t
        # z' = -x sin t + z cos t
        # If vector is (0,0,-1):
        # x' = 0 + (-1)*1 = -1
        # z' = 0 + (-1)*0 = 0
        # So it looks at -X.
        # Let's verify with logic.

        target2_id = backend.spawn_static_box(-5.0, 0.0, 0.0, 0.5, 0.5, 0.5)

        # Update physics
        backend.step_fixed()

        # Raycast should hit target2
        # Caster is at 0,0,0. Rotated Y=90.
        # Manager should calculate forward vector as (-1, 0, 0).
        # Ray from (0, 0.5, 0) direction (-1, 0, 0).
        # Target at (-5, 0, 0).
        # Should hit.

        # Verify via manager (we need to reset keys or use new key)

        # Setup SkillManager
        mock_input = MockInput()
        manager = SkillManager(caster, mock_input)

        skill = MockSkill()
        manager.add_skill(skill)
        manager.bind_key("Q", 0)

        # Simulate input
        mock_input.just_pressed_keys.add(Keys.Q)

        # Verify forward vector math BEFORE manager update
        rot = backend.get_rotation(caster_id)
        print(f"DEBUG: Rotation retrieved: x={rot.x}, y={rot.y}, z={rot.z}")
        self.assertAlmostEqual(rot.y, math.pi/2, places=3, msg="Rotation not set correctly")

        from starlight.math import get_forward_vector
        fwd = get_forward_vector(rot.x, rot.y, rot.z)
        print(f"DEBUG: Forward vector: {fwd}")

        # Should be close to (-1, 0, 0)
        self.assertAlmostEqual(fwd[0], -1.0, delta=0.1, msg=f"Forward X incorrect: {fwd}")
        self.assertAlmostEqual(fwd[1], 0.0, delta=0.1, msg=f"Forward Y incorrect: {fwd}")
        self.assertAlmostEqual(fwd[2], 0.0, delta=0.1, msg=f"Forward Z incorrect: {fwd}")

        # Run update
        manager.update(0.016)

        # Verify targeting (Expect to hit target2 because we rotated)
        self.assertIsNotNone(skill.last_target, "Skill received no target (rotated)")
        self.assertIsNotNone(skill.last_target_entity, "Skill received no target entity (rotated)")

        if skill.last_target_entity:
            print(f"DEBUG: Hit Entity ID: {skill.last_target_entity.id} (Expected {target2_id})")
            self.assertEqual(skill.last_target_entity.id, target2_id, "Skill targeted wrong entity (should be target2)")

if __name__ == "__main__":
    unittest.main()
