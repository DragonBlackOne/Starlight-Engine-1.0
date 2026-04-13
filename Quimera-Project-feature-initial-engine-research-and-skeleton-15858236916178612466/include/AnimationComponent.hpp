#pragma once
#include <ozz/animation/runtime/skeleton.h>
#include <ozz/animation/runtime/animation.h>
#include <ozz/animation/runtime/sampling_job.h>
#include <ozz/base/io/archive.h>
#include <ozz/base/io/stream.h>
#include <ozz/base/span.h>
#include <ozz/base/containers/vector.h>
#include <ozz/base/maths/simd_math.h>
#include <ozz/base/maths/soa_transform.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace Vamos {
    struct SkeletalAnimationComponent {
        ozz::animation::Skeleton skeleton;
        ozz::animation::Animation animation;
        
        float currentTime = 0.0f;
        float playbackSpeed = 1.0f;
        bool loop = true;
        bool playing = true;

        // Buffers de runtime — ozz::vector garante alinhamento SIMD correto
        ozz::vector<ozz::math::SoaTransform> m_ozzLocals;
        ozz::vector<ozz::math::Float4x4> m_ozzModels;
        std::vector<glm::mat4> m_boneMatrices; 

        bool initialized = false;

        void LoadSkeleton(const char* path) {
            ozz::io::File file(path, "rb");
            if (!file.opened()) return;
            ozz::io::IArchive archive(&file);
            archive >> skeleton;
            
            m_ozzLocals.resize(skeleton.num_soa_joints());
            m_ozzModels.resize(skeleton.num_joints());
            m_boneMatrices.resize(skeleton.num_joints());
            initialized = true;
        }

        void LoadAnimation(const char* path) {
            ozz::io::File file(path, "rb");
            if (!file.opened()) return;
            ozz::io::IArchive archive(&file);
            archive >> animation;
        }
    };
}
