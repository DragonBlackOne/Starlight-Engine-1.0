# ⚡ Starlight Quickstart: Your First Game

Bem-vindo ao Starlight Engine! Este guia rápido (5 minutos) ensinará você a compilar a engine, abrir o Starlight Studio, criar sua primeira cena interativa e exportá-la.

## 🚀 1. Compilation & Setup

Certifique-se de ter Python 3.10+ e Rust instalados.
Abra seu terminal na raiz do projeto:

```bash
# Crie e ative seu ambiente virtual
python -m venv venv
.\venv\Scripts\activate  # Windows
# source venv/bin/activate # Linux/Mac

# Instale ferramentas de build e UI
pip install maturin numpy pillow dearpygui

# Compile o núcleo C++/Rust (Isto irá gerar o starlight.backend)
maturin develop --release
```

## 🎮 2. Lançando o Starlight Studio

Diferente de frameworks puramente via código, o Starlight possui um editor visual completo. Inicie-o:

```bash
python pysrc/starlight/editor/studio.py
```

Você será recebido por uma interface escura (Dear PyGui) contendo:
*   **Viewport**: A renderização 3D em tempo real.
*   **Scene Hierarchy**: A lista de entidades no seu mundo.
*   **Inspector**: Propriedades da entidade selecionada.
*   **Asset Browser**: Seus arquivos locais (`assets/`, `scripts/`).

## 🧱 3. Criando sua Primeira Entidade

1.  Na aba **Hierarchy**, clique em `+ Add Entity`.
2.  Uma nova entidade aparecerá na lista. Clique nela.
3.  No **Inspector**, mude o nome de "New Entity" para "Alien" e aperte Enter.
4.  No campo **Mesh** de `<None>`, escreva `cube` e dê Enter. Você verá um cubo no Viewport!
5.  Role o Inspector para baixo até **Native Components**, selecione `PointLight` no menu e clique em **Add**. O cubo agora emite luz.

## 🐍 4. Escrevendo Lógica Python (The Scripting Layer)

O Starlight permite estender o ECS do Rust usando scripts Python.

1.  Abra seu VS Code e crie um arquivo `scripts/spin.py`:

```python
from starlight.behaviour import StarlightBehaviour
from starlight import backend

class SpinBehaviour(StarlightBehaviour):
    def on_start(self):
        self.rot_y = 0.0
        
    def update(self, dt):
        self.rot_y += 1.0 * dt
        # Chama diretamente a engine C-API (Custo zero!)
        backend.set_rotation(self.entity.id, 0, self.rot_y, 0)
```

2.  Volte para o Starlight Studio. Selecione o seu cubo "Alien".
3.  No Inspector, role até **Behaviours**.
4.  Digite `scripts/spin.py` e clique em **Add**.
5.  Clique no botão 🟥 **Play (Espaço)** no console de reprodução. O cubo começará a girar imediatamente sem precisar recompilar a engine!

## 📦 5. Standalone Build

Sua obra-prima está pronta.
1.  Pare a simulação ⏹️ **Stop**.
2.  Vá no menu superior: `File -> Build Game...`.
3.  O console indicará sucesso. Vá até a pasta `build/` que acaba de ser gerada na raiz do projeto.
4.  Lá haverá um executável `run.bat` (ou `main.py`). Este é o seu jogo independente, livre da interface do Editor, rodando na performance máxima!

Bem-vindo à Starlight Engine. Explore o manual (`docs/MANUAL.md`) para aprender sobre Física Rapier3D, Sombras Dinâmicas e Multiplayer!
