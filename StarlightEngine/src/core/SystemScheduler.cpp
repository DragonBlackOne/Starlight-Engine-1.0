#include "SystemScheduler.hpp"
#include "Engine.hpp"
#include "Log.hpp"
#include <chrono>

namespace starlight {

    void SystemScheduler::Build(const std::vector<std::shared_ptr<ISystem>>& systems) {
        m_nodes.clear();
        m_layers.clear();

        // 1. Cria nos do grafo na ordem atual (ja ordenada por prioridade)
        for (size_t i = 0; i < systems.size(); ++i) {
            if (systems[i]->IsEnabled()) {
                SystemNode node;
                node.system = systems[i];
                node.inDegree = 0;
                node.originalIndex = m_nodes.size();
                m_nodes.push_back(node);
            }
        }

        // 2. Cria as arestas direcionadas com base em conflitos de componentes
        // i < j significa que 'i' tem prioridade mais alta ou foi registrado antes.
        // Aresta direcionada: i -> j
        for (size_t i = 0; i < m_nodes.size(); ++i) {
            for (size_t j = i + 1; j < m_nodes.size(); ++j) {
                if (HasConflict(m_nodes[i], m_nodes[j])) {
                    m_nodes[i].dependents.push_back(j);
                    m_nodes[j].inDegree++;
                }
            }
        }

        // 3. Ordenacao topologica por camadas (Kahn's algorithm por buckets)
        std::vector<int> tempInDegrees(m_nodes.size());
        for (size_t i = 0; i < m_nodes.size(); ++i) {
            tempInDegrees[i] = m_nodes[i].inDegree;
        }

        std::vector<size_t> remaining;
        for (size_t i = 0; i < m_nodes.size(); ++i) {
            remaining.push_back(i);
        }

        while (!remaining.empty()) {
            std::vector<size_t> currentLayer;
            std::vector<size_t> nextRemaining;

            for (size_t idx : remaining) {
                if (tempInDegrees[idx] == 0) {
                    currentLayer.push_back(idx);
                } else {
                    nextRemaining.push_back(idx);
                }
            }

            if (currentLayer.empty()) {
                Log::Error("SystemScheduler: Dependency cycle detected in systems DAG!");
                break;
            }

            // Decrementa graus de entrada dos dependentes
            for (size_t idx : currentLayer) {
                for (size_t depIdx : m_nodes[idx].dependents) {
                    tempInDegrees[depIdx]--;
                }
            }

            m_layers.push_back(currentLayer);
            remaining = nextRemaining;
        }

        Log::Info("SystemScheduler: Compiled {} systems into {} parallel layers.", m_nodes.size(), m_layers.size());
        for (size_t l = 0; l < m_layers.size(); ++l) {
            std::string sysNames = "";
            for (size_t idx : m_layers[l]) {
                sysNames += m_nodes[idx].system->GetName();
                sysNames += " ";
            }
            Log::Info("  Layer {}: {}", l, sysNames);
        }
    }

    bool SystemScheduler::HasConflict(const SystemNode& a, const SystemNode& b) const {
        std::vector<std::type_index> aReads, aWrites;
        std::vector<std::type_index> bReads, bWrites;

        a.system->GetComponentAccess(aReads, aWrites);
        b.system->GetComponentAccess(bReads, bWrites);

        // 1. Escrita - Escrita
        for (const auto& wA : aWrites) {
            for (const auto& wB : bWrites) {
                if (wA == wB) return true;
            }
        }

        // 2. Escrita - Leitura
        for (const auto& wA : aWrites) {
            for (const auto& rB : bReads) {
                if (wA == rB) return true;
            }
        }

        // 3. Leitura - Escrita
        for (const auto& rA : aReads) {
            for (const auto& wB : bWrites) {
                if (rA == wB) return true;
            }
        }

        return false;
    }

    void SystemScheduler::Update(float dt) {
        float scaledDt = Engine::Get().IsPaused() ? 0.0f : dt * Engine::Get().GetTime().timeScale;

        for (const auto& layer : m_layers) {
            ExecuteLayer(layer, dt, scaledDt, false);
        }
    }

    void SystemScheduler::FixedUpdate(float dt) {
        if (Engine::Get().IsPaused()) return;

        for (const auto& layer : m_layers) {
            ExecuteLayer(layer, dt, dt, true);
        }
    }

    void SystemScheduler::ExecuteLayer(const std::vector<size_t>& layerNodes, float dt, float scaledDt, bool isFixedUpdate) {
        for (size_t idx : layerNodes) {
            auto& node = m_nodes[idx];
            if (!node.system->IsEnabled()) continue;

            std::string name = node.system->GetName();
            float dtParam = isFixedUpdate ? dt : ((name == "InputSystem" || name == "EditorSystem" || name == "FileWatcher") ? dt : scaledDt);

            auto start = std::chrono::high_resolution_clock::now();
            if (isFixedUpdate) {
                node.system->OnFixedUpdate(dtParam);
            } else {
                node.system->OnUpdate(dtParam);
            }
            auto end = std::chrono::high_resolution_clock::now();
            float elapsed = std::chrono::duration<float, std::milli>(end - start).count();

            if (name == "ScriptSystem") {
                Engine::Get().AccumulateScriptTime(elapsed);
            } else if (name == "AudioSystem") {
                Engine::Get().AccumulateAudioTime(elapsed);
            }
        }
    }

}
