#pragma once
#include <vector>
#include <string>
#include <map>
#include <glm/glm.hpp>
#include <ozz/animation/runtime/skeleton.h>
#include <ozz/animation/runtime/animation.h>
#include <ozz/animation/runtime/sampling_job.h>
#include <ozz/animation/runtime/local_to_model_job.h>
#include <ozz/base/memory/unique_ptr.h>

namespace starlight {

    struct SkeletalAnimationComponent {
        ozz::unique_ptr<ozz::animation::Skeleton> skeleton;
        std::map<std::string, ozz::unique_ptr<ozz::animation::Animation>> animations;
        
        std::string activeAnimation;
        float currentTime = 0.0f;
        float playbackSpeed = 1.0f;
        bool playing = true;
        bool looping = true;

        std::vector<glm::mat4> boneMatrices; // Final matrices for shader
        
        // ozz runtime objects
        ozz::animation::SamplingJob::Context samplingContext;
        
        bool initialized = false;
        
        SkeletalAnimationComponent() : initialized(false) {}
        SkeletalAnimationComponent(const SkeletalAnimationComponent&) = delete;
        SkeletalAnimationComponent& operator=(const SkeletalAnimationComponent&) = delete;
        SkeletalAnimationComponent(SkeletalAnimationComponent&&) = default;
        SkeletalAnimationComponent& operator=(SkeletalAnimationComponent&&) = default;
    };

}
