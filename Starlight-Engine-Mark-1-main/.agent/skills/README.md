# Starlight Engine — Agent Skills Directory

Bem-vindo ao diretório de habilidades do Agent (`.agent/skills/`). Este repositório de conhecimento contém protocolos de arquitetura, convenções de código e documentação modular para cada subsistema do Starlight Engine Mark 1.

## Navegação Rápida
Como possuímos uma densidade acentuada (atualmente 180+ skills), os tópicos gerais estão separados pelos seguintes prefixos:

* **`ai-*`**: Sistemas de Inteligência Artificial, Comportamento, Percepção e Árvores Lógicas.
* **`audio-*`**: Áudio Espacial 3D, Síntese, DSP (Reverb, FFT), e Mixing em Rust (Rodio).
* **`core-*`**: Sistemas fundamentais da Engine (Input, Game Loop, Scene Manager, FFI API).
* **`editor-*`**: Todos os painéis, utilitários, gizmos e janelas da interface Starlight Studio (DearPyGui).
* **`game-*`**: Guias para sistemas orientados a Gameplay (Inventários, Interações, RPG Stats).
* **`net-*`**: Documentações sobre Transporte Network, Replicação, Lag Compensation e Arquitetura Cliente/Servidor.
* **`opt-*`**: Estratégias Críticas de Otimização, Culling, Mip-Maps, Instancing e Memory Pools.
* **`physics-*`**: Wrappers, Colliders e Queries do backend Rapier3D.
* **`platform-*`**: Input mappings locais, Display Configs, Steam APIs.
* **`render-*`**: Tópicos avançados da WGPU (Forward+, PBR, Post-Processing, Custom Shaders, Shadows).
* **`ui-*`**: Padronização da interface in-game e widgets 2D/3D no Python.

### Skills Vitais para Leitura:
- `starlight-master/SKILL.md`: O Blueprint Principal Definitivo da Engine com a relação Rust <-> Python.
- `visual-scripting/SKILL.md`: Lógicas baseadas em Nodos via UI e injeções PyO3 runtime.
- `render-lighting/SKILL.md`: Guia de configuração atmosférica PBR e Shaders.
- `rust-build/SKILL.md` e `starlight-standard/SKILL.md`: Regras basilares de Code Style e build pipelines da camada C.

Use estas documentações como fonte de verdade primária para alterar e evoluir subsistemas massivos da engine.
