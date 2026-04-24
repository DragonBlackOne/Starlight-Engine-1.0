// Este projeto é feito por IA e só o prompt é feito por um humano.
#include "RenderGraph.hpp"
#include "Log.hpp"

namespace titan {

    void RenderGraph::AddPass(std::shared_ptr<RenderPass> pass) {
        m_passes.push_back(pass);
    }

    void RenderGraph::Compile() {
        // Em uma implementaÃ§Ã£o completa (AAA), farÃ­amos a validaÃ§Ã£o de DAG (Directed Acyclic Graph),
        // resoluÃ§Ã£o de dependÃªncias, culling de passes nÃ£o utilizados e aliasing de memÃ³ria VRAM aqui.
        Log::Info("RenderGraph: Compiled " + std::to_string(m_passes.size()) + " passes successfully.");
    }

    void RenderGraph::Execute() {
        for (auto& pass : m_passes) {
            pass->Execute();
        }
    }

}
