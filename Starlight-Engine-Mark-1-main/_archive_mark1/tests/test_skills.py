import unittest
from unittest.mock import MagicMock
import sys
import os

# Add pysrc to path
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../pysrc')))

# Mock backend BEFORE importing game code
sys.modules['starlight.backend'] = MagicMock()
sys.modules['engine.backend'] = MagicMock()

from starlight.skills.base import Skill
from starlight.skills.manager import SkillManager
from starlight.entity import Entity
from starlight.input import Input

class MockSkill(Skill):
    def __init__(self):
        super().__init__("MockSkill", cooldown=5.0)
        self.cast_count = 0

    def cast(self, caster, target=None, **kwargs):
        self.cast_count += 1
        return True

class TestSkillSystem(unittest.TestCase):
    def setUp(self):
        self.mock_owner = MagicMock(spec=Entity)
        self.mock_input = MagicMock(spec=Input)
        self.manager = SkillManager(self.mock_owner, self.mock_input)

    def test_add_skill(self):
        skill = MockSkill()
        idx = self.manager.add_skill(skill)
        self.assertEqual(idx, 0)
        self.assertEqual(len(self.manager.skills), 1)

    def test_cooldown_logic(self):
        skill = MockSkill()
        self.manager.add_skill(skill) # Index 0
        
        # First cast should succeed
        success = skill.try_cast(self.mock_owner)
        self.assertTrue(success)
        self.assertEqual(skill.cast_count, 1)
        self.assertEqual(skill.cooldown_timer, 5.0)

        # Second cast immediate should fail
        success = skill.try_cast(self.mock_owner)
        self.assertFalse(success)
        self.assertEqual(skill.cast_count, 1)

        # Valid update
        skill.update(3.0)
        self.assertEqual(skill.cooldown_timer, 2.0)
        self.assertFalse(skill.is_ready())

        # Finish cooldown
        skill.update(3.0)
        self.assertTrue(skill.is_ready())
        
        # Cast again
        success = skill.try_cast(self.mock_owner)
        self.assertTrue(success)
        self.assertEqual(skill.cast_count, 2)

if __name__ == '__main__':
    unittest.main()
