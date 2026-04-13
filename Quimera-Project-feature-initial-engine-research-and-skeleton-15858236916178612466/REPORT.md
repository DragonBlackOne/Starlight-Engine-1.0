# Relatório de Pesquisa: Engines Open Source Permissivas

Este relatório detalha as principais game engines open source com licenças permissivas (MIT, Apache 2.0, BSD) e analisa a viabilidade de extrair seus componentes para a criação de uma nova engine modular em C++.

## 1. Seleção de Engines e Licenças

Foram selecionadas engines que permitem modificação, distribuição e uso comercial sem a obrigatoriedade de abrir o código-fonte do projeto final (licenças permissivas).

| Engine | Licença | Linguagem | Foco | Modularidade |
| :--- | :--- | :--- | :--- | :--- |
| **Open 3D Engine (O3DE)** | Apache 2.0 | C++ | 3D AAA | Alta (Arquitetura de "Gems") |
| **Godot Engine** | MIT | C++ | 2D/3D | Média (Muito acoplada ao core) |
| **Wicked Engine** | MIT | C++ | 3D High-End | Alta (Estrutura clara de bibliotecas) |
| **Filament (Google)** | Apache 2.0 | C++ | Renderização PBR | Altíssima (É uma biblioteca de render) |
| **Panda3D** | BSD | C++, Python | 3D | Média |
| **Torque3D** | MIT | C++ | 3D | Baixa (Legado) |

## 2. Análise de Componentes para Extração

### 2.1. Renderização (Graphics)
*   **Filament:** Desenvolvido pelo Google, é focado puramente em renderização PBR (Physically Based Rendering). É a opção mais fácil de "extrair" por já ser modular.
*   **O3DE (Atom Renderer):** O "Atom" é o renderizador moderno da O3DE. É extremamente poderoso, mas extraí-lo requer lidar com o sistema de "AZ Modules" da Amazon.
*   **Wicked Engine:** Possui um renderizador moderno e eficiente que pode ser adaptado com relativa facilidade em projetos C++ externos.

### 2.2. Física (Physics)
*   **O3DE (PhysX/Jolt Gems):** A O3DE usa PhysX ou Jolt via "Gems". O Jolt Physics é atualmente uma das melhores opções open source (MIT).
*   **Godot Physics:** A Godot possui seu próprio servidor de física, mas extraí-lo exigiria levar muitas dependências internas.

### 2.3. Arquitetura (ECS - Entity Component System)
*   **O3DE (Component Entity System):** Uma implementação robusta, mas pesada.
*   **EnTT (MIT):** Embora seja uma biblioteca e não uma engine completa, é o padrão de facto para ECS em C++ de alta performance e é usada por muitas engines modernas.

## 3. Estratégia Proposta para a Nova Engine

Para atender ao requisito de "extrair de várias engines" mantendo a modernidade (ECS) e o foco em Windows:

1.  **Core/ECS:** Utilizar a filosofia de ECS. Embora o usuário tenha pedido para "extrair de engines", o sistema de ECS da **EnTT** é tão superior e comum que serviria como a "cola" ideal para os componentes extraídos.
2.  **Renderização:** Adaptar o renderizador da **Wicked Engine** ou **Filament**. A Wicked Engine oferece uma base C++ mais próxima de uma "engine de jogo" completa (incluindo suporte a DX12/Vulkan).
3.  **Física:** Integrar o **Jolt Physics** (extraído da O3DE ou usado como componente standalone, que é como a O3DE o trata).
4.  **Janelamento/Input:** Extrair/Utilizar a lógica da **GLFW** ou similar (comumente usada por todas as engines acima).

## 4. Conclusão da Pesquisa

A melhor abordagem técnica para "criar uma engine com outras" hoje é focar na O3DE para arquitetura de módulos e na Wicked Engine para funcionalidades gráficas rápidas em Windows.

## 5. Implementação Realizada

Foram integrados com sucesso:
- **Godot Math:** Tipos matemáticos core extraídos e adaptados.
- **Wicked Job System:** Sistema de multi-threading profissional.
- **Filament Architecture:** Refatoração do renderizador para comandos.

---
*Status: Componentes de alto nível integrados. Engine VAMOS pronta para expansão.*
