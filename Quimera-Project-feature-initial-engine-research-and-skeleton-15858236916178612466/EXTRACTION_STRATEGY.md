# Estratégia de Extração de Componentes de Engines Grandes

Para acelerar o desenvolvimento da engine **VAMOS**, selecionamos componentes específicos de grandes motores de jogo que utilizam licenças extremamente permissivas (Apache 2.0 e MIT).

## 1. Componentes Identificados para Extração/Integração

### A. Jolt Physics (O3DE / Horizon Forbidden West)
*   **Origem:** É o motor de física oficial da **Open 3D Engine (O3DE)**.
*   **Licença:** MIT.
*   **Por que:** Altíssima performance, moderno, e já validado em jogos AAA. Ele substitui o PhysX na O3DE e é perfeito para ser extraído como um módulo de física para a VAMOS.

### B. Job System (Wicked Engine)
*   **Origem:** **Wicked Engine**.
*   **Licença:** MIT.
*   **Por que:** O sistema de multi-threading (Job System) da Wicked Engine é simples, eficiente e fácil de isolar. Ele permite que a engine utilize todos os núcleos da CPU para tarefas como física e animação.

### C. Sistema de Jobs/Tasks (O3DE AzCore)
*   **Origem:** **O3DE (Módulo Base AzCore)**.
*   **Licença:** Apache 2.0.
*   **Por que:** Se precisarmos de algo mais robusto que a Wicked Engine, o AzCore da O3DE possui sistemas de memória e gerenciamento de tarefas de nível industrial.

### D. Renderizador PBR (Filament / Wicked Engine)
*   **Origem:** **Filament (Google)** e **Wicked Engine**.
*   **Licença:** Apache 2.0 / MIT.
*   **Por que:** Em vez de escrever shaders manuais, extrairemos as técnicas de iluminação global e PBR desses motores.

## 2. Plano de Implementação Imediata

1.  **Módulo de Física:** Integrar o **Jolt Physics** (o mesmo componente da O3DE).
2.  **Módulo de Multi-threading:** Extrair/Adaptar o **wiJobSystem** da Wicked Engine.

---
*Status: Pesquisa concluída. Iniciando a integração do Jolt Physics.*
