# Pesquisa de Motores 3D Leves para Hardware Low-End (GTX 1050 2GB + R5 5500)

O hardware alvo (AMD Ryzen 5 5500 associado a uma NVIDIA GTX 1050 de 2GB de VRAM) é extremamente capaz, mas exige respeito absoluto ao orçamento modesto de memória de vídeo. Tentar usar engines focadas em ecossistemas fechados Triple-A modernos sem profundo conhecimento do pipeline pode resultar em travamentos por gargalo de VRAM.

Aqui estão as melhores opções de mercado que respeitam essa configuração e permitem criar jogos 3D comerciais sem engasgar o hardware:

## 1. Godot Engine (Recomendação Principal)
O **Godot** (versões 3.x e a moderna 4.x usando renderer Compatibility/Mobile) é o rei indiscutível da leveza.
- **Tamanho e Peso**: O editor inteiro tem menos de 100 MB, abre em segundos e não consome centenas de megabytes de VRAM apenas para manter as ferramentas rodando.
- **Workflow 3D**: O pipeline de renderização foca na eficiência. Em vez de entupir a placa gráfica com "Megatextures" ou BVHs pesadas nativamente, ele dá ferramentas ao dev para fazer *GridMaps* e *Baking* de lightmaps assados na CPU.
- **Linguagem**: GDScript (muito similar a Python) ou C#.
- **Performance na GTX 1050**: Você facilmente baterá 60 FPS ou até mais de 100 FPS (dependendo dos shaders) com PBR simples. 

## 2. Unity (Usando o URP - Universal Render Pipeline)
A **Unity** é historicamente pesada quando se usa o pipeline HDRP (High Definition Render Pipeline), que destruiria os 2GB da 1050. No entanto, se configurada corretamente, é incrivelmente leve.
- **O Segredo (URP)**: Usando o *Universal Render Pipeline* (antigamente LWRP), a Unity é otimizada para celulares e PCs low-end.
- **Gráficos**: O URP permite Forward Rendering (ideal para placas sem muita banda de VRAM) com Sombras Cascata suaves.
- **Atenção ao Editor**: O editor da Unity em si é pesado. Ele pode consumir até 1GB ou mais de memória enquanto você o opera, deixando pouco para testar o jogo dentro da interface. No entanto, o jogo exportado rodará liso na GTX 1050.

## 3. Defold
Originalmente um motor focado em mobile da King (Candy Crush), o **Defold** tornou-se open-source e expandiu suas capacidades 3D.
- **Leveza Extrema**: Possivelmente o motor mais leve e otimizado desta lista em termo de tamanho de build.
- **Foco em 3D Simples**: Muito usado para jogos "PS1-style" ou Low Poly em 3D. 
- **Linguagem**: Lua. Se você quer performance e uma comunidade focada em fazer o jogo rodar num micro-ondas, Defold é a escolha lógica.

## 4. Panda3D e Irrlicht
Motores "Old-School" puros baseados em frameworks C++/Python. 
- **Panda3D**: Desenvolvido originalmente pela Disney, permite usar Python (assim como o nosso Starlight Engine) para a lógica e C++ por debaixo dos panos. Possui um overhead minúsculo, os jogos "nascem" já gastando meros ~30MB de memória RAM.
- No entanto, dominar a iluminação moderna nesses motores exige escrever shaders quase na unha, em contraste aos nós visuais das engines mais modernas.

## E a Nossa Engine? (Starlight Mark 1)
O **Starlight Engine**, rodando o backend WGPU em **Rust**, foi exatamente projetado para se juntar a esta lista de elite. 
- Ao abraçar o Forward Rendering e rejeitar propositalmente tecnologias que estrangulam VRAM (como buffers gigantescos do Deferred e Hardware Raytracing), a Starlight mira proporcionar o workflow de scripting Python elegante (como no Panda3D e Godot) com a eficiência implacável da compilação LLVM do Rust na GPU compatível da GTX 1050.
