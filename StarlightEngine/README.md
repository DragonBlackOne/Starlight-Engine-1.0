<!-- Este projeto é feito por IA e só o prompt é feito por um humano. -->
# 🌟 Starlight Engine (SBA 2.0 & PBR/HDR Edition)

O **Starlight Engine** é o motor de simulação principal da suíte **Fusion ENGINE**, agora elevado a um novo patamar de fidelidade visual e facilidade de desenvolvimento.

## ✨ Novidades da Versão 2.1 (Odyssey Edition)
*   **Modular RenderGraph**: Pipeline de renderização totalmente desacoplado e extensível.
*   **Cascaded Shadow Maps (CSM)**: Sombras de alta fidelidade com múltiplas cascatas para cenários abertos.
*   **Pipeline HDR/PBR Moderno**: Renderização fisicamente correta (Cook-Torrance BRDF) com suporte a IBL (Image Based Lighting).
*   **Post-Processing Elite**: SSAO (Ambient Occlusion), Tonemapping ACES, Bloom e Motion Blur.
*   **SBA 2.0 (Starlight Beginner API)**: API organizada em namespaces (`World`, `Graphics`, `Audio`, `Input`) para um desenvolvimento intuitivo e profissional.
*   **Showcase Odyssey**: Demonstração técnica integrada com física Jolt estável e animações Ozz.

## 🚀 Como Desenvolver (SBA 2.0)
A API foi simplificada para que você possa criar mundos complexos com poucas linhas:

```lua
function OnStart()
    -- Spawnar um sol brilhante com Sombras Cascata
    Luz = World.Spawn("Sun", 0, 50, 0)
    World.SetLight(Luz, 1.0, 0.9, 0.5, 2000) -- Intensidade HDR
    
    -- Criar um objeto metálico com PBR
    Player = World.Spawn("Sphere", 0, 2, 0)
    World.SetMaterial(Player, "Metal", 0.1) -- Rugosidade baixa
end
```

## 🧬 Tecnologias Integradas

| Biblioteca | Versão | Função |
| :--- | :--- | :--- |
| **Jolt Physics** | 5.5.0 | Simulação de corpos rígidos (Estabilizada) |
| **EnTT** | 3.16.0 | Sistema de Componentes de Entidade (ECS) |
| **SDL2** | 2.30.11 | Gerenciamento de Janelas e Input |
| **Lua / Sol2** | 5.4.7 / 3.5.0 | Scripting e lógica de alto nível |
| **ozz-animation** | 0.16.0 | Sistema de animação esquelética profissional |
| **meshoptimizer** | 1.1 | Otimização de geometria para GPU |
| **cgltf** | 1.15 | Carregamento de modelos PBR (glTF) |

## 🕹️ Funcionalidades de Play Mode
Pressione **F2** durante a execução para abrir o **Studio Inspector**.
- **Edit Mode**: Câmera livre, inspeção de entidades e console.
- **Game Mode**: Execução total da lógica SBA 2.0.

---

### NOTA DO CORNO QUE FEZ zO PROJETO 
Fiz esse projeto no meu tempo livre no ultimo 1 ano só  uma ideia que eu tive e fui ver se era possivel fazer uma engine de jogos usando só inteligencia articial e minha pouca esperiência com prompt de ia depois de muito errar e recomeçar acho que ja da para fazer algumas coisas com o estado atual do projeto feitos por ia e usando a propria engine tem uma serie de joogos arcade 2d simples executando o GameSuite que está na pasta examples/StarlightShowcase e uma simulação 3d simples tambem na pasta examples vejam ate onde da para ir com os recursos atuais da engine me falta conhecimento para estrair melhores resultados com o projeto  não sei se isso sea útil para alguém, mas se for e você quizer  me deixe uma doação por ser gerado por ia só com minha supervisão e eu escrevendo sentenas  de milhares de linhas de prompt o projeto é open source, não lembro quantas ias usei, mas foram muitas e muitas o pix para doação é 5af3d0ff-4200-4613-9455-ee36a06737bc  obrigado e espero que minha ideia maluca seja útil para alguém.

---
*Este submódulo faz parte do ecossistema Fusion ENGINE. Licenciado sob a [Licença MIT](LICENSE).*  
