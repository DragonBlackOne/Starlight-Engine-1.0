# Guia do Usuário — Editor Visual da Fusion ENGINE

Bem-vindo ao manual do **Editor Visual da Fusion ENGINE**. O Editor é um dos pilares do desenvolvimento do ecossistema Fusion, fornecendo uma interface integrada de nível profissional (Unreal/Unity-style) para design de cenários, depuração em tempo real, ajuste de componentes e controle de física/áudio espacial.

---

## 1. Visão Geral da Interface (Layout de 4 Regiões)

O editor é estruturado em um layout de quatro quadrantes que se autoajustam e podem ser colapsados, movidos ou acoplados em abas.

```
+-----------------------------------------------------------------------+
| File  View                                   [Play] [Pause] [Stop]    |
+----------------------------------+------------------------------------+
| Hierarchy                        | Viewport (Cena)                    |
| - Entity 1 (Sprite)              |                                    |
| - Entity 2 (Camera2D)            |   +-[Gizmo Move]                   |
|                                  |   |                                |
|----------------------------------|   v                                |
| Inspector                        | [Circle overlay: Reverb Zone]      |
| [Transform]                      |                                    |
| [Sprite Renderer]                |                                    |
| [Jolt Physics]                   +------------------------------------+
| [Audio Reverb]                   | Console                            |
|                                  | [INFO] Audio active.               |
|                                  | [INFO] SBA self-tests passed.      |
|                                  | +----------------------------------|
|                                  | | > Lua Command Input              |
+----------------------------------+------------------------------------+
```

---

## 2. Guia de Funcionalidades dos Painéis

### A. Viewport (Painel de Cena)
O Viewport exibe a cena renderizada em tempo real. Ele serve tanto para editar visualmente quanto para visualizar a gameplay durante a execução do sandbox.

* **Navegação de Câmera 2D**:
  * Clique com o mouse no Viewport para ativá-lo.
  * Use as teclas `W`, `A`, `S`, `D` (ou as setas do teclado) para mover a câmera pelo cenário.
  * Use a **roda do mouse** para aproximar ou afastar o zoom (mínimo de `0.1x`, maximum de `10.0x`).
* **Manipulador por Gizmo (Move)**:
  * Quando uma entidade com `TransformComponent` está selecionada na Hierarquia, um gizmo em cruz amarela/laranja aparecerá sobre ela no Viewport.
  * Clique e arraste o gizmo com o mouse para reposicionar o objeto livremente.
* **Gizmo de Depuração de Áudio**:
  * Se a entidade selecionada contiver um `ReverbZoneComponent` ativo, dois círculos neon serão desenhados ao redor dela: um **verde** (representando a distância de início do reverb) e um **azul/cyan** (representando a distância máxima de atenuação do reverb).
* **Performance HUD (Profiler)**:
  * Quando o comando de performance está ativo (`r_showProfiler 1` ou `2`), um painel overlay neon magenta/cyan é desenhado no canto superior esquerdo do Viewport exibindo:
    * Modelo da GPU ativa e versão do driver OpenGL.
    * Taxa de FPS atual e tempo médio de frame em milissegundos.
    * Estatísticas de renderização (Draw Calls gerados e quantidade de Quads batch).
    * Total de entidades ativas na cena.
    * Histórico gráfico de latência de CPU dos subsistemas (Render, Scripts, Physics, Audio e Update).

### B. Hierarchy (Hierarquia da Cena)
Exibe a lista sequencial de todas as entidades registradas na cena atual do motor.

* **Seleção**: Clique em qualquer entidade para abrir suas propriedades no Inspector.
* **Menu de Contexto (Clique Direito)**:
  * **Create Empty Entity**: Cria um objeto vazio contendo apenas tags e transformação básica.
  * **Create Sprite Entity**: Cria um objeto com componente de imagem (Sprite).
  * **Create Camera 2D**: Instancia uma nova câmera no cenário.
  * **Create Animated Sprite**: Cria uma entidade pronta com componentes de Sprite e Animação.
  * **Instantiate Prefab...**: Abre uma janela de diálogo para você digitar o caminho de um arquivo JSON de Prefab (ex: `assets/prefabs/player.json`) e instanciar o objeto completo de forma automática.
  * **Delete Entity** (em cima de um item existente): Remove o objeto da cena.

### C. Inspector (Inspetor de Componentes)
Permite visualizar e editar todos os componentes da entidade que estiver selecionada.

* **Tag**: Altere o nome identificador da entidade.
* **Edição de Componentes**:
  * **Transform**: Edite manualmente as coordenadas X, Y e Z de Posição, Rotação (em graus) e Escala.
  * **Sprite**: Configure a cor (usando o Color Picker), pivô, flip horizontal/vertical, visibilidade e as propriedades de camadas de ordenação (`layer` e `orderInLayer`).
  * **Sprite Animation**: Acompanhe o frame atual da folha de sprites, ajuste o FPS da animação e configure se deve rodar em loop ou começar pausada.
  * **Mesh Renderer (3D)**: Controle o Albedo, Metalicidade e Rugosidade do material PBR da geometria.
  * **Point Light**: Ajuste a iluminação colorida 3D regulando intensidade e o vetor de cor RGB.
  * **Reverb Zone**: Regule as distâncias de propagação e atenuação acústica 3D (`minDistance` e `maxDistance`) e o fator de reverb (`reverbFactor`).
* **Botão Add Component...**:
  * Permite anexar novos componentes em tempo real (como corpos físicos Jolt, sprites, câmeras ou emissores de partículas) à entidade selecionada.

### D. Console e Terminal Lua
O Console unifica a visualização de logs e a inserção de comandos rápidos para testes em tempo real.

* **Filtros rápidos**: Marque ou desmarque as caixas de seleção (*Info*, *Warns*, *Errors*) para filtrar as mensagens exibidas.
* **Busca**: Digite um termo de busca no campo de pesquisa para encontrar logs específicos instantaneamente.
* **Terminal Lua (Entrada no rodapé)**:
  * Digite códigos Lua válidos diretamente e pressione **Enter** para executar na máquina virtual.
  * Exemplos de comandos úteis:
    * `Engine.spawn("Sprite")` — Spawna um novo sprite na posição de origem.
    * `print(Entity.get(1):getTag())` — Imprime no log a tag da entidade ID 1.
    * `Engine.set_time_scale(0.5)` — Ativa o modo câmera lenta (slow motion) na gameplay.

### E. Content Browser (Navegador de Arquivos)
Exibe a estrutura de pastas do seu projeto a partir do diretório raiz.
* Clique nos diretórios sinalizados por colchetes (ex: `[assets]`) para navegar pelas subpastas.
* Use o botão `< ..` para voltar ao nível de diretório anterior.

---

## 3. Fluxo de Trabalho Sandbox (Play / Pause / Stop)

O Editor funciona no modelo de **Sandbox com Restauração de Estado**:

1. **Edição**: Monte o cenário, posicione inimigos, configure a física e ajuste câmeras.
2. **Play**: Clique no botão verde **Play** no menu superior. 
   * A engine gera um snapshot em memória de todas as entidades e estados.
   * A gameplay começa a executar, os scripts Lua de controle são ativados e o motor de física Jolt assume o movimento dos corpos rígidos.
3. **Pause**: Pause a simulação a qualquer momento para inspecionar os valores dos componentes no Inspector no exato instante do congelamento.
4. **Stop**: Clique no botão vermelho **Stop**.
   * A simulação é interrompida.
   * A engine descarta o estado alterado pela gameplay física e restaura a cena exatamente ao ponto em que estava antes de você clicar em Play. 
   * Você pode continuar a editar sem se preocupar em ter "sujado" o cenário com a gameplay de testes.

---

## 4. Gerenciamento de Cenas (Save & Load)

* Para **Salvar** o cenário:
  1. Vá ao menu superior **File -> Save Scene** (ou pressione `Ctrl+S`).
  2. Digite o nome do arquivo da cena (ex: `level1.scene`) e confirme. O arquivo será salvo na pasta `assets/scenes/` do seu projeto.
* Para **Carregar** um cenário salvo:
  1. Vá ao menu superior **File -> Load Scene** (ou pressione `Ctrl+O`).
  2. Digite o nome da cena desejada e confirme. O editor carregará instantaneamente todas as entidades e suas propriedades.

---

## 5. Atalhos Rápidos e Resolução de Problemas

| Atalho | Ação |
|---|---|
| `Ctrl+Shift+Space` | **Reset Layout**: Reconstrói e reseta o layout do docking padrão Unreal-style instantaneamente. |
| `Ctrl+S` | Salvar Cena ativa. |
| `Ctrl+O` | Carregar Cena. |
| `F1` ou `` ` `` | Alternar exibição do terminal/console interno de desenvolvimento. |

### Layout Desorganizado ou Tela Preta?
Se você fechar alguma janela acidentalmente ou arrastar um painel de forma que ele fique perdido na tela, basta ir ao menu **View -> Reset Layout** ou pressionar o atalho **`Ctrl+Shift+Space`**. O editor fará uma limpeza nos buffers de memória do ImGui e reconstruirá o preset de 4 quadrantes perfeitamente na próxima frame.
