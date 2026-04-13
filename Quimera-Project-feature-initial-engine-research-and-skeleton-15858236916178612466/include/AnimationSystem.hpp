#pragma once
#include <entt/entt.hpp>
#include "AnimationComponent.hpp"
#include "TransformComponent.hpp"
#include "JobSystem.hpp"
#include <ozz/animation/runtime/sampling_job.h>
#include <ozz/animation/runtime/local_to_model_job.h>
#include <ozz/base/maths/simd_math.h>
#include <ozz/base/span.h>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <cmath>

namespace Vamos {
    class AnimationSystem {
    public:
        // Processa uma entidade individual (chamado pelo Job System)
        static void UpdateEntity(entt::registry& registry, entt::entity entity, float dt) {
            auto& anim = registry.get<SkeletalAnimationComponent>(entity);
            if (!anim.initialized || !anim.playing) return;

            // 1. Atualizar tempo
            anim.currentTime += dt * anim.playbackSpeed;
            float duration = anim.animation.duration();
            if (anim.loop) {
                anim.currentTime = fmod(anim.currentTime, duration);
            } else {
                anim.currentTime = (std::min)(anim.currentTime, duration);
            }

            // 2. Sampling Job
            ozz::animation::SamplingJob sampling_job;
            sampling_job.animation = &anim.animation;
            sampling_job.context = nullptr; 
            sampling_job.ratio = anim.currentTime / duration;
            sampling_job.output = ozz::make_span(anim.m_ozzLocals);
            if (!sampling_job.Run()) return;

            // 3. Local-to-Model Job
            ozz::animation::LocalToModelJob ltm_job;
            ltm_job.skeleton = &anim.skeleton;
            ltm_job.input = ozz::make_span(anim.m_ozzLocals);
            ltm_job.output = ozz::make_span(anim.m_ozzModels);
            if (!ltm_job.Run()) return;

            // 4. Copia SIMD -> GLM usando StorePtrU (unaligned store seguro)
            size_t nModels = anim.m_ozzModels.size();
            for (size_t i = 0; i < nModels; ++i) {
                float m[16];
                ozz::math::StorePtrU(anim.m_ozzModels[i].cols[0], m + 0);
                ozz::math::StorePtrU(anim.m_ozzModels[i].cols[1], m + 4);
                ozz::math::StorePtrU(anim.m_ozzModels[i].cols[2], m + 8);
                ozz::math::StorePtrU(anim.m_ozzModels[i].cols[3], m + 12);
                
                // glm::mat4 é column-major, mesma ordem que o array
                anim.m_boneMatrices[i] = glm::make_mat4(m);
            }
        }

        // Dispara a atualização paralela de todas as entidades com animação
        static void Update(entt::registry& registry, float dt) {
            auto view = registry.view<SkeletalAnimationComponent, Transform>();

            // Coleta entidades
            std::vector<entt::entity> entities;
            for (auto entity : view) {
                entities.push_back(entity);
            }
            
            if (entities.empty()) return;

            static JobContext animCtx;
            uint32_t jobCount = static_cast<uint32_t>(entities.size());
            entt::entity* pEntities = entities.data();
            entt::registry* pRegistry = &registry;

            JobSystem::Dispatch(animCtx, jobCount, 1, [pEntities, pRegistry, dt](wi::jobsystem::JobArgs jobArgs) {
                AnimationSystem::UpdateEntity(*pRegistry, pEntities[jobArgs.jobIndex], dt);
            });

            JobSystem::Wait(animCtx);
        }
    };
}
