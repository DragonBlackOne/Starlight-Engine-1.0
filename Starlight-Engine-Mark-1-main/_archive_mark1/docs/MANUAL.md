# 📖 Starlight Engine - Manual Completo

Bem-vindo ao manual completo da **Starlight Engine**. Este documento detalha como operar a Starlight Studio (Interface Visual) e como usar a API Python em harmonia com a infraestrutura Custo Zero em Rust.

---

## 🏗️ 1. Paradigma do ECS e Starlight Studio

O Starlight utiliza um sistema híbrido ECS (Entity Component System):
- O **Rust Core** gerencia memória, loops de física (Rapier3D) e loops de render (WGPU).
- O **Starlight Studio** manipula esses dados através do pacote `starlight.backend`.

### Starlight Studio: Interface Geral
Ao executar `python pysrc/starlight/editor/studio.py`, você entra no Editor Visual.
- **Hierarchy Panel**: Lista as Instâncias. Clique com botão direito para `Duplicate` ou `Delete`.
- **Inspector Panel**: Exibe dados brutos selecionados (Transformações de Matriz, Materiais PBR).
- **Viewport**: A câmera do editor. Pressione o Botão Direito do Mouse + WASD para navegar no mundo.

---

## 🐍 2. The Scripting Layer 

A revolução da engine reside nos `Behaviours`. Eles substituem a necessidade de repensar a Game Engine a cada novo projeto.

1.  Um behavior deve sempre herdar de `StarlightBehaviour`.
2.  Defina `on_start(self)` e `update(self, dt)`.
3.  Você pode ler o estado e as referências usando `self.entity` contendo todas as descrições da entidade.
4.  No editor: Adicione a string (ex: `scripts/enemy.py`) no painel **Behaviours** da Entidade.

*Exemplo de Patrulha Inimiga*:
```python
import math
from starlight.behaviour import StarlightBehaviour
from starlight import backend

class Patrol(StarlightBehaviour):
    def on_start(self):
        self.time = 0.0
        self.start_x = backend.get_transform(self.entity.id)[0]

    def update(self, dt):
        self.time += dt
        new_x = self.start_x + math.sin(self.time) * 5.0
        # Teleporta sem interferir com a física (Set kinematic)
        backend.set_transform(self.entity.id, new_x, 2.0, 0.0)
```

---

## ⚙️ 3. Componentes Nativos Externos (Native Components)

A interface Rust possui dezenas de Componentes ECS altamente eficientes compilados que você injeta através da Aba **Native Components** do Inspector.
Por não serem scripts Python, eles operam no passo Fixo de Física (16ms) pelo thread do Rust, consumindo O(1) da API de reflexão:

- **Health / Mana**: Injeta um tracker RPG que pode ser recuperado por plugins multiplayer e de dano de sistema.
- **PointLight**: Injeta uma struct `PointLight` lida diretamente na passagem do shader GLSL/WGSL de "Global Illumination".

Ao rodar (▶), a Studio.py fará um "Hot-plug" e chamará os construtores O3 do Rust antes de rodar os scripts python.

---

## 🎨 4. Gráficos, Materiais e Skybox

O Starlight Engine usa PBR Mettalic-Roughness workflow.
No Inspector da entidade:
- **Mesh**: `cube`, `sphere`, ou nome de um `.glb` na pasta `/assets`.
- **Metallic / Roughness**: Controlados por Slider. Metal=1 significa que reflete o mundo quase perfeitamente como espelho se Roughness for baixo (0.0).
- **Post FX/Lighting**: Pelo console (terminal nativo F10), você pode chamar comandos globais como `set_fog(0.1, 0, 0, 0)`.

---

## 📦 5. O Pipeline de Build (Standalone)

O Starlight Studio embute uma ferramenta de empacotamento. É vital que tudo que seu jogo referencie esteja na subpasta `assets/` e `scripts/`.
Para gerar a Release:
1. Vá em `File -> Build Game...`.
2. A pasta `/build` será gerada.
3. Se você passar aos seus usuários os arquivos de `/build`, eles não precisarão ter a `starlight` engine instalada no Python de ambiente, pois o Python deles utilizará os módulos contidos e ativará o `Main App`.

> **Problemas de Física?** A engine adota `(X=Right, Y=Up, Z=Back)` no WGPU, a gravidade incide em `Y=-9.8`.

Para mais dúvidas API profundas ou programação da Rendering Backend em WGSL, leia o repositório `/crates/` em Rust ou consulte o `ARCHITETURA.md`!
