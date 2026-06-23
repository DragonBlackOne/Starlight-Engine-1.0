# Fusion ENGINE // FusionFight — Visual & Aesthetic Guidelines

Este documento estabelece as diretrizes artísticas e técnicas para a manutenção e evolução da identidade visual **Retro Synthwave / Outrun** do **FusionFight** na **Fusion ENGINE**.
Todos os agentes e desenvolvedores que criarem ou modificarem códigos de renderização, cenários, interfaces ou efeitos visuais **DEVEM** seguir rigorosamente estes padrões.

---

## 1. Direção Artística: Retro Synthwave & Outrun Cyberpunk

A estética do jogo é inspirada no visual retrô-futurista dos anos 80, caracterizado por luzes neon vibrantes, grades de perspectiva 3D infinitas, pores do sol gigantes com recortes horizontais e pós-processamento de brilho difuso (bloom) espesso.

### Paleta de Cores Oficial (Neon Palette)

Evite o uso de cores primárias padrão ou tons opacos. Use apenas os códigos cromáticos da paleta curada abaixo:

| Nome da Cor | Representação Lua (R, G, B) | Representação Hex | Uso Recomendado |
| --- | --- | --- | --- |
| **Hot Magenta** | `1.0, 0.2, 0.95` | `#FF33F3` | Contornos neon do P1, grade do solo, faíscas de Kaito, destaques. |
| **Cyber Cyan** | `0.0, 0.9, 0.9` | `#00FFE6` | Contornos neon do P2, detalhes da Rin, escudos de energia, faíscas. |
| **Neon Violet** | `0.55, 0.12, 0.75` | `#8C1EC0` | Traje da Rin, prédios do fundo, sombras ambientais frias. |
| **Sunset Orange** | `1.0, 0.45, 0.05` | `#FF730D` | Sol synthwave de fundo, lanternas japonesas acesas, bloqueios. |
| **Glow Gold** | `1.0, 0.85, 0.1` | `#FFD91A` | Auras Super Ultimates, molduras de vida douradas, faíscas douradas. |
| **Dark Obsidian** | `0.04, 0.02, 0.08` | `#0A0514` | Fundos translúcidos de UI, chão líquido de vidro fosco, sombras. |

---

## 2. Regras de Pós-Processamento (Bloom & Exposure)

A atmosfera luminosa depende inteiramente do buffer de bloom da engine. As configurações ideais são:

- **Bloom**: Ativado através de `Engine.set_bloom(threshold, steps)`. O threshold deve ser mantido baixo para fazer o neon brilhar sem superexpor os elementos brancos:

  ```lua
  Engine.set_bloom(0.35, 3) -- Brilho de alta fidelidade
  ```

- **Exposure e Gamma**: Ativados via `Engine.set_exposure(exposure, gamma)`. O exposure ajusta a intensidade geral da luz e o gamma controla o contraste para manter pretos profundos e neons saturados:

  ```lua
  Engine.set_exposure(1.1, 2.0) -- Cores Outrun intensas
  ```

---

## 3. Padrão de Spritesheets e Proporções dos Personagens

Para evitar distorções de pixels e garantir que os lutadores caibam em suas caixas de colisão físicas (`pushbox`), adote o seguinte padrão de sprites:

- **Grid Vertical (4x2)**: Cada arquivo de spritesheet de lutador (`1024x1024` pixels) deve ser dividido em 4 colunas horizontais e exatamente **2 linhas verticais**, gerando quadros de **256x512** pixels por pose.
- **Aspect Ratio 1:2**: Como o quadro tem proporção de largura igual a 50% da altura, a renderização **DEVE** usar o multiplicador de `0.5` para a largura:

  ```lua
  local drawW = h * 0.5  -- Mantém a proporção pixel-perfect original
  local drawH = h
  ```

- **Mapeamento de Estados da Spritesheet**:
  - **Coluna 0**: Idle, Crouch (crouch usa frame 0 reduzido a `75%` de altura), Win (frame 1).
  - **Coluna 1**: Walk Forward & Backward (ciclo de 2 quadros), Jump (frame 1).
  - **Coluna 2**: Attack, Specials (frame 0 de startup, frame 1 de active strike).
  - **Coluna 3**: Hitstun (vibrando frame 0 e 1 rápido), Knockdown/KO (queda frame 0, no chão frame 1).

---

## 4. Efeito de Reflexo Líquido Espelhado (Obsidian Reflective Floor)

O chão do cenário deve simular vidro de obsidiana escuro com reflexo líquido distorcido em tempo real:

- **Slicing de Reflexo**: Os reflexos não devem ser desenhados como um único quad estático. Em vez disso, a sprite e os prédios do fundo devem ser fatiados em tiras verticais finas de 4px de altura.
- **Distorção Horizontal e Tempo**: Aplique um deslocamento senoidal horizontal em cada fatia baseado na sua coordenada Y e no tempo de jogo (`time.get_time()`):

  ```lua
  local shift = math.sin(time.get_time() * 7.5 + cur_y * 0.08) * 6.2
  ```

- **Fading de Opacidade (Vertical Falloff)**: A opacidade do reflexo deve diminuir linearmente à medida que se afasta da linha de horizonte/chão (`groundScreenY`), garantindo que elementos mais distantes sumam suavemente:

  ```lua
  local fade = math.max(0.0, 1.0 - (cur_y - groundScreenY) / 200.0)
  local alpha = alphaScale * 0.25 * fade
  ```

---

## 5. UI & HUD: Efeito Glassmorphism (Acrílico)

Toda a interface deve parecer feita de vidro acrílico fosco neon:

- **Fundo Translúcido**: Use fundos pretos/roxos muito escuros e semi-transparentes:

  ```lua
  gfx.draw_rounded_rect(rx, ry, rw, rh, radius, 0.04, 0.02, 0.08, 0.85)
  ```

- **Borda de Glow Neon**: Toda janela e botão deve ter uma borda brilhante e pulsante em neon.
- **Tipografia**: Use fontes grandes e em maiúsculas (Caps) com sombra preta projetada (`drop shadow`) deslocada em 2 pixels para legibilidade impecável sobre cenários de neon intensos.

---

## 6. Partículas de Impacto (Hit Sparks & Starburst)

Os impactos devem gerar feedbacks visuais satisfatórios que combinam com o peso dos golpes:

- **Hit Starburst**: Ao conectar um golpe, dispare fagulhas agulhadas finas que se expandem radialmente a partir do ponto de impacto, esticando-se e encolhendo-se dinamicamente conforme uma curva senoidal envelope (`math.sin(vida_util * math.pi)`).
- **Aura EX e Super**: Golpes especiais consumindo meter geram rastros azuis transparentes (`EX Trails`) e auras douradas (`Super Glow`) com anéis de choque (shockwaves) circulares que se expandem e desvanecem.
- **Feedback de Impacto**: Conecte tremores físicos de tela (`Rumble` ou `Earthquake`) proporcionais ao dano do golpe.
