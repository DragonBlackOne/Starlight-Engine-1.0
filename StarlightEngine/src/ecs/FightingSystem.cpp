#include "FightingSystem.hpp"
#include "InputSystem.hpp"
#include "AudioSystem.hpp"
#include "Log.hpp"
#include "Engine.hpp"
#include "PathResolver.hpp"
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <filesystem>

namespace starlight {

// Helper to synthesize chiptune sound effects in code
static void PlaySynthesizedSound(int soundType) {
    if (!Engine::IsInitialized()) return;
    auto* audio = Engine::Get().GetSystem<AudioSystem>();
    if (!audio) return;
    
    switch (soundType) {
        case 0: // Select / Beep
            audio->PlayNote(600.0f, 0.08f, WaveType::Sine);
            break;
        case 1: // Normal Hit
            audio->PlayNote(130.0f, 0.08f, WaveType::Noise);
            break;
        case 2: // Block sound
            audio->PlayNote(240.0f, 0.05f, WaveType::Triangle);
            break;
        case 3: // Jump/Launch sound
            audio->PlayNote(480.0f, 0.08f, WaveType::Triangle);
            break;
        case 4: // Fireball shoot sound
            audio->PlayNote(320.0f, 0.12f, WaveType::Saw);
            break;
        case 5: // Heavy Hit / KO crash sound
            audio->PlayNote(75.0f, 0.45f, WaveType::Noise);
            break;
        case 6: // Attack whiff swing
            audio->PlayNote(750.0f, 0.04f, WaveType::Sine);
            break;
        case 7: // Special/Super Cancel Sound
            audio->PlayNote(660.0f, 0.06f, WaveType::Saw);
            audio->PlayNote(880.0f, 0.08f, WaveType::Sine);
            break;
        default:
            break;
    }
}

static void SetNormalAttack(FighterDef& def, int index, const std::string& name, int startup, int active, int recovery, int damage, int hitstun, int blockstun, glm::vec2 offset, glm::vec2 size, bool isOverhead = false, bool isLow = false, float kbX = 3.0f, float kbY = 0.0f) {
    AttackData& att = def.normals[index];
    att.name = name;
    att.startup = startup;
    att.active = active;
    att.recovery = recovery;
    att.damage = damage;
    att.hitstun = hitstun;
    att.blockstun = blockstun;
    att.knockbackX = kbX;
    att.knockbackY = kbY;
    att.isOverhead = isOverhead;
    att.isLow = isLow;
    att.hitboxOffset = offset;
    att.hitboxSize = size;
}

FightingSystem::FightingSystem() {
    m_p1.playerIndex = 0;
    m_p2.playerIndex = 1;
}

FightingSystem::~FightingSystem() {
    OnShutdown();
}

bool FightingSystem::OnInitialize(const EngineContext& context) {
    (void)context;
    InitDefaultFighters();
    m_active = false;
    return true;
}

void FightingSystem::OnShutdown() {
    m_fighterDefs.clear();
    m_projectiles.clear();
    m_active = false;
}

void FightingSystem::StartMatch(int p1Char, int p2Char, int stage) {
    if (m_fighterDefs.empty()) {
        InitDefaultFighters();
    }
    
    m_match.Reset();
    m_match.p1CharIndex = p1Char;
    m_match.p2CharIndex = p2Char;
    m_match.stageIndex = stage;
    
    // Assign definitions
    m_p1.def = m_fighterDefs[p1Char % m_fighterDefs.size()];
    m_p2.def = m_fighterDefs[p2Char % m_fighterDefs.size()];
    
    m_p1.playerIndex = 0;
    m_p2.playerIndex = 1;
    
    m_projectiles.clear();
    m_slowMoFrames = 0;
    m_slowMoTick = 0;
    
    if (!m_isPlayingReplay) {
        m_isRecording = true;
        m_replayBuffer.clear();
        m_replayBuffer.reserve(18000); // Pre-allocate for ~5 mins of gameplay
    } else {
        m_isRecording = false;
        m_replayPlaybackIndex = 0;
    }
    
    StartRound();
    m_active = true;
}

void FightingSystem::SetRoundCount(int rounds) {
    m_match.roundsToWin = rounds;
}

void FightingSystem::SetAIDifficulty(int difficulty) {
    m_match.aiDifficulty = difficulty;
}

void FightingSystem::RegisterFighterDef(const FighterDef& def) {
    m_fighterDefs.push_back(def);
}

void FightingSystem::OnFixedUpdate(float dt) {
    if (!m_active) return;
    
    // Slow motion tick skip
    if (m_slowMoFrames > 0) {
        m_slowMoFrames--;
        m_slowMoTick = (m_slowMoTick + 1) % 4;
        if (m_slowMoTick != 0) {
            return;
        }
    }
    
    // Hit pause freeze frames
    if (m_match.hitPauseFrames > 0) {
        m_match.hitPauseFrames--;
        return;
    }
    
    UpdateMatch(dt);
    
    // Only update gameplay if match is running and not in matching end screen
    if (m_match.phase != MatchPhase::MATCH_END && m_match.phase != MatchPhase::PAUSED) {
        // Read input
        if (m_isPlayingReplay) {
            if (m_replayPlaybackIndex < m_replayBuffer.size()) {
                m_p1.currentInput = m_replayBuffer[m_replayPlaybackIndex].p1Input;
                m_p2.currentInput = m_replayBuffer[m_replayPlaybackIndex].p2Input;
                m_replayPlaybackIndex++;
            } else {
                m_isPlayingReplay = false;
                m_p1.currentInput = InputFrame{};
                m_p2.currentInput = InputFrame{};
            }
        } else {
            if (m_match.phase == MatchPhase::FIGHTING) {
                ReadPlayerInput(m_p1);
                if (m_match.aiDifficulty >= 0) {
                    ReadAIInput(m_p2, m_p1);
                } else {
                    ReadPlayerInput(m_p2);
                }
            } else {
                m_p1.currentInput = InputFrame{};
                m_p2.currentInput = InputFrame{};
            }

            if (m_isRecording) {
                ReplayFrame frame;
                frame.p1Input = m_p1.currentInput;
                frame.p2Input = m_p2.currentInput;
                m_replayBuffer.push_back(frame);
            }
        }
        
        // Update fighters
        UpdateFighter(m_p1, m_p2, dt);
        UpdateFighter(m_p2, m_p1, dt);
        
        // Update projectiles
        for (auto& proj : m_projectiles) {
            if (proj.active) {
                proj.position.x += proj.velocity.x;
                proj.position.y += proj.velocity.y;
                
                if (proj.position.x < m_arenaLeft || proj.position.x > m_arenaRight) {
                    proj.active = false;
                }
            }
        }
        
        // Resolve pushboxes
        ResolvePushBoxes();
        
        // Check collisions
        CheckHitCollisions();
    }
}

void FightingSystem::UpdateMatch(float dt) {
    if (m_match.phase == MatchPhase::NONE) return;
    
    if (m_match.phase != MatchPhase::FIGHTING && m_match.phase != MatchPhase::PAUSED) {
        m_match.phaseTimer -= dt;
        if (m_match.phaseTimer <= 0.0f) {
            m_match.phaseTimer = 0.0f;
            
            if (m_match.phase == MatchPhase::INTRO) {
                m_match.phase = MatchPhase::COUNTDOWN;
                m_match.phaseTimer = 1.0f;
                PlaySynthesizedSound(0);
            } else if (m_match.phase == MatchPhase::COUNTDOWN) {
                m_match.phase = MatchPhase::FIGHTING;
                PlaySynthesizedSound(3);
            } else if (m_match.phase == MatchPhase::ROUND_END) {
                bool matchOver = (m_match.p1Score >= m_match.roundsToWin) || (m_match.p2Score >= m_match.roundsToWin);
                if (matchOver) {
                    m_match.phase = MatchPhase::MATCH_END;
                    m_match.phaseTimer = 4.0f;
                } else {
                    m_match.currentRound++;
                    StartRound();
                }
            } else if (m_match.phase == MatchPhase::KO) {
                int winner = 0;
                if (m_p1.currentHealth <= 0 && m_p2.currentHealth <= 0) {
                    winner = 0;
                } else if (m_p1.currentHealth <= 0) {
                    winner = 2;
                } else if (m_p2.currentHealth <= 0) {
                    winner = 1;
                }
                EndRound(winner);
            }
        }
    }
    
    if (m_match.phase == MatchPhase::FIGHTING) {
        UpdateRoundTimer(dt);
        
        bool p1KO = (m_p1.currentHealth <= 0);
        bool p2KO = (m_p2.currentHealth <= 0);
        
        if (p1KO || p2KO) {
            m_match.phase = MatchPhase::KO;
            m_match.phaseTimer = 3.0f;
            
            // Check if it's the final round (match point for either player)
            bool isFinalRound = (m_match.p1Score == m_match.roundsToWin - 1) || (m_match.p2Score == m_match.roundsToWin - 1) || (m_match.currentRound >= m_match.maxRounds);
            if (isFinalRound) {
                m_slowMoFrames = 120; // 120 frames at 1/4 speed = 2 seconds of slow motion K.O.
                m_slowMoTick = 0;
            }
            
            if (p1KO) {
                TransitionTo(m_p1, FighterStateType::KO);
                m_p1.velocity = { -m_p1.facing * 3.0f, -6.0f };
                m_p1.isGrounded = false;
            }
            if (p2KO) {
                TransitionTo(m_p2, FighterStateType::KO);
                m_p2.velocity = { -m_p2.facing * 3.0f, -6.0f };
                m_p2.isGrounded = false;
            }
            
            PlaySynthesizedSound(5);
        }
    }
}

void FightingSystem::UpdateFighter(Fighter& fighter, Fighter& opponent, float dt) {
    fighter.inputBuffer[fighter.inputBufferIndex] = fighter.currentInput;
    fighter.inputBufferIndex = (fighter.inputBufferIndex + 1) % Fighter::INPUT_BUFFER_SIZE;
    
    fighter.stateFrame++;
    
    if (fighter.parryFlashFrames > 0) {
        fighter.parryFlashFrames--;
        if (fighter.parryFlashFrames == 0) {
            fighter.isParryFlash = false;
        }
    }
    
    if (fighter.cancelFlashFrames > 0) {
        fighter.cancelFlashFrames--;
        if (fighter.cancelFlashFrames == 0) {
            fighter.isCancelFlash = false;
        }
    }
    
    UpdateFighterState(fighter, dt);
    ApplyPhysics(fighter, dt);
    
    // Facing direction calculation
    if (fighter.isGrounded && fighter.state != FighterStateType::HITSTUN_STAND && 
        fighter.state != FighterStateType::HITSTUN_CROUCH && fighter.state != FighterStateType::BLOCKSTUN_STAND && 
        fighter.state != FighterStateType::BLOCKSTUN_CROUCH && fighter.state != FighterStateType::KNOCKDOWN && 
        fighter.state != FighterStateType::GETUP && fighter.state != FighterStateType::WIN && 
        fighter.state != FighterStateType::KO && fighter.state != FighterStateType::ATTACK && 
        fighter.state != FighterStateType::SPECIAL) {
        if (fighter.position.x < opponent.position.x) {
            fighter.facing = 1;
        } else {
            fighter.facing = -1;
        }
    }
}

void FightingSystem::ReadPlayerInput(Fighter& fighter) {
    auto* input = Engine::Get().GetSystem<InputSystem>();
    if (!input) return;
    
    FighterDirection dir = FighterDirection::NEUTRAL;
    uint8_t buttons = 0;
    
    if (fighter.playerIndex == 0) {
        // Player 1: WASD + UIJKOL
        bool up = input->IsKeyPressed(pal::KeyCode::W);
        bool down = input->IsKeyPressed(pal::KeyCode::S);
        bool left = input->IsKeyPressed(pal::KeyCode::A);
        bool right = input->IsKeyPressed(pal::KeyCode::D);
        
        if (up && left) dir = FighterDirection::UP_LEFT;
        else if (up && right) dir = FighterDirection::UP_RIGHT;
        else if (down && left) dir = FighterDirection::DOWN_LEFT;
        else if (down && right) dir = FighterDirection::DOWN_RIGHT;
        else if (up) dir = FighterDirection::UP;
        else if (down) dir = FighterDirection::DOWN;
        else if (left) dir = FighterDirection::LEFT;
        else if (right) dir = FighterDirection::RIGHT;
        
        if (input->IsKeyPressed(pal::KeyCode::U)) buttons |= (1 << static_cast<uint8_t>(FighterButton::LP));
        if (input->IsKeyPressed(pal::KeyCode::I)) buttons |= (1 << static_cast<uint8_t>(FighterButton::MP));
        if (input->IsKeyPressed(pal::KeyCode::O)) buttons |= (1 << static_cast<uint8_t>(FighterButton::HP));
        if (input->IsKeyPressed(pal::KeyCode::J)) buttons |= (1 << static_cast<uint8_t>(FighterButton::LK));
        if (input->IsKeyPressed(pal::KeyCode::K)) buttons |= (1 << static_cast<uint8_t>(FighterButton::MK));
        if (input->IsKeyPressed(pal::KeyCode::L)) buttons |= (1 << static_cast<uint8_t>(FighterButton::HK));
    } else {
        // Player 2: Arrows + Num1-6
        bool up = input->IsKeyPressed(pal::KeyCode::Up);
        bool down = input->IsKeyPressed(pal::KeyCode::Down);
        bool left = input->IsKeyPressed(pal::KeyCode::Left);
        bool right = input->IsKeyPressed(pal::KeyCode::Right);
        
        if (up && left) dir = FighterDirection::UP_LEFT;
        else if (up && right) dir = FighterDirection::UP_RIGHT;
        else if (down && left) dir = FighterDirection::DOWN_LEFT;
        else if (down && right) dir = FighterDirection::DOWN_RIGHT;
        else if (up) dir = FighterDirection::UP;
        else if (down) dir = FighterDirection::DOWN;
        else if (left) dir = FighterDirection::LEFT;
        else if (right) dir = FighterDirection::RIGHT;
        
        if (input->IsKeyPressed(pal::KeyCode::Num4)) buttons |= (1 << static_cast<uint8_t>(FighterButton::LP));
        if (input->IsKeyPressed(pal::KeyCode::Num5)) buttons |= (1 << static_cast<uint8_t>(FighterButton::MP));
        if (input->IsKeyPressed(pal::KeyCode::Num6)) buttons |= (1 << static_cast<uint8_t>(FighterButton::HP));
        if (input->IsKeyPressed(pal::KeyCode::Num1)) buttons |= (1 << static_cast<uint8_t>(FighterButton::LK));
        if (input->IsKeyPressed(pal::KeyCode::Num2)) buttons |= (1 << static_cast<uint8_t>(FighterButton::MK));
        if (input->IsKeyPressed(pal::KeyCode::Num3)) buttons |= (1 << static_cast<uint8_t>(FighterButton::HK));
    }
    
    fighter.currentInput.direction = dir;
    fighter.currentInput.buttons = buttons;
}

void FightingSystem::ReadAIInput(Fighter& fighter, const Fighter& opponent) {
    m_aiState.actionTimer++;
    m_aiState.reactionTimer++;
    
    float dx = opponent.position.x - fighter.position.x;
    float dist = std::abs(dx);
    m_aiState.distanceToOpponent = dist;
    
    int reactionDelay = 15;
    if (m_match.aiDifficulty == 1) reactionDelay = 8;
    if (m_match.aiDifficulty == 2) reactionDelay = 3;
    
    bool opponentIsAttacking = (opponent.state == FighterStateType::ATTACK || opponent.state == FighterStateType::SPECIAL);
    if (opponentIsAttacking) {
        if (m_aiState.reactionTimer >= reactionDelay) {
            int chance = (m_match.aiDifficulty == 0) ? 30 : ((m_match.aiDifficulty == 1) ? 60 : 85);
            m_aiState.shouldBlock = (rand() % 100 < chance);
            m_aiState.reactionTimer = 0;
        }
    } else {
        m_aiState.shouldBlock = false;
    }
    
    if (m_aiState.actionTimer >= (15 + (rand() % 25))) {
        m_aiState.actionTimer = 0;
        int r = rand() % 100;
        if (dist > 140.0f) {
            if (r < 65) m_aiState.currentAction = 1;      // Advance
            else if (r < 85) m_aiState.currentAction = 3; // Attack (ranged fireball)
            else m_aiState.currentAction = 0;            // Idle
        } else {
            if (r < 35) m_aiState.currentAction = 4;      // Block
            else if (r < 75) m_aiState.currentAction = 3; // Attack
            else if (r < 90) m_aiState.currentAction = 2; // Retreat
            else m_aiState.currentAction = 5;            // Jump
        }
    }
    
    if (m_aiState.shouldBlock) {
        m_aiState.currentAction = 4;
    }
    
    fighter.currentInput = InputFrame{};
    
    if (m_aiState.currentAction == 1) {
        fighter.currentInput.direction = (dx > 0) ? FighterDirection::RIGHT : FighterDirection::LEFT;
    } else if (m_aiState.currentAction == 2) {
        fighter.currentInput.direction = (dx > 0) ? FighterDirection::LEFT : FighterDirection::RIGHT;
    } else if (m_aiState.currentAction == 4) {
        bool crouchBlock = (rand() % 100 < 65);
        if (dx > 0) {
            fighter.currentInput.direction = crouchBlock ? FighterDirection::DOWN_LEFT : FighterDirection::LEFT;
        } else {
            fighter.currentInput.direction = crouchBlock ? FighterDirection::DOWN_RIGHT : FighterDirection::RIGHT;
        }
    } else if (m_aiState.currentAction == 5) {
        int r = rand() % 3;
        if (r == 0) fighter.currentInput.direction = FighterDirection::UP;
        else fighter.currentInput.direction = (dx > 0) ? FighterDirection::UP_RIGHT : FighterDirection::UP_LEFT;
    }
    
    if (m_aiState.currentAction == 3) {
        int r = rand() % 100;
        // Decidir se vai soltar um especial (25% de chance)
        if (r < 25) {
            std::string specialName = "";
            FighterButton btn = FighterButton::LP;
            bool isEX = false;
            bool isSuper = false;
            
            // Decidir nivel de meter a usar
            if (fighter.superMeter >= 3.0f && (rand() % 100 < 40)) {
                isSuper = true;
            } else if (fighter.superMeter >= 1.0f && (rand() % 100 < 30)) {
                isEX = true;
            }
            
            if (fighter.def.name == "KAITO") {
                // Kaito specials: Hadouken (Fireball), Shoryuken (Dragon Punch), Tatsumaki (Hurricane Kick)
                if (isSuper) {
                    specialName = "Super Fireball";
                    btn = FighterButton::LP;
                } else {
                    int specChoice = rand() % 3;
                    if (specChoice == 0) {
                        specialName = "Fireball";
                        btn = FighterButton::LP;
                    } else if (specChoice == 1) {
                        specialName = "Dragon Punch";
                        btn = FighterButton::LP;
                    } else {
                        specialName = "Hurricane Kick";
                        btn = FighterButton::LK;
                    }
                }
            } else if (fighter.def.name == "GOROU") {
                // Gorou specials: SPD, Super SPD
                if (isSuper) {
                    specialName = "Super SPD";
                    btn = FighterButton::LP;
                } else {
                    specialName = "Spinning Pile Driver";
                    btn = FighterButton::LP;
                }
            } else if (fighter.def.name == "RIN") {
                // Rin specials: Fujin Slash, Gale Teleport, Super Rin
                if (isSuper) {
                    specialName = "Super Rin";
                    btn = FighterButton::LP;
                } else {
                    int specChoice = rand() % 2;
                    if (specChoice == 0) {
                        specialName = "Fujin Slash";
                        btn = FighterButton::LP;
                    } else {
                        specialName = "Gale Teleport";
                        btn = FighterButton::LK;
                    }
                }
            }
            
            if (!specialName.empty()) {
                InjectAISpecialInput(fighter, specialName, btn, isEX, isSuper);
                return;
            }
        }
        
        int btn = rand() % 6;
        fighter.currentInput.SetButton(static_cast<FighterButton>(btn));
    }
}

void FightingSystem::InjectAISpecialInput(Fighter& fighter, const std::string& name, FighterButton btn, bool isEX, bool isSuper) {
    (void)isSuper;
    const CommandInput* foundCmd = nullptr;
    for (const auto& spec : fighter.def.specials) {
        if (spec.name == name) {
            foundCmd = &spec;
            break;
        }
    }
    
    if (!foundCmd) return;
    
    // Limpa o input do frame atual
    fighter.currentInput = InputFrame{};
    
    // Configura os botoes no frame atual
    if (isEX) {
        fighter.currentInput.SetButton(btn);
        if (btn == FighterButton::LP || btn == FighterButton::MP || btn == FighterButton::HP) {
            fighter.currentInput.SetButton(btn == FighterButton::LP ? FighterButton::MP : FighterButton::LP);
        } else {
            fighter.currentInput.SetButton(btn == FighterButton::LK ? FighterButton::MK : FighterButton::LK);
        }
    } else {
        fighter.currentInput.SetButton(btn);
    }
    
    const auto& seq = foundCmd->sequence;
    if (seq.empty()) return;
    
    // Configura a direcao do frame atual para ser a ultima da sequencia
    FighterDirection lastDir = seq.back();
    if (fighter.facing == -1) {
        lastDir = MirrorDirection(lastDir);
    }
    fighter.currentInput.direction = lastDir;
    
    // Injeta as direcoes anteriores retroativamente no buffer
    int h = fighter.inputBufferIndex; // Proxima posicao de insercao
    
    // Limpar os botoes do frame anterior para garantir o justPressed no frame atual
    int prevIdx = (h - 1 + Fighter::INPUT_BUFFER_SIZE) % Fighter::INPUT_BUFFER_SIZE;
    fighter.inputBuffer[prevIdx].buttons = 0;
    
    // Injetar direcoes anteriores da sequencia (do penultimo ao primeiro)
    for (size_t i = 1; i < seq.size(); ++i) {
        size_t seqIdx = seq.size() - 1 - i;
        int bufIdx = (h - static_cast<int>(i) + Fighter::INPUT_BUFFER_SIZE) % Fighter::INPUT_BUFFER_SIZE;
        
        FighterDirection dir = seq[seqIdx];
        if (fighter.facing == -1) {
            dir = MirrorDirection(dir);
        }
        fighter.inputBuffer[bufIdx].direction = dir;
        fighter.inputBuffer[bufIdx].buttons = 0;
    }
}

void FightingSystem::UpdateFighterState(Fighter& fighter, float dt) {
    (void)dt;
    
    if (fighter.state == FighterStateType::WIN || fighter.state == FighterStateType::KO) {
        return;
    }
    
    if (fighter.state == FighterStateType::INTRO) {
        if (m_match.phase == MatchPhase::FIGHTING) {
            TransitionTo(fighter, FighterStateType::IDLE);
        }
        return;
    }
    
    if (fighter.state == FighterStateType::HITSTUN_STAND || fighter.state == FighterStateType::HITSTUN_CROUCH ||
        fighter.state == FighterStateType::BLOCKSTUN_STAND || fighter.state == FighterStateType::BLOCKSTUN_CROUCH) {
        fighter.stunFramesRemaining--;
        if (fighter.stunFramesRemaining <= 0) {
            TransitionTo(fighter, FighterStateType::IDLE);
            if (fighter.playerIndex == 0) m_match.comboCountP2 = 0;
            else m_match.comboCountP1 = 0;
        }
        return;
    }
    
    if (fighter.state == FighterStateType::KNOCKDOWN) {
        fighter.knockdownFramesRemaining--;
        if (fighter.knockdownFramesRemaining <= 0) {
            TransitionTo(fighter, FighterStateType::GETUP);
            fighter.getupFrames = 20;
        }
        return;
    }
    
    if (fighter.state == FighterStateType::GETUP) {
        fighter.getupFrames--;
        if (fighter.getupFrames <= 0) {
            TransitionTo(fighter, FighterStateType::IDLE);
            if (fighter.playerIndex == 0) m_match.comboCountP2 = 0;
            else m_match.comboCountP1 = 0;
        }
        return;
    }
    
    if (fighter.state == FighterStateType::JUMP_LAND) {
        if (fighter.stateFrame >= 4) {
            TransitionTo(fighter, FighterStateType::IDLE);
        }
        return;
    }
    
    // 1. CHECAGEM DE CANCELS (SUPERS E GOLPES ESPECIAIS)
    bool isNormalAttack = (fighter.state == FighterStateType::ATTACK && fighter.currentAttackIndex != 99);
    bool isSpecialAttack = (fighter.state == FighterStateType::SPECIAL && !fighter.isSuper);
    bool canCancelNormal = isNormalAttack && fighter.attackHasHit;
    bool canCancelSpecial = isSpecialAttack && fighter.attackHasHit;

    if (CanAct(fighter) || canCancelNormal || canCancelSpecial) {
        // 1a. Check Level 3 Super Ultimates
        for (const auto& spec : fighter.def.specials) {
            bool isSuperMove = (spec.name.rfind("Super ", 0) == 0);
            if (isSuperMove) {
                if (CheckCommandInput(fighter, spec)) {
                    if (fighter.superMeter >= 3.0f) {
                        fighter.superMeter -= 3.0f;
                        fighter.isSuper = true;
                        fighter.isEX = false;
                        
                        fighter.currentAttack = &spec.attack;
                        fighter.currentAttackIndex = -1;
                        fighter.attackHasHit = false;
                        TransitionTo(fighter, FighterStateType::SPECIAL);
                        
                        fighter.isCancelFlash = true;
                        fighter.cancelFlashFrames = 15;
                        PlaySynthesizedSound(7);
                        
                        // Executar logica especifica do Super
                        if (spec.name == "Super Fireball") {
                            Projectile proj;
                            proj.ownerIndex = fighter.playerIndex;
                            proj.position = fighter.position + glm::vec2(30.0f * static_cast<float>(fighter.facing), -60.0f);
                            proj.size = {40.0f, 40.0f};
                            proj.velocity = glm::vec2(8.5f * static_cast<float>(fighter.facing), 0.0f);
                            proj.damage = spec.attack.damage;
                            proj.hitstun = spec.attack.hitstun;
                            proj.blockstun = spec.attack.blockstun;
                            proj.knockbackX = spec.attack.knockbackX;
                            proj.knockbackY = spec.attack.knockbackY;
                            proj.facing = fighter.facing;
                            proj.active = true;
                            m_projectiles.push_back(proj);
                            PlaySynthesizedSound(4);
                            PlaySynthesizedSound(5);
                        } else if (spec.name == "Super SPD") {
                            PlaySynthesizedSound(5);
                            PlaySynthesizedSound(3);
                        } else if (spec.name == "Super Rin") {
                            PlaySynthesizedSound(6);
                            PlaySynthesizedSound(5);
                        }
                        
                        // Congelamento de tela de 30 frames
                        m_match.hitPauseFrames = 30;
                        return;
                    }
                }
            }
        }
    }

    if (CanAct(fighter) || canCancelNormal) {
        // 1b. Check Normal and EX Specials
        for (const auto& spec : fighter.def.specials) {
            bool isSuperMove = (spec.name.rfind("Super ", 0) == 0);
            if (isSuperMove) continue;
            
            if (CheckCommandInput(fighter, spec)) {
                // Checar se o comando EX foi ativado
                int punchCount = 0;
                if (fighter.currentInput.HasButton(FighterButton::LP)) punchCount++;
                if (fighter.currentInput.HasButton(FighterButton::MP)) punchCount++;
                if (fighter.currentInput.HasButton(FighterButton::HP)) punchCount++;
                
                int kickCount = 0;
                if (fighter.currentInput.HasButton(FighterButton::LK)) kickCount++;
                if (fighter.currentInput.HasButton(FighterButton::MK)) kickCount++;
                if (fighter.currentInput.HasButton(FighterButton::HK)) kickCount++;
                
                bool isEXAttempt = false;
                bool matchPunch = (spec.buttonMask & 0x01) != 0;
                bool matchKick = (spec.buttonMask & 0x02) != 0;
                
                if (matchPunch && punchCount >= 2) isEXAttempt = true;
                if (matchKick && kickCount >= 2) isEXAttempt = true;
                
                bool activatedEX = false;
                if (isEXAttempt && fighter.superMeter >= 1.0f) {
                    fighter.superMeter -= 1.0f;
                    fighter.isEX = true;
                    fighter.isSuper = false;
                    activatedEX = true;
                } else {
                    fighter.isEX = false;
                    fighter.isSuper = false;
                    if (!canCancelNormal) {
                        fighter.superMeter = std::min(3.0f, fighter.superMeter + 0.03f);
                    }
                }
                
                fighter.currentAttack = &spec.attack;
                fighter.currentAttackIndex = -1;
                fighter.attackHasHit = false;
                TransitionTo(fighter, FighterStateType::SPECIAL);
                
                fighter.isCancelFlash = true;
                fighter.cancelFlashFrames = 15;
                PlaySynthesizedSound(7);
                
                if (spec.name == "Fireball") {
                    Projectile proj;
                    proj.ownerIndex = fighter.playerIndex;
                    proj.position = fighter.position + glm::vec2(30.0f * static_cast<float>(fighter.facing), -60.0f);
                    
                    if (activatedEX) {
                        proj.size = {30.0f, 30.0f};
                        proj.velocity = glm::vec2(8.5f * static_cast<float>(fighter.facing), 0.0f);
                        proj.damage = static_cast<int>(spec.attack.damage * 1.3f);
                        proj.hitstun = spec.attack.hitstun + 5;
                        proj.blockstun = spec.attack.blockstun + 5;
                    } else {
                        proj.size = {25.0f, 25.0f};
                        proj.velocity = glm::vec2(6.5f * static_cast<float>(fighter.facing), 0.0f);
                        proj.damage = spec.attack.damage;
                        proj.hitstun = spec.attack.hitstun;
                        proj.blockstun = spec.attack.blockstun;
                    }
                    proj.knockbackX = spec.attack.knockbackX;
                    proj.knockbackY = spec.attack.knockbackY;
                    proj.facing = fighter.facing;
                    proj.active = true;
                    m_projectiles.push_back(proj);
                    PlaySynthesizedSound(4);
                    if (activatedEX) PlaySynthesizedSound(5);
                } else if (spec.name == "Dragon Punch" || spec.name == "Hurricane Kick" || spec.name == "Spinning Pile Driver") {
                    PlaySynthesizedSound(3);
                    if (activatedEX) PlaySynthesizedSound(5);
                } else if (spec.name == "Fujin Slash") {
                    PlaySynthesizedSound(6);
                    if (activatedEX) PlaySynthesizedSound(5);
                } else if (spec.name == "Gale Teleport") {
                    PlaySynthesizedSound(3);
                    if (activatedEX) PlaySynthesizedSound(5);
                }
                
                return;
            }
        }
    }
    
    // 2. ATUALIZAÇÃO DO ATAQUE OU ESPECIAL EM CURSO
    if (fighter.state == FighterStateType::ATTACK || fighter.state == FighterStateType::SPECIAL) {
        if (fighter.state == FighterStateType::SPECIAL && fighter.currentAttack) {
            if (fighter.currentAttack->name == "Shoryuken") {
                if (fighter.stateFrame == 3) {
                    float velY = fighter.isEX ? -15.5f : -12.5f;
                    float velX = fighter.isEX ? 3.5f : 2.5f;
                    fighter.velocity.y = velY;
                    fighter.velocity.x = velX * static_cast<float>(fighter.facing);
                    fighter.isGrounded = false;
                    PlaySynthesizedSound(3);
                    if (fighter.isEX) PlaySynthesizedSound(5);
                }
            } else if (fighter.currentAttack->name == "Tatsumaki") {
                if (fighter.stateFrame >= 8 && fighter.stateFrame < 20) {
                    float speed = fighter.isEX ? 5.2f : 3.8f;
                    fighter.velocity.x = speed * static_cast<float>(fighter.facing);
                }
            } else if (fighter.currentAttack->name == "Gale Teleport") {
                if (fighter.stateFrame >= 1 && fighter.stateFrame <= 12) {
                    float speed = fighter.isEX ? 14.0f : 9.0f;
                    fighter.velocity.x = speed * static_cast<float>(fighter.facing);
                }
            } else if (fighter.currentAttack->name == "Fujin Slash") {
                if (fighter.stateFrame >= 1 && fighter.stateFrame <= 10) {
                    float speed = fighter.isEX ? 4.5f : 3.0f;
                    fighter.velocity.x = speed * static_cast<float>(fighter.facing);
                }
            }
        }
        
        if (fighter.stateFrame >= fighter.currentAttack->TotalFrames()) {
            fighter.currentAttack = nullptr;
            fighter.currentAttackIndex = -1;
            TransitionTo(fighter, fighter.isGrounded ? FighterStateType::IDLE : FighterStateType::JUMP);
        }
        return;
    }
    
    // 3. COMANDOS NORMAIS (FREE STATE)
    if (CanAct(fighter)) {
        // 2. Normal Throw (LP + LK)
        bool holdsLP = fighter.currentInput.HasButton(FighterButton::LP);
        bool holdsLK = fighter.currentInput.HasButton(FighterButton::LK);
        if (holdsLP && holdsLK) {
            fighter.currentAttack = &fighter.def.throwAttack;
            fighter.currentAttackIndex = 99;
            fighter.attackHasHit = false;
            TransitionTo(fighter, FighterStateType::ATTACK);
            return;
        }
        
        // 3. Normals
        for (int b = 0; b < static_cast<int>(FighterButton::COUNT); ++b) {
            FighterButton btn = static_cast<FighterButton>(b);
            if (fighter.currentInput.HasButton(btn)) {
                int base = fighter.isGrounded ? (fighter.isCrouching ? 6 : 0) : 12;
                int index = base + b;
                fighter.currentAttack = &fighter.def.normals[index];
                fighter.currentAttackIndex = index;
                fighter.attackHasHit = false;
                TransitionTo(fighter, FighterStateType::ATTACK);
                fighter.superMeter = std::min(3.0f, fighter.superMeter + 0.03f);
                
                PlaySynthesizedSound(6);
                return;
            }
        }
        
        // 4. Movement directions
        FighterDirection dir = fighter.currentInput.direction;
        if (fighter.isGrounded) {
            if (dir == FighterDirection::UP || dir == FighterDirection::UP_LEFT || dir == FighterDirection::UP_RIGHT) {
                TransitionTo(fighter, FighterStateType::JUMP);
                fighter.isGrounded = false;
                fighter.velocity.y = -13.0f;
                
                if (dir == FighterDirection::UP_LEFT) {
                    fighter.velocity.x = -fighter.def.jumpForwardSpeed;
                } else if (dir == FighterDirection::UP_RIGHT) {
                    fighter.velocity.x = fighter.def.jumpForwardSpeed;
                } else {
                    fighter.velocity.x = 0.0f;
                }
                PlaySynthesizedSound(3);
            } else if (dir == FighterDirection::DOWN || dir == FighterDirection::DOWN_LEFT || dir == FighterDirection::DOWN_RIGHT) {
                if (fighter.state != FighterStateType::CROUCH) {
                    TransitionTo(fighter, FighterStateType::CROUCH);
                }
            } else if (dir == FighterDirection::LEFT) {
                if (fighter.facing == 1) {
                    TransitionTo(fighter, FighterStateType::WALK_BACKWARD);
                    fighter.velocity.x = -fighter.def.backWalkSpeed;
                } else {
                    TransitionTo(fighter, FighterStateType::WALK_FORWARD);
                    fighter.velocity.x = -fighter.def.walkSpeed;
                }
            } else if (dir == FighterDirection::RIGHT) {
                if (fighter.facing == 1) {
                    TransitionTo(fighter, FighterStateType::WALK_FORWARD);
                    fighter.velocity.x = fighter.def.walkSpeed;
                } else {
                    TransitionTo(fighter, FighterStateType::WALK_BACKWARD);
                    fighter.velocity.x = fighter.def.backWalkSpeed;
                }
            } else {
                if (fighter.state != FighterStateType::IDLE) {
                    TransitionTo(fighter, FighterStateType::IDLE);
                }
                fighter.velocity.x = 0.0f;
            }
        } else {
            // Aerial actions: Double Jump
            int headIndex = (fighter.inputBufferIndex - 1 + Fighter::INPUT_BUFFER_SIZE) % Fighter::INPUT_BUFFER_SIZE;
            int prevIndex = (headIndex - 1 + Fighter::INPUT_BUFFER_SIZE) % Fighter::INPUT_BUFFER_SIZE;
            FighterDirection currDir = fighter.inputBuffer[headIndex].direction;
            FighterDirection prevDir = fighter.inputBuffer[prevIndex].direction;
            
            bool currJump = (currDir == FighterDirection::UP || currDir == FighterDirection::UP_LEFT || currDir == FighterDirection::UP_RIGHT);
            bool prevJump = (prevDir == FighterDirection::UP || prevDir == FighterDirection::UP_LEFT || prevDir == FighterDirection::UP_RIGHT);
            
            if (currJump && !prevJump && fighter.jumpsRemaining > 0) {
                fighter.jumpsRemaining--;
                TransitionTo(fighter, FighterStateType::JUMP);
                fighter.velocity.y = -13.0f;
                
                if (currDir == FighterDirection::UP_LEFT) {
                    fighter.velocity.x = -fighter.def.jumpForwardSpeed;
                } else if (currDir == FighterDirection::UP_RIGHT) {
                    fighter.velocity.x = fighter.def.jumpForwardSpeed;
                } else {
                    fighter.velocity.x = 0.0f;
                }
                PlaySynthesizedSound(3);
            }
        }
    }
}

void FightingSystem::ApplyPhysics(Fighter& fighter, float dt) {
    (void)dt;
    
    fighter.position += fighter.velocity;
    
    if (!fighter.isGrounded) {
        float gravity = 0.65f;
        fighter.velocity.y += gravity;
        
        if (fighter.position.y >= fighter.groundY) {
            fighter.position.y = fighter.groundY;
            fighter.velocity = {0.0f, 0.0f};
            fighter.isGrounded = true;
            fighter.jumpsRemaining = fighter.def.maxJumps;
            fighter.juggleHits = 0;
            if (fighter.state == FighterStateType::JUMP) {
                TransitionTo(fighter, FighterStateType::JUMP_LAND);
            }
        }
    } else {
        fighter.velocity.x *= 0.82f;
    }
    
    if (fighter.position.x < m_arenaLeft) {
        fighter.position.x = m_arenaLeft;
    }
    if (fighter.position.x > m_arenaRight) {
        fighter.position.x = m_arenaRight;
    }
}

void FightingSystem::ResolvePushBoxes() {
    FightBox box1 = m_p1.GetPushBox();
    FightBox box2 = m_p2.GetPushBox();
    
    glm::vec2 pos1 = m_p1.GetBoxWorldPos(box1);
    glm::vec2 pos2 = m_p2.GetBoxWorldPos(box2);
    
    float halfW1 = box1.size.x * 0.5f;
    float halfW2 = box2.size.x * 0.5f;
    
    float left1 = pos1.x - halfW1;
    float right1 = pos1.x + halfW1;
    float left2 = pos2.x - halfW2;
    float right2 = pos2.x + halfW2;
    
    float top1 = pos1.y - box1.size.y * 0.5f;
    float bottom1 = pos1.y + box1.size.y * 0.5f;
    float top2 = pos2.y - box2.size.y * 0.5f;
    float bottom2 = pos2.y + box2.size.y * 0.5f;
    
    bool yOverlap = (bottom1 > top2) && (bottom2 > top1);
    bool xOverlap = (right1 > left2) && (right2 > left1);
    
    if (xOverlap && yOverlap) {
        float overlapX = 0.0f;
        if (pos1.x < pos2.x) {
            overlapX = right1 - left2;
            
            // P1 vai para a esquerda, P2 para a direita
            float maxLeft1 = m_p1.position.x - m_arenaLeft;
            float maxRight2 = m_arenaRight - m_p2.position.x;
            
            float move1 = overlapX * 0.5f;
            float move2 = overlapX * 0.5f;
            
            if (move1 > maxLeft1) {
                float extra = move1 - maxLeft1;
                move1 = maxLeft1;
                move2 += extra;
            }
            if (move2 > maxRight2) {
                float extra = move2 - maxRight2;
                move2 = maxRight2;
                move1 = std::min(move1 + extra, maxLeft1);
            }
            
            m_p1.position.x -= move1;
            m_p2.position.x += move2;
        } else {
            overlapX = right2 - left1;
            
            // P1 vai para a direita, P2 para a esquerda
            float maxRight1 = m_arenaRight - m_p1.position.x;
            float maxLeft2 = m_p2.position.x - m_arenaLeft;
            
            float move1 = overlapX * 0.5f;
            float move2 = overlapX * 0.5f;
            
            if (move1 > maxRight1) {
                float extra = move1 - maxRight1;
                move1 = maxRight1;
                move2 += extra;
            }
            if (move2 > maxLeft2) {
                float extra = move2 - maxLeft2;
                move2 = maxLeft2;
                move1 = std::min(move1 + extra, maxRight1);
            }
            
            m_p1.position.x += move1;
            m_p2.position.x -= move2;
        }
        
        m_p1.position.x = std::clamp(m_p1.position.x, m_arenaLeft, m_arenaRight);
        m_p2.position.x = std::clamp(m_p2.position.x, m_arenaLeft, m_arenaRight);
    }
}

void FightingSystem::CheckHitCollisions() {
    // 1. P1 attack vs P2 hurtbox
    if (m_p1.currentAttack && !m_p1.attackHasHit && !IsInvincible(m_p2)) {
        FightBox hitbox = m_p1.GetHitBox();
        if (hitbox.active) {
            FightBox hurtbox = m_p2.GetHurtBox();
            if (BoxOverlap(m_p1.GetBoxWorldPos(hitbox), hitbox.size, m_p2.GetBoxWorldPos(hurtbox), hurtbox.size)) {
                if (m_p1.currentAttackIndex == 99 || m_p1.currentAttack->name == "SPD") {
                    CheckThrow(m_p1, m_p2);
                } else {
                    ApplyHit(m_p1, m_p2, *m_p1.currentAttack);
                    m_p1.attackHasHit = true;
                }
            }
        }
    }
    
    // 2. P2 attack vs P1 hurtbox
    if (m_p2.currentAttack && !m_p2.attackHasHit && !IsInvincible(m_p1)) {
        FightBox hitbox = m_p2.GetHitBox();
        if (hitbox.active) {
            FightBox hurtbox = m_p1.GetHurtBox();
            if (BoxOverlap(m_p2.GetBoxWorldPos(hitbox), hitbox.size, m_p1.GetBoxWorldPos(hurtbox), hurtbox.size)) {
                if (m_p2.currentAttackIndex == 99 || m_p2.currentAttack->name == "SPD") {
                    CheckThrow(m_p2, m_p1);
                } else {
                    ApplyHit(m_p2, m_p1, *m_p2.currentAttack);
                    m_p2.attackHasHit = true;
                }
            }
        }
    }
    
    // 3. Projectiles vs P1/P2 hurtbox
    for (auto& proj : m_projectiles) {
        if (!proj.active) continue;
        
        if (proj.ownerIndex == 0) {
            if (!IsInvincible(m_p2)) {
                FightBox hurtbox = m_p2.GetHurtBox();
                if (BoxOverlap(proj.position, proj.size, m_p2.GetBoxWorldPos(hurtbox), hurtbox.size)) {
                    proj.active = false;
                    ApplyHit(m_p1, m_p2, AttackData{ "Hadouken Projectile", 0, 0, 0, proj.damage, proj.hitstun, proj.blockstun, proj.knockbackX, proj.knockbackY, false, false, {0,0}, {0,0} });
                }
            }
        } else if (proj.ownerIndex == 1) {
            if (!IsInvincible(m_p1)) {
                FightBox hurtbox = m_p1.GetHurtBox();
                if (BoxOverlap(proj.position, proj.size, m_p1.GetBoxWorldPos(hurtbox), hurtbox.size)) {
                    proj.active = false;
                    ApplyHit(m_p2, m_p1, AttackData{ "Hadouken Projectile", 0, 0, 0, proj.damage, proj.hitstun, proj.blockstun, proj.knockbackX, proj.knockbackY, false, false, {0,0}, {0,0} });
                }
            }
        }
    }
}

void FightingSystem::ApplyHit(Fighter& attacker, Fighter& defender, const AttackData& attack) {
    // ─── PARRY CHECK ─────────────────────────────────────────────────────────
    bool parried = false;
    
    // CPU procedural parry
    if (defender.playerIndex != 0 && m_match.aiDifficulty >= 0) {
        int parryChance = 0;
        if (m_match.aiDifficulty == 1) parryChance = 5;       // 5% chance
        else if (m_match.aiDifficulty == 2) parryChance = 15;  // 15% chance
        
        if (parryChance > 0 && (rand() % 100) < parryChance) {
            parried = true;
        }
    } else {
        // Player manual parry check
        if (defender.state == FighterStateType::IDLE ||
            defender.state == FighterStateType::WALK_FORWARD ||
            defender.state == FighterStateType::WALK_BACKWARD ||
            defender.state == FighterStateType::CROUCH ||
            defender.state == FighterStateType::JUMP) {
            
            if (attack.isLow) {
                if (HasTappedDown(defender)) {
                    parried = true;
                }
            } else {
                if (HasTappedForward(defender)) {
                    parried = true;
                }
            }
        }
    }
    
    if (parried) {
        defender.isParryFlash = true;
        defender.parryFlashFrames = 15;
        
        // General screen freeze frames (Parry Screen Freeze)
        m_match.hitPauseFrames = 12;
        
        // Reward: Charge super meter by 0.15f (5% of 3 bars)
        defender.superMeter = std::min(3.0f, defender.superMeter + 0.15f);
        
        // Reset attacker combo
        attacker.comboCount = 0;
        attacker.comboDamage = 0;
        
        // Play high metallic synthwave sound
        if (Engine::IsInitialized()) {
            auto* audio = Engine::Get().GetSystem<AudioSystem>();
            if (audio) {
                audio->PlayNote(780.0f, 0.12f, WaveType::Sine);
                audio->PlayNote(390.0f, 0.08f, WaveType::Triangle);
            }
        }
        
        return; // Complete negation of hit: no damage, no blockstun, no hitstun!
    }
    
    bool blocked = false;
    if (CanBlock(defender) && IsHoldingBack(defender)) {
        if (attack.isOverhead && defender.isCrouching) {
            blocked = false;
        } else if (attack.isLow && !defender.isCrouching) {
            blocked = false;
        } else {
            blocked = true;
        }
    }
    
    if (blocked) {
        defender.stunFramesRemaining = attack.blockstun;
        TransitionTo(defender, defender.isCrouching ? FighterStateType::BLOCKSTUN_CROUCH : FighterStateType::BLOCKSTUN_STAND);
        
        int chip = attack.damage / 10;
        defender.currentHealth = std::max(0, defender.currentHealth - chip);
        
        float kbX = attack.knockbackX * 0.4f;
        bool defenderAtWall = (defender.position.x <= m_arenaLeft + 5.0f && attacker.facing == -1) ||
                              (defender.position.x >= m_arenaRight - 5.0f && attacker.facing == 1);
        if (defenderAtWall) {
            attacker.velocity.x = kbX * -static_cast<float>(attacker.facing);
            defender.velocity.x = 0.0f;
        } else {
            defender.velocity.x = kbX * -static_cast<float>(defender.facing);
        }
        
        attacker.comboCount = 0;
        attacker.comboDamage = 0;
        
        // Super Meter charging on block
        defender.superMeter = std::min(3.0f, defender.superMeter + 0.06f); // +2% of 3 bars (0.06)
        attacker.superMeter = std::min(3.0f, attacker.superMeter + 0.03f); // +1% of 3 bars (0.03)
        
        PlaySynthesizedSound(2);
    } else {
        // Check for Counter Hit
        bool isCounter = false;
        if (defender.state == FighterStateType::ATTACK || defender.state == FighterStateType::SPECIAL) {
            if (defender.currentAttack && defender.stateFrame < defender.currentAttack->startup) {
                isCounter = true;
            }
        }
        
        int finalDamage = attack.damage;
        int finalHitstun = attack.hitstun;
        
        if (attacker.isEX) {
            finalDamage = static_cast<int>(finalDamage * 1.3f);
            finalHitstun += 5;
        }
        
        if (isCounter) {
            finalDamage = static_cast<int>(finalDamage * 1.25f);
            finalHitstun += 5;
            defender.isCounterFlash = true;
            PlaySynthesizedSound(5); // extra counter hit impact sound
        }
        
        defender.stunFramesRemaining = finalHitstun;
        defender.currentHealth = std::max(0, defender.currentHealth - finalDamage);
        
        attacker.comboCount++;
        attacker.comboDamage += finalDamage;
        if (defender.playerIndex == 0) {
            m_match.comboCountP2 = attacker.comboCount;
        } else {
            m_match.comboCountP1 = attacker.comboCount;
        }
        
        // Super Meter charging on hit
        if (isCounter) {
            defender.superMeter = std::min(3.0f, defender.superMeter + 0.21f); // +7% of 3 bars (0.21)
            attacker.superMeter = std::min(3.0f, attacker.superMeter + 0.15f); // +5% of 3 bars (0.15)
        } else {
            defender.superMeter = std::min(3.0f, defender.superMeter + 0.15f); // +5% of 3 bars (0.15)
            attacker.superMeter = std::min(3.0f, attacker.superMeter + 0.09f); // +3% of 3 bars (0.09)
        }
        
        bool defenderAtWall = (defender.position.x <= m_arenaLeft + 5.0f && attacker.facing == -1) ||
                              (defender.position.x >= m_arenaRight - 5.0f && attacker.facing == 1);
                              
        bool isAirJuggle = !defender.isGrounded;
        if (isAirJuggle) {
            defender.juggleHits++;
        }
                              
        if (attack.knockbackY > 0.0f || attack.name == "Crouch HK" || attack.name == "Tatsumaki" || attack.name == "Shoryuken" || isAirJuggle) {
            if (defenderAtWall) {
                attacker.velocity.x = attack.knockbackX * -static_cast<float>(attacker.facing);
                defender.velocity.x = 0.0f;
            } else {
                defender.velocity.x = attack.knockbackX * -static_cast<float>(defender.facing);
            }
            
            float targetKbY = attack.knockbackY;
            if (isAirJuggle && targetKbY <= 0.0f) {
                targetKbY = 3.0f; // Impulso vertical padrão para manter no ar
            }
            
            // Reduz o ganho de altura em hits sucessivos para limitar o juggle
            if (isAirJuggle && defender.juggleHits > 1) {
                targetKbY *= 0.6f;
            }
            
            defender.velocity.y = -targetKbY;
            defender.isGrounded = false;
            TransitionTo(defender, FighterStateType::KNOCKDOWN);
            defender.knockdownFramesRemaining = 50;
        } else {
            if (defenderAtWall) {
                attacker.velocity.x = attack.knockbackX * -static_cast<float>(attacker.facing);
                defender.velocity.x = 0.0f;
            } else {
                defender.velocity.x = attack.knockbackX * -static_cast<float>(defender.facing);
            }
            TransitionTo(defender, defender.isCrouching ? FighterStateType::HITSTUN_CROUCH : FighterStateType::HITSTUN_STAND);
        }
        
        m_match.hitPauseFrames = (finalDamage > 100) ? 10 : 6;
        
        PlaySynthesizedSound(1);
        if (finalDamage > 100) {
            PlaySynthesizedSound(5);
        }
    }
}

void FightingSystem::CheckThrow(Fighter& attacker, Fighter& defender) {
    if (defender.isGrounded && defender.state != FighterStateType::HITSTUN_STAND &&
        defender.state != FighterStateType::HITSTUN_CROUCH && defender.state != FighterStateType::BLOCKSTUN_STAND &&
        defender.state != FighterStateType::BLOCKSTUN_CROUCH && defender.state != FighterStateType::KNOCKDOWN &&
        defender.state != FighterStateType::GETUP) {
        
        attacker.attackHasHit = true;
        
        int dmg = attacker.currentAttack->damage;
        defender.currentHealth = std::max(0, defender.currentHealth - dmg);
        
        float kbX = attacker.currentAttack->knockbackX * static_cast<float>(attacker.facing);
        float kbY = attacker.currentAttack->knockbackY;
        
        defender.velocity = {kbX, -kbY};
        defender.isGrounded = false;
        
        TransitionTo(defender, FighterStateType::KNOCKDOWN);
        defender.knockdownFramesRemaining = 60;
        
        attacker.comboCount++;
        attacker.comboDamage += dmg;
        if (defender.playerIndex == 0) m_match.comboCountP2 = attacker.comboCount;
        else m_match.comboCountP1 = attacker.comboCount;
        
        m_match.hitPauseFrames = 12;
        PlaySynthesizedSound(5);
    }
}

void FightingSystem::TransitionTo(Fighter& fighter, FighterStateType newState) {
    fighter.state = newState;
    fighter.stateFrame = 0;
    
    if (newState != FighterStateType::SPECIAL) {
        fighter.isEX = false;
        fighter.isSuper = false;
    }
    if (newState != FighterStateType::HITSTUN_STAND && newState != FighterStateType::HITSTUN_CROUCH && newState != FighterStateType::KNOCKDOWN) {
        fighter.isCounterFlash = false;
    }
    
    if (newState == FighterStateType::CROUCH || newState == FighterStateType::CROUCH_TRANSITION) {
        fighter.isCrouching = true;
    } else if (newState != FighterStateType::ATTACK && newState != FighterStateType::SPECIAL && 
               newState != FighterStateType::BLOCKSTUN_CROUCH && newState != FighterStateType::HITSTUN_CROUCH) {
        if (newState == FighterStateType::IDLE || newState == FighterStateType::WALK_FORWARD || 
            newState == FighterStateType::WALK_BACKWARD || newState == FighterStateType::JUMP) {
            fighter.isCrouching = false;
        }
    }
    
    if (newState == FighterStateType::HITSTUN_STAND || newState == FighterStateType::HITSTUN_CROUCH ||
        newState == FighterStateType::BLOCKSTUN_STAND || newState == FighterStateType::BLOCKSTUN_CROUCH ||
        newState == FighterStateType::KNOCKDOWN) {
        fighter.comboCount = 0;
        fighter.comboDamage = 0;
        if (fighter.playerIndex == 0) {
            m_match.comboCountP2 = 0;
        } else {
            m_match.comboCountP1 = 0;
        }
    }
}

bool FightingSystem::CanAct(const Fighter& fighter) const {
    return fighter.state == FighterStateType::IDLE ||
           fighter.state == FighterStateType::WALK_FORWARD ||
           fighter.state == FighterStateType::WALK_BACKWARD ||
           fighter.state == FighterStateType::CROUCH ||
           fighter.state == FighterStateType::JUMP;
}

bool FightingSystem::CanBlock(const Fighter& fighter) const {
    return fighter.state == FighterStateType::IDLE ||
           fighter.state == FighterStateType::WALK_FORWARD ||
           fighter.state == FighterStateType::WALK_BACKWARD ||
           fighter.state == FighterStateType::CROUCH;
}

bool FightingSystem::IsInHitstun(const Fighter& fighter) const {
    return fighter.state == FighterStateType::HITSTUN_STAND ||
           fighter.state == FighterStateType::HITSTUN_CROUCH ||
           fighter.state == FighterStateType::KNOCKDOWN;
}

bool FightingSystem::IsInBlockstun(const Fighter& fighter) const {
    return fighter.state == FighterStateType::BLOCKSTUN_STAND ||
           fighter.state == FighterStateType::BLOCKSTUN_CROUCH;
}

bool FightingSystem::IsAttacking(const Fighter& fighter) const {
    return fighter.state == FighterStateType::ATTACK ||
           fighter.state == FighterStateType::SPECIAL;
}

bool FightingSystem::IsInvincible(const Fighter& fighter) const {
    if (fighter.state == FighterStateType::GETUP ||
        fighter.state == FighterStateType::INTRO ||
        fighter.state == FighterStateType::KO ||
        fighter.state == FighterStateType::WIN) {
        return true;
    }
    if (fighter.state == FighterStateType::KNOCKDOWN) {
        if (fighter.isGrounded) {
            return true;
        } else if (fighter.juggleHits >= 3) {
            return true;
        }
    }
    if (fighter.state == FighterStateType::SPECIAL && fighter.currentAttack) {
        if (fighter.currentAttack->name == "Gale Teleport") {
            if (fighter.stateFrame >= 0 && fighter.stateFrame <= 15) {
                return true;
            }
        }
        if (fighter.currentAttack->name == "Shoryuken") {
            if (fighter.stateFrame < 6) {
                return true;
            }
        }
    }
    return false;
}

FighterDirection FightingSystem::MirrorDirection(FighterDirection dir) const {
    switch (dir) {
        case FighterDirection::LEFT: return FighterDirection::RIGHT;
        case FighterDirection::RIGHT: return FighterDirection::LEFT;
        case FighterDirection::UP_LEFT: return FighterDirection::UP_RIGHT;
        case FighterDirection::UP_RIGHT: return FighterDirection::UP_LEFT;
        case FighterDirection::DOWN_LEFT: return FighterDirection::DOWN_RIGHT;
        case FighterDirection::DOWN_RIGHT: return FighterDirection::DOWN_LEFT;
        default: return dir;
    }
}

bool FightingSystem::CheckCommandInput(const Fighter& fighter, const CommandInput& cmd) const {
    int headIndex = (fighter.inputBufferIndex - 1 + Fighter::INPUT_BUFFER_SIZE) % Fighter::INPUT_BUFFER_SIZE;
    int prevIndex = (headIndex - 1 + Fighter::INPUT_BUFFER_SIZE) % Fighter::INPUT_BUFFER_SIZE;
    
    uint8_t headButtons = fighter.inputBuffer[headIndex].buttons;
    uint8_t prevButtons = fighter.inputBuffer[prevIndex].buttons;
    uint8_t justPressed = headButtons & ~prevButtons;
    
    bool buttonPressed = false;
    bool matchPunch = (cmd.buttonMask & 0x01) != 0;
    bool matchKick = (cmd.buttonMask & 0x02) != 0;
    
    if (matchPunch) {
        if ((justPressed & (1 << static_cast<uint8_t>(FighterButton::LP))) ||
            (justPressed & (1 << static_cast<uint8_t>(FighterButton::MP))) ||
            (justPressed & (1 << static_cast<uint8_t>(FighterButton::HP)))) {
            buttonPressed = true;
        }
    }
    if (matchKick) {
        if ((justPressed & (1 << static_cast<uint8_t>(FighterButton::LK))) ||
            (justPressed & (1 << static_cast<uint8_t>(FighterButton::MK))) ||
            (justPressed & (1 << static_cast<uint8_t>(FighterButton::HK)))) {
            buttonPressed = true;
        }
    }
    
    if (!buttonPressed) {
        return false;
    }
    
    int seqIdx = static_cast<int>(cmd.sequence.size()) - 1;
    if (seqIdx < 0) return true;
    
    int currentSearchIdx = headIndex;
    int framesSearched = 0;
    
    while (framesSearched < cmd.windowFrames) {
        FighterDirection absDir = fighter.inputBuffer[currentSearchIdx].direction;
        FighterDirection relDir = absDir;
        if (fighter.facing == -1) {
            relDir = MirrorDirection(absDir);
        }
        
        if (relDir == cmd.sequence[seqIdx]) {
            seqIdx--;
            if (seqIdx < 0) {
                return true;
            }
        }
        
        currentSearchIdx = (currentSearchIdx - 1 + Fighter::INPUT_BUFFER_SIZE) % Fighter::INPUT_BUFFER_SIZE;
        framesSearched++;
    }
    
    return false;
}

FighterDirection FightingSystem::GetDirectionFromInput(const Fighter& fighter) const {
    return fighter.currentInput.direction;
}

bool FightingSystem::IsHoldingBack(const Fighter& fighter) const {
    FighterDirection dir = fighter.currentInput.direction;
    if (fighter.facing == 1) {
        return dir == FighterDirection::LEFT || dir == FighterDirection::DOWN_LEFT || dir == FighterDirection::UP_LEFT;
    } else {
        return dir == FighterDirection::RIGHT || dir == FighterDirection::DOWN_RIGHT || dir == FighterDirection::UP_RIGHT;
    }
}

bool FightingSystem::IsForward(const Fighter& fighter, FighterDirection dir) const {
    if (fighter.facing == 1) {
        return dir == FighterDirection::RIGHT || dir == FighterDirection::UP_RIGHT || dir == FighterDirection::DOWN_RIGHT;
    } else {
        return dir == FighterDirection::LEFT || dir == FighterDirection::UP_LEFT || dir == FighterDirection::DOWN_LEFT;
    }
}

bool FightingSystem::IsDown(FighterDirection dir) const {
    return dir == FighterDirection::DOWN || dir == FighterDirection::DOWN_LEFT || dir == FighterDirection::DOWN_RIGHT;
}

bool FightingSystem::HasTappedForward(const Fighter& fighter) const {
    int head = (fighter.inputBufferIndex - 1 + Fighter::INPUT_BUFFER_SIZE) % Fighter::INPUT_BUFFER_SIZE;
    // Look up to 10 frames back for a rising edge of Forward input
    for (int i = 0; i < 10; ++i) {
        int k = (head - i + Fighter::INPUT_BUFFER_SIZE) % Fighter::INPUT_BUFFER_SIZE;
        int prev_k = (k - 1 + Fighter::INPUT_BUFFER_SIZE) % Fighter::INPUT_BUFFER_SIZE;
        if (IsForward(fighter, fighter.inputBuffer[k].direction) && !IsForward(fighter, fighter.inputBuffer[prev_k].direction)) {
            return true;
        }
    }
    return false;
}

bool FightingSystem::HasTappedDown(const Fighter& fighter) const {
    int head = (fighter.inputBufferIndex - 1 + Fighter::INPUT_BUFFER_SIZE) % Fighter::INPUT_BUFFER_SIZE;
    // Look up to 10 frames back for a rising edge of Down input
    for (int i = 0; i < 10; ++i) {
        int k = (head - i + Fighter::INPUT_BUFFER_SIZE) % Fighter::INPUT_BUFFER_SIZE;
        int prev_k = (k - 1 + Fighter::INPUT_BUFFER_SIZE) % Fighter::INPUT_BUFFER_SIZE;
        if (IsDown(fighter.inputBuffer[k].direction) && !IsDown(fighter.inputBuffer[prev_k].direction)) {
            return true;
        }
    }
    return false;
}

void FightingSystem::UpdateAI(float dt) {
    (void)dt;
}

void FightingSystem::StartRound() {
    m_p1.Reset(-120.0f, m_groundY, 1);
    m_p2.Reset(120.0f, m_groundY, -1);
    
    m_projectiles.clear();
    m_slowMoFrames = 0;
    m_slowMoTick = 0;
    
    m_match.timer = 99.0f;
    m_match.phase = MatchPhase::INTRO;
    m_match.phaseTimer = 2.0f;
    
    PlaySynthesizedSound(0);
}

void FightingSystem::EndRound(int winner) {
    if (winner == 1) {
        m_match.p1Score++;
        m_p1.state = FighterStateType::WIN;
        m_p1.stateFrame = 0;
    } else if (winner == 2) {
        m_match.p2Score++;
        m_p2.state = FighterStateType::WIN;
        m_p2.stateFrame = 0;
    }
    
    bool matchOver = (m_match.p1Score >= m_match.roundsToWin) || (m_match.p2Score >= m_match.roundsToWin);
    if (matchOver) {
        m_match.phase = MatchPhase::MATCH_END;
        m_match.phaseTimer = 4.0f;
    } else {
        m_match.phase = MatchPhase::ROUND_END;
        m_match.phaseTimer = 3.0f;
    }
    
    PlaySynthesizedSound(5);
}

void FightingSystem::UpdateRoundTimer(float dt) {
    if (m_match.phase == MatchPhase::FIGHTING) {
        m_match.timer -= dt * m_match.timerSpeed;
        if (m_match.timer <= 0.0f) {
            m_match.timer = 0.0f;
            if (m_p1.currentHealth > m_p2.currentHealth) {
                EndRound(1);
            } else if (m_p2.currentHealth > m_p1.currentHealth) {
                EndRound(2);
            } else {
                EndRound(0);
            }
        }
    }
}

bool FightingSystem::BoxOverlap(glm::vec2 posA, glm::vec2 sizeA, glm::vec2 posB, glm::vec2 sizeB) const {
    float halfWA = sizeA.x * 0.5f;
    float halfHA = sizeA.y * 0.5f;
    float halfWB = sizeB.x * 0.5f;
    float halfHB = sizeB.y * 0.5f;
    
    float leftA = posA.x - halfWA;
    float rightA = posA.x + halfWA;
    float topA = posA.y - halfHA;
    float bottomA = posA.y + halfHA;
    
    float leftB = posB.x - halfWB;
    float rightB = posB.x + halfWB;
    float topB = posB.y - halfHB;
    float bottomB = posB.y + halfHB;
    
    return (rightA > leftB && leftA < rightB && bottomA > topB && topA < bottomB);
}

void FightingSystem::InitDefaultFighters() {
    RegisterFighterDef(CreateBalancedFighter());
    RegisterFighterDef(CreateGrapplerFighter());
    RegisterFighterDef(CreateSpeedFighter());
}

FighterDef FightingSystem::CreateBalancedFighter() {
    FighterDef def;
    def.name = "KAITO";
    def.walkSpeed = 3.5f;
    def.backWalkSpeed = 2.5f;
    def.jumpHeight = 180.0f;
    def.jumpForwardSpeed = 3.2f;
    def.jumpDuration = 0.5f;
    def.health = 1000;
    def.height = 120.0f;
    def.width = 50.0f;
    def.bodyColor = {0.1f, 0.4f, 0.9f};
    def.accentColor = {0.9f, 0.7f, 0.1f};
    
    def.pushBoxSize = {40.0f, 110.0f};
    def.pushBoxOffset = {0.0f, -55.0f};
    def.hurtBoxStandSize = {45.0f, 110.0f};
    def.hurtBoxStandOffset = {0.0f, -55.0f};
    def.hurtBoxCrouchSize = {50.0f, 70.0f};
    def.hurtBoxCrouchOffset = {0.0f, -35.0f};

    SetNormalAttack(def, 0, "Stand LP", 4, 3, 6, 35, 12, 6, {32.0f, -80.0f}, {35.0f, 20.0f});
    SetNormalAttack(def, 1, "Stand MP", 6, 4, 10, 65, 18, 10, {38.0f, -80.0f}, {40.0f, 22.0f});
    SetNormalAttack(def, 2, "Stand HP", 9, 4, 14, 110, 24, 14, {45.0f, -80.0f}, {45.0f, 25.0f});
    SetNormalAttack(def, 3, "Stand LK", 5, 3, 7, 30, 11, 5, {35.0f, -15.0f}, {35.0f, 18.0f});
    SetNormalAttack(def, 4, "Stand MK", 8, 4, 11, 60, 17, 9, {42.0f, -15.0f}, {40.0f, 20.0f});
    SetNormalAttack(def, 5, "Stand HK", 11, 5, 16, 105, 25, 12, {50.0f, -25.0f}, {48.0f, 24.0f});

    SetNormalAttack(def, 6, "Crouch LP", 4, 3, 6, 30, 11, 5, {30.0f, -40.0f}, {35.0f, 18.0f});
    SetNormalAttack(def, 7, "Crouch MP", 6, 4, 9, 60, 17, 9, {35.0f, -40.0f}, {38.0f, 20.0f});
    SetNormalAttack(def, 8, "Crouch HP", 8, 5, 13, 100, 22, 12, {38.0f, -60.0f}, {40.0f, 30.0f});
    SetNormalAttack(def, 9, "Crouch LK", 5, 3, 6, 25, 10, 5, {32.0f, -10.0f}, {35.0f, 15.0f}, false, true);
    SetNormalAttack(def, 10, "Crouch MK", 7, 4, 10, 55, 16, 8, {38.0f, -10.0f}, {38.0f, 18.0f}, false, true);
    SetNormalAttack(def, 11, "Crouch HK", 10, 5, 18, 95, 22, 10, {45.0f, -10.0f}, {42.0f, 18.0f}, false, true, 4.0f, 6.0f);

    SetNormalAttack(def, 12, "Jump LP", 4, 5, 4, 40, 15, 8, {28.0f, -50.0f}, {30.0f, 20.0f}, true);
    SetNormalAttack(def, 13, "Jump MP", 6, 6, 4, 70, 20, 10, {32.0f, -50.0f}, {35.0f, 22.0f}, true);
    SetNormalAttack(def, 14, "Jump HP", 8, 6, 6, 115, 26, 12, {38.0f, -60.0f}, {40.0f, 28.0f}, true);
    SetNormalAttack(def, 15, "Jump LK", 4, 5, 4, 35, 14, 7, {30.0f, -20.0f}, {30.0f, 18.0f}, true);
    SetNormalAttack(def, 16, "Jump MK", 7, 6, 4, 65, 19, 9, {35.0f, -25.0f}, {35.0f, 20.0f}, true);
    SetNormalAttack(def, 17, "Jump HK", 9, 6, 6, 110, 25, 11, {42.0f, -30.0f}, {40.0f, 24.0f}, true);

    CommandInput fireball;
    fireball.name = "Fireball";
    fireball.sequence = {FighterDirection::DOWN, FighterDirection::DOWN_RIGHT, FighterDirection::RIGHT};
    fireball.buttonMask = 0x01;
    fireball.windowFrames = 15;
    fireball.attack.name = "Hadouken";
    fireball.attack.startup = 12;
    fireball.attack.active = 4;
    fireball.attack.recovery = 20;
    fireball.attack.damage = 70;
    fireball.attack.hitstun = 22;
    fireball.attack.blockstun = 12;
    fireball.attack.knockbackX = 4.0f;
    fireball.attack.knockbackY = 0.0f;
    def.specials.push_back(fireball);

    CommandInput dp;
    dp.name = "Dragon Punch";
    dp.sequence = {FighterDirection::RIGHT, FighterDirection::DOWN, FighterDirection::DOWN_RIGHT};
    dp.buttonMask = 0x01;
    dp.windowFrames = 15;
    dp.attack.name = "Shoryuken";
    dp.attack.startup = 3;
    dp.attack.active = 6;
    dp.attack.recovery = 28;
    dp.attack.damage = 120;
    dp.attack.hitstun = 30;
    dp.attack.blockstun = 15;
    dp.attack.knockbackX = 2.0f;
    dp.attack.knockbackY = 12.0f;
    def.specials.push_back(dp);

    CommandInput hk;
    hk.name = "Hurricane Kick";
    hk.sequence = {FighterDirection::DOWN, FighterDirection::DOWN_LEFT, FighterDirection::LEFT};
    hk.buttonMask = 0x02;
    hk.windowFrames = 15;
    hk.attack.name = "Tatsumaki";
    hk.attack.startup = 8;
    hk.attack.active = 12;
    hk.attack.recovery = 12;
    hk.attack.damage = 90;
    hk.attack.hitstun = 24;
    hk.attack.blockstun = 10;
    hk.attack.knockbackY = 4.0f;
    def.specials.push_back(hk);

    CommandInput superFireball;
    superFireball.name = "Super Fireball";
    superFireball.sequence = {FighterDirection::DOWN, FighterDirection::DOWN_RIGHT, FighterDirection::RIGHT, FighterDirection::DOWN, FighterDirection::DOWN_RIGHT, FighterDirection::RIGHT};
    superFireball.buttonMask = 0x01;
    superFireball.windowFrames = 25;
    superFireball.attack.name = "Super Fireball";
    superFireball.attack.startup = 8;
    superFireball.attack.active = 10;
    superFireball.attack.recovery = 25;
    superFireball.attack.damage = 230;
    superFireball.attack.hitstun = 45;
    superFireball.attack.blockstun = 20;
    superFireball.attack.knockbackX = 4.0f;
    superFireball.attack.knockbackY = 6.0f;
    def.specials.push_back(superFireball);

    def.throwAttack.name = "Shoulder Throw";
    def.throwAttack.startup = 5;
    def.throwAttack.active = 2;
    def.throwAttack.recovery = 24;
    def.throwAttack.damage = 130;
    def.throwAttack.hitstun = 0;
    def.throwAttack.blockstun = 0;
    def.throwAttack.knockbackX = 5.0f;
    def.throwAttack.knockbackY = 8.0f;

    return def;
}

FighterDef FightingSystem::CreateGrapplerFighter() {
    FighterDef def;
    def.name = "GOROU";
    def.walkSpeed = 2.4f;
    def.backWalkSpeed = 1.8f;
    def.jumpHeight = 140.0f;
    def.jumpForwardSpeed = 2.4f;
    def.jumpDuration = 0.6f;
    def.health = 1200;
    def.height = 135.0f;
    def.width = 60.0f;
    def.bodyColor = {0.8f, 0.2f, 0.2f};
    def.accentColor = {0.3f, 0.3f, 0.3f};
    
    def.pushBoxSize = {52.0f, 125.0f};
    def.pushBoxOffset = {0.0f, -62.5f};
    def.hurtBoxStandSize = {58.0f, 125.0f};
    def.hurtBoxStandOffset = {0.0f, -62.5f};
    def.hurtBoxCrouchSize = {64.0f, 80.0f};
    def.hurtBoxCrouchOffset = {0.0f, -40.0f};

    SetNormalAttack(def, 0, "Stand LP", 5, 3, 8, 45, 14, 7, {38.0f, -85.0f}, {40.0f, 22.0f});
    SetNormalAttack(def, 1, "Stand MP", 8, 4, 12, 85, 20, 11, {42.0f, -85.0f}, {45.0f, 24.0f});
    SetNormalAttack(def, 2, "Stand HP", 12, 5, 18, 140, 28, 16, {50.0f, -85.0f}, {55.0f, 30.0f});
    SetNormalAttack(def, 3, "Stand LK", 6, 3, 9, 40, 13, 6, {38.0f, -15.0f}, {40.0f, 20.0f});
    SetNormalAttack(def, 4, "Stand MK", 10, 4, 13, 80, 19, 10, {46.0f, -15.0f}, {45.0f, 22.0f});
    SetNormalAttack(def, 5, "Stand HK", 14, 5, 20, 135, 27, 14, {55.0f, -25.0f}, {52.0f, 26.0f});

    SetNormalAttack(def, 6, "Crouch LP", 5, 3, 8, 40, 13, 6, {36.0f, -45.0f}, {40.0f, 20.0f});
    SetNormalAttack(def, 7, "Crouch MP", 8, 4, 11, 80, 19, 10, {40.0f, -45.0f}, {42.0f, 22.0f});
    SetNormalAttack(def, 8, "Crouch HP", 10, 5, 16, 130, 24, 13, {44.0f, -65.0f}, {45.0f, 32.0f});
    SetNormalAttack(def, 9, "Crouch LK", 6, 3, 8, 35, 12, 6, {36.0f, -10.0f}, {40.0f, 18.0f}, false, true);
    SetNormalAttack(def, 10, "Crouch MK", 9, 4, 12, 75, 18, 9, {42.0f, -10.0f}, {42.0f, 20.0f}, false, true);
    SetNormalAttack(def, 11, "Crouch HK", 12, 5, 20, 125, 24, 11, {50.0f, -10.0f}, {48.0f, 20.0f}, false, true, 5.0f, 7.0f);

    SetNormalAttack(def, 12, "Jump LP", 5, 5, 4, 50, 16, 8, {32.0f, -55.0f}, {35.0f, 22.0f}, true);
    SetNormalAttack(def, 13, "Jump MP", 7, 6, 4, 90, 22, 11, {36.0f, -55.0f}, {40.0f, 24.0f}, true);
    SetNormalAttack(def, 14, "Jump HP", 10, 6, 6, 150, 30, 14, {42.0f, -65.0f}, {48.0f, 30.0f}, true);
    SetNormalAttack(def, 15, "Jump LK", 5, 5, 4, 45, 15, 7, {34.0f, -20.0f}, {35.0f, 20.0f}, true);
    SetNormalAttack(def, 16, "Jump MK", 8, 6, 4, 85, 21, 10, {38.0f, -25.0f}, {40.0f, 22.0f}, true);
    SetNormalAttack(def, 17, "Jump HK", 11, 6, 6, 145, 29, 13, {46.0f, -30.0f}, {45.0f, 26.0f}, true);

    CommandInput spd;
    spd.name = "Spinning Pile Driver";
    spd.sequence = {FighterDirection::RIGHT, FighterDirection::DOWN_RIGHT, FighterDirection::DOWN, FighterDirection::DOWN_LEFT, FighterDirection::LEFT};
    spd.buttonMask = 0x01;
    spd.windowFrames = 20;
    spd.attack.name = "SPD";
    spd.attack.startup = 6;
    spd.attack.active = 2;
    spd.attack.recovery = 35;
    spd.attack.damage = 250;
    spd.attack.hitstun = 0;
    spd.attack.blockstun = 0;
    spd.attack.knockbackX = 4.0f;
    spd.attack.knockbackY = 14.0f;
    def.specials.push_back(spd);

    CommandInput superSpd;
    superSpd.name = "Super SPD";
    superSpd.sequence = {FighterDirection::RIGHT, FighterDirection::DOWN_RIGHT, FighterDirection::DOWN, FighterDirection::DOWN_LEFT, FighterDirection::LEFT, FighterDirection::RIGHT, FighterDirection::DOWN_RIGHT, FighterDirection::DOWN, FighterDirection::DOWN_LEFT, FighterDirection::LEFT};
    superSpd.buttonMask = 0x01;
    superSpd.windowFrames = 30;
    superSpd.attack.name = "Super SPD";
    superSpd.attack.startup = 10;
    superSpd.attack.active = 2;
    superSpd.attack.recovery = 35;
    superSpd.attack.damage = 320;
    superSpd.attack.hitstun = 0;
    superSpd.attack.blockstun = 0;
    superSpd.attack.knockbackX = 4.0f;
    superSpd.attack.knockbackY = 15.0f;
    def.specials.push_back(superSpd);

    def.throwAttack.name = "German Suplex";
    def.throwAttack.startup = 5;
    def.throwAttack.active = 2;
    def.throwAttack.recovery = 24;
    def.throwAttack.damage = 150;
    def.throwAttack.hitstun = 0;
    def.throwAttack.blockstun = 0;
    def.throwAttack.knockbackX = -6.0f;
    def.throwAttack.knockbackY = 9.0f;

    return def;
}

FighterDef FightingSystem::CreateSpeedFighter() {
    FighterDef def;
    def.name = "RIN";
    def.walkSpeed = 4.2f;
    def.backWalkSpeed = 3.2f;
    def.jumpHeight = 200.0f;
    def.jumpForwardSpeed = 4.0f;
    def.jumpDuration = 0.45f;
    def.health = 850;
    def.height = 115.0f;
    def.width = 45.0f;
    def.bodyColor = {0.6f, 0.1f, 0.8f};
    def.accentColor = {0.1f, 0.9f, 0.9f};
    def.maxJumps = 2;
    
    def.pushBoxSize = {36.0f, 105.0f};
    def.pushBoxOffset = {0.0f, -52.5f};
    def.hurtBoxStandSize = {40.0f, 105.0f};
    def.hurtBoxStandOffset = {0.0f, -52.5f};
    def.hurtBoxCrouchSize = {45.0f, 65.0f};
    def.hurtBoxCrouchOffset = {0.0f, -32.5f};

    SetNormalAttack(def, 0, "Stand LP", 3, 3, 5, 25, 10, 5, {32.0f, -75.0f}, {30.0f, 18.0f});
    SetNormalAttack(def, 1, "Stand MP", 5, 3, 8, 55, 15, 8, {35.0f, -75.0f}, {35.0f, 20.0f});
    SetNormalAttack(def, 2, "Stand HP", 8, 4, 12, 90, 20, 11, {42.0f, -75.0f}, {40.0f, 22.0f});
    SetNormalAttack(def, 3, "Stand LK", 4, 3, 6, 20, 9, 4, {32.0f, -15.0f}, {32.0f, 16.0f});
    SetNormalAttack(def, 4, "Stand MK", 7, 3, 9, 50, 14, 7, {38.0f, -15.0f}, {38.0f, 18.0f});
    SetNormalAttack(def, 5, "Stand HK", 10, 4, 14, 85, 21, 10, {45.0f, -25.0f}, {42.0f, 22.0f});

    SetNormalAttack(def, 6, "Crouch LP", 3, 3, 5, 20, 9, 4, {28.0f, -38.0f}, {32.0f, 16.0f});
    SetNormalAttack(def, 7, "Crouch MP", 5, 3, 8, 50, 14, 7, {32.0f, -38.0f}, {35.0f, 18.0f});
    SetNormalAttack(def, 8, "Crouch HP", 7, 4, 11, 85, 19, 10, {35.0f, -55.0f}, {38.0f, 26.0f});
    SetNormalAttack(def, 9, "Crouch LK", 4, 3, 5, 15, 8, 4, {30.0f, -10.0f}, {32.0f, 14.0f}, false, true);
    SetNormalAttack(def, 10, "Crouch MK", 6, 3, 8, 45, 13, 6, {35.0f, -10.0f}, {35.0f, 16.0f}, false, true);
    SetNormalAttack(def, 11, "Crouch HK", 9, 4, 15, 80, 19, 8, {40.0f, -10.0f}, {38.0f, 16.0f}, false, true, 3.5f, 5.0f);

    SetNormalAttack(def, 12, "Jump LP", 3, 4, 4, 30, 13, 7, {26.0f, -45.0f}, {28.0f, 18.0f}, true);
    SetNormalAttack(def, 13, "Jump MP", 5, 5, 4, 60, 17, 9, {30.0f, -45.0f}, {32.0f, 20.0f}, true);
    SetNormalAttack(def, 14, "Jump HP", 7, 5, 5, 95, 22, 11, {35.0f, -55.0f}, {36.0f, 25.0f}, true);
    SetNormalAttack(def, 15, "Jump LK", 3, 4, 4, 25, 12, 6, {28.0f, -18.0f}, {28.0f, 16.0f}, true);
    SetNormalAttack(def, 16, "Jump MK", 6, 5, 4, 55, 16, 8, {32.0f, -22.0f}, {32.0f, 18.0f}, true);
    SetNormalAttack(def, 17, "Jump HK", 8, 5, 5, 90, 21, 10, {38.0f, -28.0f}, {36.0f, 22.0f}, true);

    CommandInput fujin;
    fujin.name = "Fujin Slash";
    fujin.sequence = {FighterDirection::DOWN, FighterDirection::DOWN_LEFT, FighterDirection::LEFT};
    fujin.buttonMask = 0x01;
    fujin.windowFrames = 15;
    fujin.attack.name = "Fujin Slash";
    fujin.attack.startup = 6;
    fujin.attack.active = 6;
    fujin.attack.recovery = 14;
    fujin.attack.damage = 85;
    fujin.attack.hitstun = 24;
    fujin.attack.blockstun = 11;
    fujin.attack.knockbackX = 3.5f;
    fujin.attack.knockbackY = 0.0f;
    def.specials.push_back(fujin);

    CommandInput gale;
    gale.name = "Gale Teleport";
    gale.sequence = {FighterDirection::DOWN, FighterDirection::DOWN_RIGHT, FighterDirection::RIGHT};
    gale.buttonMask = 0x02;
    gale.windowFrames = 15;
    gale.attack.name = "Gale Teleport";
    gale.attack.startup = 4;
    gale.attack.active = 12;
    gale.attack.recovery = 10;
    gale.attack.damage = 0;
    gale.attack.hitstun = 0;
    gale.attack.blockstun = 0;
    gale.attack.knockbackX = 0.0f;
    gale.attack.knockbackY = 0.0f;
    def.specials.push_back(gale);

    CommandInput superRin;
    superRin.name = "Super Rin";
    superRin.sequence = {FighterDirection::DOWN, FighterDirection::DOWN_LEFT, FighterDirection::LEFT, FighterDirection::DOWN, FighterDirection::DOWN_LEFT, FighterDirection::LEFT};
    superRin.buttonMask = 0x01;
    superRin.windowFrames = 25;
    superRin.attack.name = "Super Rin";
    superRin.attack.startup = 8;
    superRin.attack.active = 10;
    superRin.attack.recovery = 20;
    superRin.attack.damage = 220;
    superRin.attack.hitstun = 40;
    superRin.attack.blockstun = 20;
    superRin.attack.knockbackX = 5.0f;
    superRin.attack.knockbackY = 12.0f;
    def.specials.push_back(superRin);

    def.throwAttack.name = "Neon Throw";
    def.throwAttack.startup = 5;
    def.throwAttack.active = 2;
    def.throwAttack.recovery = 24;
    def.throwAttack.damage = 125;
    def.throwAttack.hitstun = 0;
    def.throwAttack.blockstun = 0;
    def.throwAttack.knockbackX = 4.0f;
    def.throwAttack.knockbackY = 7.0f;

    return def;
}

void FightingSystem::StartRecordingReplay() {
    m_isRecording = true;
    m_replayBuffer.clear();
}

void FightingSystem::StopRecordingReplay() {
    m_isRecording = false;
}

void FightingSystem::SaveReplay(const std::string& path) {
    std::string resolvedPath = path;
    if (std::filesystem::path(path).is_relative()) {
        resolvedPath = (std::filesystem::path(PathResolver::ExeDir()) / path).string();
    }
    FILE* f = fopen(resolvedPath.c_str(), "wb");
    if (!f) {
        Log::Error("FightingSystem: Failed to save replay to {}", resolvedPath);
        return;
    }

    // Write Header
    char magic[4] = {'F', 'R', 'E', 'P'};
    fwrite(magic, 1, 4, f);
    uint16_t version = 1;
    fwrite(&version, 2, 1, f);

    uint8_t p1Char = static_cast<uint8_t>(m_match.p1CharIndex);
    uint8_t p2Char = static_cast<uint8_t>(m_match.p2CharIndex);
    uint8_t stage = static_cast<uint8_t>(m_match.stageIndex);
    uint8_t rounds = static_cast<uint8_t>(m_match.roundsToWin);
    fwrite(&p1Char, 1, 1, f);
    fwrite(&p2Char, 1, 1, f);
    fwrite(&stage, 1, 1, f);
    fwrite(&rounds, 1, 1, f);

    uint32_t totalFrames = static_cast<uint32_t>(m_replayBuffer.size());
    fwrite(&totalFrames, 4, 1, f);

    // Write frames
    for (const auto& frame : m_replayBuffer) {
        uint8_t p1Dir = static_cast<uint8_t>(frame.p1Input.direction);
        uint8_t p1Btn = frame.p1Input.buttons;
        uint8_t p2Dir = static_cast<uint8_t>(frame.p2Input.direction);
        uint8_t p2Btn = frame.p2Input.buttons;

        fwrite(&p1Dir, 1, 1, f);
        fwrite(&p1Btn, 1, 1, f);
        fwrite(&p2Dir, 1, 1, f);
        fwrite(&p2Btn, 1, 1, f);
    }

    fclose(f);
    Log::Info("FightingSystem: Replay saved to {} ({} frames)", path, totalFrames);
}

bool FightingSystem::LoadReplay(const std::string& path) {
    std::string resolvedPath = PathResolver::Resolve(path);
    FILE* f = fopen(resolvedPath.c_str(), "rb");
    if (!f) {
        Log::Error("FightingSystem: Failed to load replay from {}", resolvedPath);
        return false;
    }

    char magic[4];
    if (fread(magic, 1, 4, f) != 4 || magic[0] != 'F' || magic[1] != 'R' || magic[2] != 'E' || magic[3] != 'P') {
        Log::Error("FightingSystem: Invalid replay format in {}", path);
        fclose(f);
        return false;
    }

    uint16_t version;
    fread(&version, 2, 1, f);

    uint8_t p1Char, p2Char, stage, rounds;
    fread(&p1Char, 1, 1, f);
    fread(&p2Char, 1, 1, f);
    fread(&stage, 1, 1, f);
    fread(&rounds, 1, 1, f);

    m_match.p1CharIndex = p1Char;
    m_match.p2CharIndex = p2Char;
    m_match.stageIndex = stage;
    m_match.roundsToWin = rounds;

    uint32_t totalFrames;
    fread(&totalFrames, 4, 1, f);

    m_replayBuffer.clear();
    m_replayBuffer.resize(totalFrames);

    for (uint32_t i = 0; i < totalFrames; ++i) {
        uint8_t p1Dir, p1Btn, p2Dir, p2Btn;
        fread(&p1Dir, 1, 1, f);
        fread(&p1Btn, 1, 1, f);
        fread(&p2Dir, 1, 1, f);
        fread(&p2Btn, 1, 1, f);

        m_replayBuffer[i].p1Input.direction = static_cast<FighterDirection>(p1Dir);
        m_replayBuffer[i].p1Input.buttons = p1Btn;
        m_replayBuffer[i].p2Input.direction = static_cast<FighterDirection>(p2Dir);
        m_replayBuffer[i].p2Input.buttons = p2Btn;
    }

    fclose(f);
    Log::Info("FightingSystem: Replay loaded from {} ({} frames, P1={}, P2={}, Stage={})", 
              path, totalFrames, p1Char, p2Char, stage);
    return true;
}

void FightingSystem::PlayReplay() {
    m_isPlayingReplay = true;
    m_isRecording = false;
    m_replayPlaybackIndex = 0;
}

} // namespace starlight
