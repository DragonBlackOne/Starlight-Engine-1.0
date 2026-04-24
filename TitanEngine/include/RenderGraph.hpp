#pragma once
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <unordered_map>
#include <glad/glad.h>

namespace titan {

    struct RenderPassResource {
        std::string name;
        uint32_t handle = 0;
        uint32_t type = GL_TEXTURE_2D;
    };

    class RenderPass {
    public:
        RenderPass(const std::string& name) : m_name(name) {}
        
        void AddInput(const std::string& name) { m_inputs.push_back(name); }
        void AddOutput(const std::string& name) { m_outputs.push_back(name); }
        
        void SetExecuteCallback(std::function<void()> callback) { m_execute = callback; }
        
        void Execute() { if(m_execute) m_execute(); }
        
        const std::string& GetName() const { return m_name; }

    private:
        std::string m_name;
        std::vector<std::string> m_inputs;
        std::vector<std::string> m_outputs;
        std::function<void()> m_execute;
    };

    class RenderGraph {
    public:
        RenderGraph() = default;
        ~RenderGraph() = default;

        void AddPass(std::shared_ptr<RenderPass> pass);
        void Compile();
        void Execute();

    private:
        std::vector<std::shared_ptr<RenderPass>> m_passes;
        std::unordered_map<std::string, RenderPassResource> m_resources;
    };
}
