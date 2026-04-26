<!-- Este projeto é feito por IA e só o prompt é feito por um humano. -->
# 🏗️ Starlight Engine Core

O **Starlight Engine** é o motor de simulação principal da suíte **Fusion ENGINE**. Focado em renderização PBR industrial e física de alta fidelidade.

## 📁 Estrutura de Pastas

- `src/`: Implementação dos sistemas (Renderer, Physics, ECS, etc).
- `include/`: Cabeçalhos da API da engine.
- `assets/`: Recursos (Shaders, Texturas, Modelos).
- `thirdparty/`: Dependências integradas (ImGui, Sol2, EnTT, etc).

## 🧬 Tecnologias Integradas

| Biblioteca | Versão | Função |
| :--- | :--- | :--- |
| **Jolt Physics** | Master | Simulação de corpos rígidos e dinâmica |
| **EnTT** | 3.13.2 | Sistema de Componentes de Entidade (ECS) |
| **SDL2** | 2.30.0 | Gerenciamento de Janelas e Input |
| **Lua / Sol2** | 5.4.6 | Scripting e lógica de alto nível |
| **meshoptimizer** | 0.21 | Otimização de geometria para GPU |
| **cgltf** | 1.14 | Carregamento de modelos PBR (glTF) |

## 🛠️ Compilação (Build)
O motor utiliza **CMake 3.20+** e requer o compilador **MSVC 2022** (ou superior) no Windows.

### Requisitos:
- **Flag `/bigobj`**: Necessária para compilar os cabeçalhos pesados do `sol2`.
- **C++ Standard**: C++20.
- **Runtime**: Multi-threaded DLL (`/MD`).

### Comandos:
```powershell
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Debug
```

## 🕹️ Funcionalidades de Play Mode

Pressione **F2** durante a execução para alternar entre o **Edit Mode** e o **Game Mode**.
- **Edit Mode**: Câmera livre (WASD + Botão Direito), inspeção de entidades e console de logs.
- **Game Mode**: Execução completa de lógica Lua e sistemas dinâmicos.

## 🚀 Desenvolvimento Ultra-Rápido (SBA API)

Para iniciantes, a Starlight Engine oferece a **SBA (Starlight Beginner API)**, permitindo criar jogos em Lua com comandos simples:

```lua
function OnStart()
    player = Object("Player", 0, 0, 0)
    SetColor(player, 0, 1, 1)
end

function OnUpdate(dt)
    if IsDown("up") then Move(player, 0, 0, -10 * dt) end
end
```

Veja `assets/scripts/beginner_tutorial.lua`.

## 🛠️ Como Desenvolver seu Primeiro Jogo

### 1. Criar um Novo Projeto
Use o script de automação para gerar a estrutura base do seu jogo:
```powershell
./create_project.ps1 -ProjectName "MeuPrimeiroJogo"
```
Isso criará uma pasta ao lado da `StarlightEngine` com o código inicial e configuração CMake.

### 2. Estrutura do C++ (`src/main.cpp`)
Seu jogo é definido por uma `Scene`. Sobrescreva os métodos principais:
- `OnEnter()`: Inicialização de objetos e recursos.
- `OnUpdate(dt)`: Lógica de jogo a cada frame.
- `OnRender()`: (Opcional) Comandos extras de desenho.

### 3. Usando Scripts Lua para Gameplay
A engine carrega automaticamente o arquivo `assets/scripts/main.lua`. Você pode criar entidades e manipular componentes de forma simples:

```lua
function OnStart()
    -- Criar uma entidade
    local player = Entity.Create()
    local t = player:GetTransform()
    t.position = vec3(0, 5, 0)

    -- Adicionar um visual
    local m = player:GetMesh()
    m.albedo = vec3(0, 1, 0) -- Verde
end

function OnUpdate(dt)
    -- Lógica de rotação ou movimento aqui
end
```

### 4. Fluxo de Trabalho com o Editor
1. Compile seu projeto e execute o executável.
2. Pressione **F2** para abrir o **Studio Inspector**.
3. Selecione entidades na hierarquia para ajustar posição, rotação e materiais em tempo real.
4. As alterações podem ser salvas ou aplicadas diretamente no script Lua.

---
*Este submódulo faz parte do ecossistema Fusion ENGINE. Licenciado sob a [Licença MIT](LICENSE).*  


NOTA DO CORNO QUE FEZ O PROJETO 

Fiz esse projeto no meu tempo livre no ultimo 1 ano só  uma ideia que eu tive e fui ver se era possivel fazer uma engine de jogos usando só inteligencia articial e minha pouca esperiência com prompt de ia depois de muito errar e recomeçar acho que ja da para fazer algumas coisas com o estado atual do projeto feitos por ia e usando a propria engine tem uma serie de joogos arcade 2d simples executando o GameSuite que está na pasta examples/StarlightShowcase e uma simulação 3d simples tambem na pasta examples vejam ate onde da para ir com os recursos atuais da engine me falta conhecimento para estrair melhores resultados com o projeto  não sei se isso sea útil para alguém, mas se for e você quizer  me deixe uma doação por ser gerado por ia só com minha supervisão e eu escrevendo sentenas  de milhares de linhas de prompt o projeto é open source, não lembro quantas ias usei, mas foram muitas e muitas o pix para doação é 5af3d0ff-4200-4613-9455-ee36a06737bc  obrigado e espero que minha ideia maluca seja útil para alguém
