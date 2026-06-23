#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>
#include "EngineSystem.hpp"

namespace starlight {

// ─── Input Definitions ───────────────────────────────────────────────────────

enum class FighterButton : uint8_t {
    LP = 0,  // Light Punch
    MP,      // Medium Punch
    HP,      // Heavy Punch
    LK,      // Light Kick
    MK,      // Medium Kick
    HK,      // Heavy Kick
    COUNT
};

enum class FighterDirection : uint8_t {
    NEUTRAL = 5,  // Numpad notation
    UP = 8,
    DOWN = 2,
    LEFT = 4,
    RIGHT = 6,
    UP_LEFT = 7,
    UP_RIGHT = 9,
    DOWN_LEFT = 1,
    DOWN_RIGHT = 3
};

struct InputFrame {
    FighterDirection direction = FighterDirection::NEUTRAL;
    uint8_t buttons = 0;  // Bitmask of FighterButton

    bool HasButton(FighterButton btn) const {
        return (buttons & (1 << static_cast<uint8_t>(btn))) != 0;
    }
    void SetButton(FighterButton btn) {
        buttons |= (1 << static_cast<uint8_t>(btn));
    }
};

// ─── Hitbox Types ────────────────────────────────────────────────────────────

enum class BoxType : uint8_t {
    PUSH,   // Prevents overlapping
    HURT,   // Vulnerable area
    HIT,    // Attack area
    THROW   // Throw range
};

struct FightBox {
    BoxType type = BoxType::HURT;
    glm::vec2 offset = {0.0f, 0.0f};  // Relative to fighter position
    glm::vec2 size = {40.0f, 100.0f};
    bool active = true;
};

// ─── Attack Data ─────────────────────────────────────────────────────────────

struct AttackData {
    std::string name;
    int startup = 5;       // Frames before hitbox appears
    int active = 3;        // Frames hitbox is active
    int recovery = 10;     // Frames after hitbox deactivates
    int damage = 50;
    int hitstun = 15;      // Frames opponent is stuck in hitstun
    int blockstun = 8;     // Frames opponent is stuck in blockstun
    float knockbackX = 3.0f;
    float knockbackY = 0.0f;
    bool isOverhead = false;  // Must block standing
    bool isLow = false;       // Must block crouching

    // Hitbox for this attack (offset from fighter center)
    glm::vec2 hitboxOffset = {30.0f, -20.0f};
    glm::vec2 hitboxSize = {40.0f, 30.0f};

    int TotalFrames() const { return startup + active + recovery; }
};

// ─── Command Input ──────────────────────────────────────────────────────────

struct CommandInput {
    std::string name;
    // Sequence of directions required, e.g., QCF = {2, 3, 6}
    std::vector<FighterDirection> sequence;
    // Which button categories trigger it (bitmask: bit0=anyPunch, bit1=anyKick)
    uint8_t buttonMask = 0x01;  // 0x01 = any punch, 0x02 = any kick
    int windowFrames = 15;  // Max frames allowed for input
    AttackData attack;
};

// ─── Fighter State ──────────────────────────────────────────────────────────

enum class FighterStateType : uint8_t {
    IDLE,
    WALK_FORWARD,
    WALK_BACKWARD,
    CROUCH,
    CROUCH_TRANSITION,
    JUMP_STARTUP,
    JUMP,
    JUMP_LAND,
    ATTACK,
    SPECIAL,
    BLOCKSTUN_STAND,
    BLOCKSTUN_CROUCH,
    HITSTUN_STAND,
    HITSTUN_CROUCH,
    KNOCKDOWN,
    GETUP,
    THROW_STARTUP,
    THROW_ACTIVE,
    THROWN,
    KO,
    INTRO,
    WIN
};

struct FighterDef {
    std::string name;
    float walkSpeed = 3.5f;
    float backWalkSpeed = 2.5f;
    float jumpHeight = 180.0f;
    float jumpForwardSpeed = 3.0f;
    float jumpDuration = 0.5f;
    int health = 1000;
    float height = 120.0f;
    float width = 50.0f;
    glm::vec3 bodyColor = {0.2f, 0.5f, 0.9f};
    glm::vec3 accentColor = {0.9f, 0.8f, 0.2f};
    int maxJumps = 1;

    // Push box
    glm::vec2 pushBoxSize = {40.0f, 110.0f};
    glm::vec2 pushBoxOffset = {0.0f, -55.0f};

    // Hurt boxes (standing, crouching)
    glm::vec2 hurtBoxStandSize = {45.0f, 110.0f};
    glm::vec2 hurtBoxStandOffset = {0.0f, -55.0f};
    glm::vec2 hurtBoxCrouchSize = {50.0f, 70.0f};
    glm::vec2 hurtBoxCrouchOffset = {0.0f, -35.0f};

    // Normal attacks (indexed by button, 0-5 standing, 6-11 crouching, 12-17 aerial)
    std::array<AttackData, 18> normals;

    // Special moves
    std::vector<CommandInput> specials;

    // Throw
    AttackData throwAttack;
};

struct Fighter {
    FighterDef def;
    int playerIndex = 0;  // 0 = P1, 1 = P2/CPU

    // Position & physics
    glm::vec2 position = {0.0f, 0.0f};
    glm::vec2 velocity = {0.0f, 0.0f};
    int facing = 1;  // 1 = right, -1 = left

    // State
    FighterStateType state = FighterStateType::INTRO;
    int stateFrame = 0;  // Current frame in the state
    int currentAttackIndex = -1;  // Index into normals or -1 for special
    const AttackData* currentAttack = nullptr;
    AttackData activeAttack;
    bool attackHasHit = false;  // Prevent multi-hit in single active window

    // Health & damage
    int currentHealth = 1000;
    int comboCount = 0;
    int comboDamage = 0;

    // Input
    static constexpr int INPUT_BUFFER_SIZE = 60;
    std::array<InputFrame, INPUT_BUFFER_SIZE> inputBuffer;
    int inputBufferIndex = 0;
    InputFrame currentInput;

    // Ground reference
    float groundY = 0.0f;
    bool isGrounded = true;
    bool isCrouching = false;

    // Hitstun/blockstun tracking
    int stunFramesRemaining = 0;
    int knockdownFramesRemaining = 0;
    int getupFrames = 0;

    // KO
    bool isKO = false;

    // EX and Super meter and Double jump properties
    float superMeter = 0.0f;
    int jumpsRemaining = 1;
    bool isEX = false;
    bool isSuper = false;
    bool isCounterFlash = false;
    bool isParryFlash = false;
    int parryFlashFrames = 0;
    int juggleHits = 0;
    bool isCancelFlash = false;
    int cancelFlashFrames = 0;

    void Reset(float startX, float ground, int face) {
        position = {startX, ground};
        velocity = {0.0f, 0.0f};
        facing = face;
        state = FighterStateType::INTRO;
        stateFrame = 0;
        currentAttackIndex = -1;
        currentAttack = nullptr;
        attackHasHit = false;
        currentHealth = def.health;
        comboCount = 0;
        comboDamage = 0;
        inputBufferIndex = 0;
        inputBuffer.fill(InputFrame{});
        currentInput = InputFrame{};
        isGrounded = true;
        isCrouching = false;
        stunFramesRemaining = 0;
        knockdownFramesRemaining = 0;
        getupFrames = 0;
        isKO = false;
        superMeter = 0.0f;
        jumpsRemaining = def.maxJumps;
        isEX = false;
        isSuper = false;
        isCounterFlash = false;
        isParryFlash = false;
        parryFlashFrames = 0;
        juggleHits = 0;
        isCancelFlash = false;
        cancelFlashFrames = 0;
        groundY = ground;
    }

    FightBox GetPushBox() const {
        FightBox box;
        box.type = BoxType::PUSH;
        box.offset = def.pushBoxOffset;
        box.size = def.pushBoxSize;
        return box;
    }

    FightBox GetHurtBox() const {
        FightBox box;
        box.type = BoxType::HURT;
        if (isCrouching) {
            box.offset = def.hurtBoxCrouchOffset;
            box.size = def.hurtBoxCrouchSize;
        } else {
            box.offset = def.hurtBoxStandOffset;
            box.size = def.hurtBoxStandSize;
        }
        return box;
    }

    FightBox GetHitBox() const {
        FightBox box;
        box.type = BoxType::HIT;
        box.active = false;
        if (currentAttack && (state == FighterStateType::ATTACK || state == FighterStateType::SPECIAL)) {
            int frameInAttack = stateFrame;
            if (frameInAttack >= currentAttack->startup &&
                frameInAttack < currentAttack->startup + currentAttack->active) {
                box.active = true;
                box.offset = currentAttack->hitboxOffset;
                box.offset.x *= static_cast<float>(facing);
                box.size = currentAttack->hitboxSize;
            }
        }
        return box;
    }

    glm::vec2 GetBoxWorldPos(const FightBox& box) const {
        return position + box.offset;
    }
};

// ─── Match State ─────────────────────────────────────────────────────────────

enum class MatchPhase : uint8_t {
    NONE,
    INTRO,        // "ROUND X"
    COUNTDOWN,    // "FIGHT!"
    FIGHTING,     // Active gameplay
    HIT_PAUSE,    // Freeze on big hits
    KO,           // KO animation
    ROUND_END,    // Score update
    MATCH_END,    // Final result
    PAUSED
};

struct MatchState {
    int roundsToWin = 2;    // Best of 3 = 2 rounds to win
    int maxRounds = 3;
    int currentRound = 1;
    int p1Score = 0;
    int p2Score = 0;
    float timer = 99.0f;
    float timerSpeed = 1.0f;
    MatchPhase phase = MatchPhase::NONE;
    float phaseTimer = 0.0f;  // Countdown for current phase
    int hitPauseFrames = 0;
    int comboCountP1 = 0;    // Current combo the P1 is performing on P2
    int comboCountP2 = 0;    // Current combo the P2 is performing on P1

    // AI settings
    int aiDifficulty = 1;  // 0=easy, 1=normal, 2=hard
    int aiReactionDelay = 8;  // Frames of reaction delay
    int aiBlockChance = 60;   // Percentage chance to block

    // Character / stage selection
    int p1CharIndex = 0;
    int p2CharIndex = 1;
    int stageIndex = 0;

    void Reset() {
        currentRound = 1;
        p1Score = 0;
        p2Score = 0;
        phase = MatchPhase::NONE;
        phaseTimer = 0.0f;
        hitPauseFrames = 0;
        comboCountP1 = 0;
        comboCountP2 = 0;
    }
};

// ─── AI Controller ──────────────────────────────────────────────────────────

struct AIState {
    int reactionTimer = 0;
    int actionTimer = 0;
    int currentAction = 0;  // 0=idle, 1=advance, 2=retreat, 3=attack, 4=block, 5=jump
    float distanceToOpponent = 0.0f;
    bool shouldBlock = false;
    int blockTimer = 0;
};

// ─── Fighting System ────────────────────────────────────────────────────────

class FightingSystem : public ISystem {
    friend class FightingSystemTest;
public:
    FightingSystem();
    ~FightingSystem();

    // ISystem
    bool OnInitialize(const EngineContext& context) override;
    void OnShutdown() override;
    void OnFixedUpdate(float dt) override;
    const char* GetName() const override { return "FightingSystem"; }

    // Public API
    void StartMatch(int p1Char, int p2Char, int stage);
    void SetRoundCount(int rounds);
    void SetAIDifficulty(int difficulty);

    // Getters for Lua bindings
    const Fighter& GetP1() const { return m_p1; }
    const Fighter& GetP2() const { return m_p2; }
    const MatchState& GetMatchState() const { return m_match; }
    MatchState& GetMatchStateMut() { return m_match; }

    struct Projectile {
        glm::vec2 position = {0.0f, 0.0f};
        glm::vec2 velocity = {0.0f, 0.0f};
        glm::vec2 size = {25.0f, 25.0f};
        int ownerIndex = 0;
        int damage = 60;
        int hitstun = 20;
        int blockstun = 10;
        float knockbackX = 4.0f;
        float knockbackY = 0.0f;
        bool active = false;
        int facing = 1;
    };
    const std::vector<Projectile>& GetProjectiles() const { return m_projectiles; }
    std::vector<Projectile>& GetProjectilesMut() { return m_projectiles; }

    // Fighter definitions
    const std::vector<FighterDef>& GetFighterDefs() const { return m_fighterDefs; }
    void RegisterFighterDef(const FighterDef& def);

    bool IsActive() const { return m_active; }
    void SetActive(bool active) { m_active = active; }

    struct ReplayFrame {
        InputFrame p1Input;
        InputFrame p2Input;
    };

    void StartRecordingReplay();
    void StopRecordingReplay();
    void SaveReplay(const std::string& path);
    bool LoadReplay(const std::string& path);
    void PlayReplay();
    void StopReplay() { m_isPlayingReplay = false; }
    bool IsReplaying() const { return m_isPlayingReplay; }

private:
    Fighter m_p1;
    Fighter m_p2;
    MatchState m_match;
    AIState m_aiState;
    bool m_active = false;
    std::vector<Projectile> m_projectiles;
    int m_slowMoFrames = 0;
    int m_slowMoTick = 0;

    bool m_isRecording = false;
    bool m_isPlayingReplay = false;
    std::vector<ReplayFrame> m_replayBuffer;
    size_t m_replayPlaybackIndex = 0;

    std::vector<FighterDef> m_fighterDefs;

    // Arena bounds
    float m_arenaLeft = -300.0f;
    float m_arenaRight = 300.0f;
    float m_groundY = 200.0f;  // Y position of the ground (screen coords, Y-down)

    // Core update functions
    void UpdateMatch(float dt);
    void UpdateFighter(Fighter& fighter, Fighter& opponent, float dt);
    void ReadPlayerInput(Fighter& fighter);
    void ReadAIInput(Fighter& fighter, const Fighter& opponent);
    void UpdateFighterState(Fighter& fighter, float dt);
    void ApplyPhysics(Fighter& fighter, float dt);
    void ResolvePushBoxes();
    void CheckHitCollisions();
    void ApplyHit(Fighter& attacker, Fighter& defender, const AttackData& attack);
    void CheckThrow(Fighter& attacker, Fighter& defender);

    // State transitions
    void TransitionTo(Fighter& fighter, FighterStateType newState);
    bool CanAct(const Fighter& fighter) const;
    bool CanBlock(const Fighter& fighter) const;
    bool IsInHitstun(const Fighter& fighter) const;
    bool IsInBlockstun(const Fighter& fighter) const;
    bool IsAttacking(const Fighter& fighter) const;
    bool IsInvincible(const Fighter& fighter) const;

    // Input detection
    bool CheckCommandInput(const Fighter& fighter, const CommandInput& cmd) const;
    FighterDirection GetDirectionFromInput(const Fighter& fighter) const;
    bool IsHoldingBack(const Fighter& fighter) const;
    FighterDirection MirrorDirection(FighterDirection dir) const;

    // Parry helpers
    bool IsForward(const Fighter& fighter, FighterDirection dir) const;
    bool IsDown(FighterDirection dir) const;
    bool HasTappedForward(const Fighter& fighter) const;
    bool HasTappedDown(const Fighter& fighter) const;

    // AI
    void UpdateAI(float dt);
    void InjectAISpecialInput(Fighter& fighter, const std::string& name, FighterButton btn, bool isEX, bool isSuper);

    // Round management
    void StartRound();
    void EndRound(int winner);  // 0 = draw, 1 = p1, 2 = p2
    void UpdateRoundTimer(float dt);

    // Collision helpers
    bool BoxOverlap(glm::vec2 posA, glm::vec2 sizeA, glm::vec2 posB, glm::vec2 sizeB) const;

    // Init helpers
    void InitDefaultFighters();
    FighterDef CreateBalancedFighter();
    FighterDef CreateGrapplerFighter();
    FighterDef CreateSpeedFighter();
};

}  // namespace starlight
