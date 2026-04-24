#include "RenderGraph.hpp"
#include "Log.hpp"

namespace titan {

    void RenderGraph::AddPass(std::shared_ptr<RenderPass> pass) {
        m_passes.push_back(pass);
    }

    void RenderGraph::Compile() {
        // Em uma implementação completa (AAA), faríamos a validação de DAG (Directed Acyclic Graph),
        // resolução de dependências, culling de passes não utilizados e aliasing de memória VRAM aqui.
        Log::Info("RenderGraph: Compiled " + std::to_string(m_passes.size()) + " passes successfully.");
    }

    void RenderGraph::Execute() {
        for (auto& pass : m_passes) {
            pass->Execute();
        }
    }

}
