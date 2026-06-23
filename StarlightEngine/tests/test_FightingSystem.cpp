#include <gtest/gtest.h>
#include "FightingSystem.hpp"
#include <cstdlib>

namespace starlight {

class FightingSystemTest : public ::testing::Test {
protected:
    FightingSystem system;

    bool IsForward(const Fighter& fighter, FighterDirection dir) {
        return system.IsForward(fighter, dir);
    }

    bool IsDown(FighterDirection dir) {
        return system.IsDown(dir);
    }

    bool HasTappedForward(const Fighter& fighter) {
        return system.HasTappedForward(fighter);
    }

    bool HasTappedDown(const Fighter& fighter) {
        return system.HasTappedDown(fighter);
    }

    void ApplyHit(Fighter& attacker, Fighter& defender, const AttackData& attack) {
        system.ApplyHit(attacker, defender, attack);
    }

    Fighter& GetP1Mut() {
        return system.m_p1;
    }

    Fighter& GetP2Mut() {
        return system.m_p2;
    }

    void UpdateFighterState(Fighter& fighter, float dt) {
        system.UpdateFighterState(fighter, dt);
    }

    bool IsInvincible(const Fighter& fighter) const {
        return system.IsInvincible(fighter);
    }

    bool CheckCommandInput(const Fighter& fighter, const CommandInput& cmd) const {
        return system.CheckCommandInput(fighter, cmd);
    }
};

TEST_F(FightingSystemTest, IsForwardAndIsDownBasicChecks) {
    Fighter f;
    f.facing = 1; // Facing Right

    // Right is forward for facing Right
    EXPECT_TRUE(IsForward(f, FighterDirection::RIGHT));
    EXPECT_TRUE(IsForward(f, FighterDirection::UP_RIGHT));
    EXPECT_TRUE(IsForward(f, FighterDirection::DOWN_RIGHT));
    EXPECT_FALSE(IsForward(f, FighterDirection::LEFT));

    f.facing = -1; // Facing Left
    // Left is forward for facing Left
    EXPECT_TRUE(IsForward(f, FighterDirection::LEFT));
    EXPECT_TRUE(IsForward(f, FighterDirection::UP_LEFT));
    EXPECT_TRUE(IsForward(f, FighterDirection::DOWN_LEFT));
    EXPECT_FALSE(IsForward(f, FighterDirection::RIGHT));

    // Down directions
    EXPECT_TRUE(IsDown(FighterDirection::DOWN));
    EXPECT_TRUE(IsDown(FighterDirection::DOWN_LEFT));
    EXPECT_TRUE(IsDown(FighterDirection::DOWN_RIGHT));
    EXPECT_FALSE(IsDown(FighterDirection::UP));
}

TEST_F(FightingSystemTest, InputTappingBufferDetection) {
    Fighter f;
    f.Reset(0.0f, 200.0f, 1);
    f.inputBufferIndex = 0;

    // Initially no tap
    EXPECT_FALSE(HasTappedForward(f));
    EXPECT_FALSE(HasTappedDown(f));

    // Simulate forward tap (neutral -> forward transition)
    // inputBufferIndex points to the NEXT insert location, so we write to current and advance
    f.inputBuffer[0].direction = FighterDirection::NEUTRAL;
    f.inputBufferIndex = 1;
    EXPECT_FALSE(HasTappedForward(f));

    f.inputBuffer[1].direction = FighterDirection::RIGHT; // Forward
    f.inputBufferIndex = 2;
    EXPECT_TRUE(HasTappedForward(f)); // Neutral -> Forward transition detected!

    // Reset and simulate down tap
    f.Reset(0.0f, 200.0f, 1);
    f.inputBuffer[0].direction = FighterDirection::NEUTRAL;
    f.inputBuffer[1].direction = FighterDirection::DOWN;
    f.inputBufferIndex = 2;
    EXPECT_TRUE(HasTappedDown(f));
}

TEST_F(FightingSystemTest, ApplyHitTriggersParryPerfectDefense) {
    Fighter attacker;
    attacker.Reset(0.0f, 200.0f, 1);
    attacker.currentHealth = 1000;

    Fighter defender;
    defender.Reset(100.0f, 200.0f, -1);
    defender.currentHealth = 1000;
    defender.playerIndex = 0; // Human player (so we don't trigger CPU random chance)
    defender.state = FighterStateType::IDLE;

    // Simulate defender tapping forward to parry high attack
    // Facing Left means Forward is LEFT
    defender.inputBuffer[0].direction = FighterDirection::NEUTRAL;
    defender.inputBuffer[1].direction = FighterDirection::LEFT;
    defender.inputBufferIndex = 2;

    // Standing Punch Attack
    AttackData punch;
    punch.name = "Stand HP";
    punch.damage = 100;
    punch.isLow = false;
    punch.isOverhead = false;
    punch.blockstun = 10;
    punch.hitstun = 20;

    // Apply the hit
    ApplyHit(attacker, defender, punch);

    // Defender should have parried successfully!
    EXPECT_EQ(defender.currentHealth, 1000); // Negated damage
    EXPECT_TRUE(defender.isParryFlash);      // Parry flash active
    EXPECT_EQ(defender.parryFlashFrames, 15); // Flash duration
    EXPECT_EQ(system.GetMatchState().hitPauseFrames, 12); // Parry Screen Freeze triggered
}

TEST_F(FightingSystemTest, SpecialCancelOnHitConnection) {
    EngineContext context;
    system.OnInitialize(context);
    system.StartMatch(0, 1, 0); // Atribui as definições para p1 e p2
    
    Fighter& p1 = GetP1Mut();
    p1.Reset(0.0f, 200.0f, 1);
    p1.currentHealth = 1000;
    p1.superMeter = 1.0f;
    p1.state = FighterStateType::ATTACK;
    p1.currentAttackIndex = 0;
    p1.currentAttack = &p1.def.normals[0];
    p1.attackHasHit = true;
    
    p1.inputBufferIndex = 0;
    for (int i = 0; i < 20; ++i) {
        p1.inputBuffer[i] = InputFrame{FighterDirection::NEUTRAL, 0};
    }
    
    p1.inputBuffer[10].direction = FighterDirection::DOWN;
    p1.inputBuffer[10].buttons = 0;
    
    p1.inputBuffer[11].direction = FighterDirection::DOWN_RIGHT;
    p1.inputBuffer[11].buttons = 0;
    
    p1.inputBuffer[12].direction = FighterDirection::RIGHT;
    p1.inputBuffer[12].SetButton(FighterButton::LP);
    
    p1.inputBufferIndex = 13;
    p1.currentInput = p1.inputBuffer[12];
    
    // Verificações intermediárias para diagnóstico
    ASSERT_GE(p1.def.specials.size(), 1);
    EXPECT_EQ(p1.def.specials[0].name, "Fireball");
    EXPECT_TRUE(CheckCommandInput(p1, p1.def.specials[0]));
    
    UpdateFighterState(p1, 0.016f);
    
    EXPECT_EQ(p1.state, FighterStateType::SPECIAL);
    EXPECT_EQ(p1.currentAttack->name, "Hadouken");
    EXPECT_FALSE(p1.attackHasHit);
}

TEST_F(FightingSystemTest, JuggleLimitAndAirCombos) {
    EngineContext context;
    system.OnInitialize(context);
    system.StartMatch(0, 1, 0); // Atribui as definições para p1 e p2
    
    Fighter& attacker = GetP1Mut();
    attacker.Reset(0.0f, 200.0f, 1);
    
    Fighter& defender = GetP2Mut();
    defender.Reset(100.0f, 200.0f, -1);
    defender.isGrounded = false;
    defender.state = FighterStateType::KNOCKDOWN;
    defender.juggleHits = 0;
    
    AttackData aerialHit;
    aerialHit.name = "Jump MK";
    aerialHit.damage = 50;
    aerialHit.knockbackX = 3.0f;
    aerialHit.knockbackY = 0.0f;
    aerialHit.hitstun = 15;
    aerialHit.blockstun = 8;
    
    ApplyHit(attacker, defender, aerialHit);
    EXPECT_EQ(defender.juggleHits, 1);
    EXPECT_EQ(defender.state, FighterStateType::KNOCKDOWN);
    EXPECT_FALSE(defender.isGrounded);
    EXPECT_NEAR(defender.velocity.y, -3.0f, 0.01f);
    
    defender.isGrounded = false;
    ApplyHit(attacker, defender, aerialHit);
    EXPECT_EQ(defender.juggleHits, 2);
    EXPECT_NEAR(defender.velocity.y, -1.8f, 0.01f);
    
    defender.isGrounded = false;
    ApplyHit(attacker, defender, aerialHit);
    EXPECT_EQ(defender.juggleHits, 3);
    
    EXPECT_TRUE(IsInvincible(defender));
}

} // namespace starlight
