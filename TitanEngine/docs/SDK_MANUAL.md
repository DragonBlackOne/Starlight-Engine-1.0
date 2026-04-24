# TitanEngine SDK: Manual de Desenvolvimento Comercial 📑

// Este projeto é feito por IA e só o prompt é feito por um humano.

Bem-vindo ao manual oficial do **TitanEngine SDK**. Este documento ensina como utilizar o motor para criar, proteger e distribuir jogos profissionais.

---

## 🏁 1. Iniciando um Novo Projeto
O TitanEngine fornece um script automatizado para criar a estrutura básica de um jogo comercial.

```powershell
powershell -ExecutionPolicy Bypass -File ./create_project.ps1 -ProjectName "MeuJogoHeroico"
```

Isso criará uma pasta ao lado do SDK com:
- `src/main.cpp`: O ponto de entrada do seu jogo.
- `assets/`: Onde você coloca texturas, modelos e sons.
- `CMakeLists.txt`: Configuração de build pré-otimizada.

---

## 🏛️ 2. Arquitetura de Cenas (ECS)
O motor utiliza um modelo baseado em **Cenas** e **ECS (EnTT)**.

### Exemplo de Boilerplate:
```cpp
class MyGame : public titan::Scene {
    void OnEnter() override {
        // Inicialize entidades aqui
        auto player = m_registry.create();
        m_registry.emplace<titan::Transform>(player);
    }

    void OnUpdate(float dt) override {
        // Lógica por frame
    }
};
```

---

## ⚡ 3. Otimização com SIMD Math
Para cálculos pesados (ex: transformar 10.000 partículas), utilize o namespace `titan::simd`.

```cpp
titan::simd::TransformPoints(modelMatrix, inData, outData, count);
```
*Nota: Requer CPUs compatíveis com AVX2 para performance máxima.*

---

## 🔐 4. Proteção de Assets (Sistema PAK)
Para distribuição comercial, você deve converter sua pasta `assets/` em um arquivo `.pak`.

### Como carregar um PAK:
```cpp
titan::VFSSystem::Get().LoadPak("data.pak");
// Agora todos os assets podem ser acessados via caminhos virtuais
auto texture = AssetLoader::LoadTexture("@assets/player.png");
```

---

## 🛠️ 5. Compilação e Distribuição
O SDK suporta builds em **Debug** (com editor e logs) e **Release** (otimizado para o jogador final).

1. No terminal: `cmake -B build -D CMAKE_BUILD_TYPE=Release`
2. Compile: `cmake --build build --config Release`
3. Distribua o executável junto com o arquivo `.pak` gerado.

---
**Suporte**: Consulte o código fonte em `TitanEngine/src/core` para detalhes profundos da implementação.
