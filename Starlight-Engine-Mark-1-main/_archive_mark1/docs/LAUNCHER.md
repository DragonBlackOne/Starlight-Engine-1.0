# 🚀 Starlight Unified Launcher - Guia do Usuário

## Visão Geral

O **Starlight Unified Launcher** é um sistema unificado que integra:
- ✨ **Starlight Engine** - Motor 3D com simulações GPU
- 🦠 **Eco Evolver** - Jogo 2D de sobrevivência e evolução

## 🎮 Como Usar

### Execução Rápida

```powershell
cd "d:\Projetos\Starlight-Engine alternative generation"
.\venv\Scripts\python.exe launcher.py
```

### Menu de Opções

Quando executado, você verá um menu com as seguintes opções:

```
[1] 🌟 Tech Demo - GPU Physics
    → Demonstração interativa das simulações GPU
    → Teclas 1/2/3 para toggle de sistemas
    → ~20 FPS com 3 sistemas ativos

[2] 🌲 Forest Demo - Procedural Forest
    → 5,000 árvores com animação de vento
    → GPU instancing (1 draw call)
    → ~60 FPS performance

[3] 🦠 Eco Evolver - Survival Game
    → Jogo 2D de sobrevivência
    → Sistema de evolução
    → NPCs com IA, múltiplos biomas

[4] 🎯 All GPU Systems - Full Showcase
    → Particles + Fluid + Cloth
    → Todos os sistemas simultâneos
    → ~30 FPS

[0] ❌ Sair
```

## 📦 Experiências Detalhadas

### 1. Tech Demo (demo.py)
**GPU Physics Showcase com Controles Interativos**

**Sistemas:**
- 100,000 partículas (GPU)
- 4,096 partículas de fluido (SPH)
- 2,500 vértices de tecido (mass-spring)

**Controles:**
- `WASD` - Mover câmera
- `Mouse` - Olhar ao redor
- `Space/Shift` - Subir/Descer
- `1/2/3` - Toggle Partículas/Fluido/Tecido
- `P` - Pausar
- `ESC` - Sair

### 2. Forest Demo (forest_demo.py)
**Floresta Procedural com GPU Instancing**

**Features:**
- 5,000 árvores geradas proceduralmente
- Animação de vento no vertex shader
- Área de 200x200 metros
- Performance: ~60 FPS

**Controles:**
- `WASD + Mouse` - Navegação
- `+/-` - Ajustar força do vento
- `[/]` - Ajustar velocidade do vento
- `R` - Reset wind
- `ESC` - Sair

### 3. Eco Evolver
**Jogo 2D de Sobrevivência e Evolução**

**Gameplay:**
- Controle um microrganismo
- Colete comida para evoluir
- Evite perigos ambientais
- Lute ou fuja de predadores
- Escolha suas evoluções

**Controles:**
- `Setas` - Movimento
- `Espaço` - Dash (se evoluído)
- `1/2/3` - Escolhas de evolução
- `V` - Toggle visão
- `ESC` - Menu/Sair

**Features:**
- Sistema de evolução com escolhas
- NPCs com IA (coletores, predadores)
- 2 biomas (Padrão, Pântano Escuro)
- Perigos ambientais variados
- Efeitos sonoros

### 4. Full Engine (main.py)
**Todos os Sistemas GPU Simultâneos**

Execute o engine completo com todas as simulações ativas:
- Partículas GPU
- Fluido SPH
- Simulação de tecido

## 🔧 Requisitos Técnicos

**Hardware Recomendado:**
- GPU: NVIDIA GTX 1050 ou superior
- RAM: 4GB+
- DirectX/OpenGL compatível

**Software:**
- Python 3.10+
- Windows 10/11

**Dependências Instaladas:**
- ✅ ModernGL (engine 3D)
- ✅ PyBullet (física)
- ✅ Pygame (Eco Evolver)
- ✅ NumPy, GLM, Pillow
- ✅ Lupa (Python-Lua bridge)
- ✅ ImGui (UI debug)

## 🐛 Troubleshooting

### Problema: "Module not found"
**Solução:** Reinstale as dependências
```powershell
.\venv\Scripts\pip.exe install -r requirements.txt
.\venv\Scripts\pip.exe install pygame lupa imgui-bundle
```

### Problema: Performance baixa
**Soluções:**
- Feche outros programas
- Atualize drivers da GPU
- Use os demos individuais (menor carga)

### Problema: Eco Evolver não inicia
**Solução:** Verifique se está no diretório correto
```powershell
# O launcher deve ser executado da raiz do projeto
cd "d:\Projetos\Starlight-Engine alternative generation"
```

## 📊 Performance Esperada

| Experiência | FPS Médio | GPU Load |
|-------------|-----------|----------|
| Tech Demo | 19-20 | Alto |
| Forest Demo | 55-60 | Médio |
| Eco Evolver | 60 | Baixo |
| Full Engine | 30 | Muito Alto |

*Testado em GTX 1050 2GB*

## 🎯 Próximos Passos

Após explorar as demos:
1. **Experimente Eco Evolver** - Jogue o game completo
2. **Ajuste parâmetros** - Modifique winds, counts, etc
3. **Explore o código** - Veja como funciona por dentro
4. **Crie seu próprio demo** - Use os sistemas como base

## 📝 Créditos

**Starlight Engine:**
- GPU Compute Shaders (Particles, Fluid, Cloth)
- GPU Instancing (Forest)
- ECS System
- Multithreading

**Eco Evolver:**
- 2D Survival Game
- Evolution System
- AI NPCs
- Multiple Biomes

---

**Desenvolvido como demonstração de integração entre engines 3D e jogos 2D.**
