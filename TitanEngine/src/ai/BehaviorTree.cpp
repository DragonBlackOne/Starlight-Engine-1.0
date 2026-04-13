#include "BehaviorTree.hpp"

namespace titan {

    BTStatus BTSelector::Update(float dt) {
        for (auto& child : m_children) {
            BTStatus status = child->Update(dt);
            if (status != BTStatus::Failure) {
                return status;
            }
        }
        return BTStatus::Failure;
    }

    BTStatus BTSequence::Update(float dt) {
        for (auto& child : m_children) {
            BTStatus status = child->Update(dt);
            if (status != BTStatus::Success) {
                return status;
            }
        }
        return BTStatus::Success;
    }

}
