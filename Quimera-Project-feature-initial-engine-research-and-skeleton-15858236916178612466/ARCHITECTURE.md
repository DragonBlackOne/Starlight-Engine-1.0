# Arquitetura da Engine: "VAMOS" (Vibrant Advanced Modular Open Source)

A engine segue uma filosofia modular baseada em **ECS (Entity Component System)**, permitindo que componentes extraídos de diferentes fontes (O3DE, Wicked, etc.) coexistam de forma performática.

## Componentes Selecionados

1.  **Core ECS:** EnTT (Padrão MIT, alta performance).
2.  **Renderer Architecture:** Inspirada na **Filament**, utiliza um sistema de `RenderView` e `RenderCommand` para desacoplar a cena da execução gráfica.
3.  **Job System:** Extraído da **Wicked Engine**, provendo um sistema robusto de multi-threading com suporte a contextos e paralelismo em grupos.
4.  **Math Library:** Extraída da **Godot Engine**, oferecendo tipos como `Vector3`, `Quaternion`, `Basis` e `Transform3D` otimizados e fáceis de usar.
5.  **Physics:** Jolt Physics (via O3DE/standalone).
6.  **Build System:** CMake (para máxima compatibilidade no Windows).

## Estrutura de Diretórios

```
/
├── CMakeLists.txt         # Configuração principal do CMake
├── REPORT.md              # Relatório de pesquisa inicial
├── ARCHITECTURE.md        # Este documento
├── src/
│   ├── main.cpp           # Ponto de entrada
│   ├── core/              # Gerenciamento de Engine, ECS e Loop
│   ├── renderer/          # Integração de componentes de vídeo
│   └── physics/           # Integração de componentes de física
├── include/               # Headers públicos
└── thirdparty/            # Código extraído de outras engines (submódulos ou cópias)
```

## Fluxo de Execução

1.  **Inicialização:** O core carrega os módulos (Gems/Plugins).
2.  **Main Loop:**
    - Processamento de Input.
    - Atualização de Sistemas ECS (Movimento, IA).
    - Atualização de Física (Step).
    - Renderização (Sync de transformações ECS -> Renderer).
3.  **Shutdown:** Cleanup ordenado dos componentes.
