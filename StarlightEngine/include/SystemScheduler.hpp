#pragma once
#include <vector>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <chrono>
#include <string>
#include "EngineSystem.hpp"
#include "JobSystem.hpp"

namespace starlight {

    class SystemScheduler {
    public:
        SystemScheduler() = default;
        ~SystemScheduler() = default;

        // Constrói o grafo de dependências e divide os sistemas em camadas paralelas (Parallel Layers)
        void Build(const std::vector<std::shared_ptr<ISystem>>& systems);

        // Executa todas as camadas em ordem, usando o JobSystem para paralelismo
        void Update(float dt);
        void FixedUpdate(float dt);

    private:
        struct SystemNode {
            std::shared_ptr<ISystem> system;
            std::vector<size_t> dependents;   // Índices de nós que dependem deste nó (rodarão depois dele)
            int inDegree = 0;                  // Número de dependências diretas ativas
            size_t originalIndex = 0;
        };

        std::vector<SystemNode> m_nodes;
        std::vector<std::vector<size_t>> m_layers; // Índices de m_nodes agrupados por camadas paralelas

        // Retorna verdadeiro se há conflito de acesso a componentes ECS entre os dois nós
        bool HasConflict(const SystemNode& a, const SystemNode& b) const;

        // Executa uma camada de nós do grafo
        void ExecuteLayer(const std::vector<size_t>& layerNodes, float dt, float scaledDt, bool isFixedUpdate);
    };

}
