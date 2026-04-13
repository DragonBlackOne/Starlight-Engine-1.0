---
name: visual-scripting
description: Guia de arquitetura e uso para o Lightweight Shader Graph e Logic Graph (Event System).
---

# Visual Scripting & Artist Tools

O **Starlight Engine Mark 1** possui duas vertentes principais de Visual Scripting via **Dear PyGui**:

1. **Lightweight Shader Graph** (Geração de código WGSL em tempo-real)
2. **Logic Nodes Blueprint** (Lógica de Gameplay via Python/Rust bindings)

Ambas residem em `pysrc/starlight/editor/nodes/`.

## 1. Lightweight Shader Graph (`shader_graph.py`)

Cria interfaces visuais baseadas em nós (math, constants, textures) que transpilam para `wgsl` e ativam **WGPU Shader Hot-Reloading**.

### Mapeamento de Nós PBR
- **Master Node**: Recebe conexões para `Albedo`, `Normal`, `Metallic`, `Roughness` e `Emission`.
- **Logic / Math**: Nós de multiplicar, de somar, etc.
- **Variables**: `Time (Float)`, `Color (Vec3)`.

### Compilação (AST & FFI)
Ao clicar em "Compile WGSL", o grafo faz parsing do Master node de trás pra frente de forma recursiva (Abstract Syntax Tree resolve). O código bruto `.wgsl` gerado reescreve localmente a sombra/luz no pipeline principal.
O envio é feito via injeção C-API:
```c
# O código gerado encapsula as properties:
self.backend.recompile_shader(modified_source)
```

## 2. Logic Graph - Blueprint Event System (`logic_graph.py`)

Interpreta execução de lógica Visual em *Runtime* através de ligações de execução.

### Pinos de Fluxo (Exec Pins)
Identificáveis por triângulos (Exec In / Exec Out):
- `On Start` (Entry point principal)
- `On Update` (Execução a cada Frame)
- `Branch/If` (Nó condicional, direciona fluxo em True ou False)

### Pinos de Dados (Data Pins)
Identificáveis por círculos, processam estados simples (int, float, entity_id).

### Dispatching (Interpreter Python)
O `_execute_node(node_id)` viaja pelos *Exec Outputs* e realiza a ponte entre UI e as macros nativas.
Exemplo: Se o tipo for `Set Position`, o Graph capta os input de `Entity ID` e faz dispatch pro Rust:
```c
self.studio.backend.set_transform(ent, pos[0], pos[1], pos[2])
```

## Regras e Fluxo de Crescimento
1. **Regra de Adição de Nós**: Para expandir a ferramenta gráfica, declare a GUI em `add_x_node` e atualize a lógica recursiva de geração WGSL (Shader) ou o IF-ELSE do Flow Execution (Logic).
2. **Acessibilidade**: Sempre providenciar valores default se um pino *Data* do Shader Graph ficar vazio (para não invalidar a string WGSL com erro de compilação da WGPU). 
3. **Persistência**: Ao gerar grafos, lembre-se que o .json da UI pode ser modificado e salvo junto de *Prefabs*.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Architecture:** Pure C (C11)\n> - **Dependencies:** SDL2, OpenGL 4.1, Flecs ECS, cglm\n> **Standard:** English-only code and comments. No Rust or Python in the core path.
