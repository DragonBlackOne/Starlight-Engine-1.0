// Este projeto é feito por IA e só o prompt é feito por um humano.
#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <string>

namespace starlight {

    enum class BTStatus { Success, Failure, Running };

    class BTNode {
    public:
        virtual ~BTNode() = default;
        virtual BTStatus Update(float dt) = 0;
    };

    class BTComposite : public BTNode {
    public:
        void AddChild(std::shared_ptr<BTNode> child) { m_children.push_back(child); }
    protected:
        std::vector<std::shared_ptr<BTNode>> m_children;
    };

    // Returns Success if any child succeeds (OR)
    class BTSelector : public BTComposite {
    public:
        BTStatus Update(float dt) override;
    };

    // Returns Failure if any child fails (AND)
    class BTSequence : public BTComposite {
    public:
        BTStatus Update(float dt) override;
    };

    class BTAction : public BTNode {
    public:
        BTAction(std::function<BTStatus(float)> action) : m_action(action) {}
        BTStatus Update(float dt) override { return m_action(dt); }
    private:
        std::function<BTStatus(float)> m_action;
    };

}
