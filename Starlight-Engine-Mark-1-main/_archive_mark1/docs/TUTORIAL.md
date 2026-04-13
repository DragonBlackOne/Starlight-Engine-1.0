# Tutorial: Criando seu Primeiro Jogo na Starlight Engine (v4.0)

Este guia rápido mostrará como criar um jogo 2D funcional em poucos minutos usando o novo **StarlightGame Framework**, introduzido na v4.0.

---

## 🏗️ Parte 1: Estrutura do Jogo

A engine organiza os jogos dentro de `src/starlight/games/`. Vamos criar um novo jogo chamado **"Caçador de Quadrados"**.

1.  Crie a pasta: `src/starlight/games/square_hunter/`
2.  Dentro dela, crie um arquivo `__init__.py` (pode estar vazio).
3.  Crie o arquivo principal do jogo: `src/starlight/games/square_hunter/main.py`

---

## 💻 Parte 2: O Código do Jogo

O novo framework `StarlightGame` simplifica drasticamente a criação de jogos. Em vez de configurar manualmente a janela, o áudio e o loop principal, você simplesmente herda da classe `StarlightGame` e preenche os métodos do ciclo de vida.

Abra o arquivo `main.py` e adicione o seguinte código:

```python
import random
from starlight.engine import StarlightGame
from starlight.engine.core_math import Vec2

# Nossa classe principal do jogo herda de StarlightGame
class SquareHunter(StarlightGame):
    """
    Um jogo simples onde o jogador controla um círculo para "caçar" um quadrado.
    """

    # O dicionário CONFIG configura a janela e os assets automaticamente.
    CONFIG = {
        "title": "Square Hunter",
        "width": 1280,
        "height": 720,
        "sounds": ["collect.wav"],  # Carrega assets/audio/collect.wav
        "music": "background.mp3"     # Toca assets/audio/background.mp3 em loop
    }

    def on_create(self):
        """
        Chamado uma vez quando o jogo inicia. Ideal para configurar o estado inicial.
        """
        # Posição do jogador (inicia no centro)
        self.player_pos = Vec2(self.window.width / 2, self.window.height / 2)
        self.player_radius = 20
        self.player_color = (0.0, 0.8, 1.0, 1.0) # Azul claro

        # Posição do alvo (quadrado)
        self.target_pos = self.get_random_position()
        self.target_size = Vec2(30, 30)
        self.target_color = (1.0, 0.8, 0.0, 1.0) # Amarelo

        # Pontuação
        self.score = 0

    def on_update(self, dt: float):
        """
        Chamado a cada frame. dt é o tempo (em segundos) desde o último frame.
        Ideal para lógica de jogo e input.
        """
        # Movimento do jogador segue o mouse
        mouse_x, mouse_y = self.input.get_mouse_pos()
        self.player_pos.x = mouse_x
        self.player_pos.y = mouse_y

        # Lógica de colisão
        dist_x = abs(self.player_pos.x - self.target_pos.x)
        dist_y = abs(self.player_pos.y - self.target_pos.y)

        if dist_x < (self.player_radius + self.target_size.x / 2) and \
           dist_y < (self.player_radius + self.target_size.y / 2):
            
            # Colisão detectada!
            self.score += 1
            self.target_pos = self.get_random_position()
            self.play_sound("collect") # Toca o som pré-carregado

            # Efeito "juice": pequeno flash na tela
            self.renderer.set_clear_color(1, 1, 1, 1)
        else:
            # Reseta a cor de fundo
            self.renderer.set_clear_color(0.05, 0.05, 0.1, 1.0)

    def on_render(self):
        """
        Chamado a cada frame, após on_update. Ideal para desenhar tudo.
        """
        # Desenha o alvo (quadrado)
        self.renderer.draw_rect(self.target_pos, self.target_size, self.target_color)

        # Desenha o jogador (círculo)
        self.renderer.draw_circle(self.player_pos, self.player_radius, self.player_color)

        # Desenha a pontuação na tela
        score_text = f"Score: {self.score}"
        self.renderer.draw_text(score_text, 10, 10, font_size=32, color=(1, 1, 1, 1))

    def get_random_position(self) -> Vec2:
        """Função utilitária para gerar uma nova posição para o alvo."""
        x = random.uniform(50, self.window.width - 50)
        y = random.uniform(50, self.window.height - 50)
        return Vec2(x, y)

# Ponto de entrada: instanciar e rodar o jogo.
if __name__ == "__main__":
    game = SquareHunter()
    game.run()
```

---

## 🎮 Parte 3: Executando o Jogo

1.  Certifique-se de estar na raiz do projeto.
2.  Execute o comando no terminal:

```bash
# Adiciona a pasta 'src' ao PYTHONPATH para que a engine seja encontrada
export PYTHONPATH=$PYTHONPATH:$(pwd)/src

# Roda o jogo como um módulo
python -m starlight.games.square_hunter.main
```

### O que você deve ver:
*   Uma janela com o título "Square Hunter".
*   Um círculo azul que segue o movimento do mouse.
*   Um quadrado amarelo em uma posição aleatória.
*   Ao tocar no quadrado, ele muda de lugar, sua pontuação aumenta e um som é emitido.

---

## 🧩 Parte 4: Próximos Passos

O `StarlightGame` Framework cuida de todo o boilerplate, permitindo que você foque no que importa: a lógica do seu jogo. A partir daqui, você pode explorar:

*   **Estados de Jogo:** Use `self.push_state()` e `self.pop_state()` para criar menus e telas de game over.
*   **Efeitos:** Adicione um `PostProcess` para criar efeitos de *bloom*, *vignette* e *screenshake*.
*   **Partículas:** Use o `ParticleSystem` para criar explosões e outros efeitos visuais.