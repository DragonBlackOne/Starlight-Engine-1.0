// Este projeto ÃƒÆ’Ã‚Â© feito por IA e sÃƒÆ’Ã‚Â³ o prompt ÃƒÆ’Ã‚Â© feito por um humano.
#include "AnimationSystem.hpp"
#include "Engine.hpp"
#include <ozz/animation/runtime/sampling_job.h>
#include <ozz/animation/runtime/local_to_model_job.h>
#include <ozz/base/span.h>
#include <ozz/base/maths/soa_transform.h>
#include <ozz/base/maths/simd_math.h>

namespace starlight {

    void AnimationSystem::Update(float dt) {
        auto activeScene = Engine::Get().GetSceneStack().Active();
        if (!activeScene) return;

        auto& registry = activeScene->GetRegistry();
        auto view = registry.view<SkeletalAnimationComponent>();

        for (auto entity : view) {
            auto& anim = view.get<SkeletalAnimationComponent>(entity);
            if (!anim.playing || anim.activeAnimation.empty() || !anim.skeleton) continue;

            auto it = anim.animations.find(anim.activeAnimation);
            if (it == anim.animations.end() || !it->second) continue;

            const ozz::animation::Animation& animation = *it->second;
            const ozz::animation::Skeleton& skeleton = *anim.skeleton;

            if (anim.samplingContext.max_tracks() < skeleton.num_joints()) {
                anim.samplingContext.Resize(skeleton.num_joints());
            }

            // Update time
            anim.currentTime += dt * anim.playbackSpeed;
            float duration = animation.duration();
            if (anim.currentTime > duration) {
                if (anim.looping) anim.currentTime = fmod(anim.currentTime, duration);
                else anim.currentTime = duration;
            }

            // 1. Sampling Job
            ozz::animation::SamplingJob sampling_job;
            sampling_job.animation = &animation;
            sampling_job.context = &anim.samplingContext;
            sampling_job.ratio = anim.currentTime / duration;
            
            // Output local-space transforms (SoA)
            std::vector<ozz::math::SoaTransform> locals(skeleton.num_soa_joints());
            sampling_job.output = ozz::make_span(locals);

            if (!sampling_job.Run()) continue;

            // 2. Local-to-Model Job
            ozz::animation::LocalToModelJob ltm_job;
            ltm_job.skeleton = &skeleton;
            ltm_job.input = ozz::make_span(locals);
            
            // Output model-space matrices
            std::vector<ozz::math::Float4x4> models(skeleton.num_joints());
            ltm_job.output = ozz::make_span(models);

            if (!ltm_job.Run()) continue;

            // 3. Convert to GLM and store in component
            anim.boneMatrices.resize(skeleton.num_joints());
            for (int i = 0; i < skeleton.num_joints(); ++i) {
                const ozz::math::Float4x4& m = models[i];
                // ozz is column-major like GLM
                anim.boneMatrices[i] = glm::mat4(
                    m.cols[0].m128_f32[0], m.cols[0].m128_f32[1], m.cols[0].m128_f32[2], m.cols[0].m128_f32[3],
                    m.cols[1].m128_f32[0], m.cols[1].m128_f32[1], m.cols[1].m128_f32[2], m.cols[1].m128_f32[3],
                    m.cols[2].m128_f32[0], m.cols[2].m128_f32[1], m.cols[2].m128_f32[2], m.cols[2].m128_f32[3],
                    m.cols[3].m128_f32[0], m.cols[3].m128_f32[1], m.cols[3].m128_f32[2], m.cols[3].m128_f32[3]
                );
            }
        }
    }

    void AnimationSystem::UpdateEntity(entt::registry& registry, entt::entity entity, float dt) {
        // Legacy method, can be removed or redirected
        (void)registry; (void)entity; (void)dt;
    }

}
