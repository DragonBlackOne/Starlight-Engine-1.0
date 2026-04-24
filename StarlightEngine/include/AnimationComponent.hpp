// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <ozz/animation/runtime/skeleton.h>
#include <ozz/animation/runtime/animation.h>
#include <ozz/base/containers/vector.h>
#include <ozz/base/maths/soa_transform.h>
#include <vector>
#include <glm/glm.hpp>

namespace starlight {

    struct SkeletalAnimationComponent {
        ozz::animation::Skeleton skeleton;
        ozz::animation::Animation animation;
        
        float currentTime = 0.0f;
        float playbackSpeed = 1.0f;
        bool loop = true;
        bool playing = true;

        ozz::vector<ozz::math::SoaTransform> m_ozzLocals;
        ozz::vector<ozz::math::Float4x4> m_ozzModels;
        std::vector<glm::mat4> m_boneMatrices; 

        bool initialized = false;

        void LoadSkeleton(const char* path);
        void LoadAnimation(const char* path);
    };

}
