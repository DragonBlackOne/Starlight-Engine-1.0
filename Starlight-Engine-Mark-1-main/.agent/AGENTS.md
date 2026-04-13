# Diretrizes para Agentes e Desenvolvedores (AGENTS.md)

Este documento estabelece as "Skills" e regras de ouro para o desenvolvimento da **Starlight Engine**. Todo código novo ou refatorado deve aderir estritamente a estas diretrizes para garantir a convergência arquitetural e a qualidade do código híbrido (Rust + Python).

## 1. Princípios Fundamentais (Starlight DNA)

### 1.1 Fonte da Verdade Arquitetural
Antes de tomar decisões estruturais, consulte **`docs/ARQUITETURA.md`**. Este documento descreve o "DNA" do motor. Se sua solução contradiz este documento, ela está errada.

### 1.2 O Game Loop é Sagrado
*   **Nunca** acople a simulação física (`physics.step`) diretamente à taxa de quadros de renderização (`render()`).
*   **Sempre** utilize um modelo de **Passo de Tempo Fixo (Fixed Timestep)** com acumulador para a física.
*   Lógica de Gameplay crítica deve rodar no passo fixo. Animações visuais e UI rodam no passo variável.

### 1.3 Gestão de Assets e Caminhos
*   **Evite** caminhos de arquivo hardcoded (ex: `"assets/textures/wall.png"` espalhados pelo código).
*   Prepare o terreno para um sistema de **GUIDs**. Se precisar carregar um asset, use o `ResourceManager` (ou equivalente) centralizado.
*   Considere que assets podem não estar no disco físico (podem estar em um `.pak` ou na memória). Use abstrações de I/O (`Vfs`).

### 1.4 Input: Ações, Não Teclas
*   **Não** escreva código de jogo que verifique teclas específicas (ex: `if key == 'W'`).
*   **Use** abstrações semânticas (ex: `if action == 'MoveForward'`).
*   Isso é vital para suportar remapeamento e múltiplos controles (gamepads, touch) sem reescrever a lógica do jogo.

## 2. Diretrizes Rust (Backend)

### 2.1 Organização e Estrutura
*   **Orientado a Features:** Organize módulos por funcionalidade, não por tipo de arquivo. Mantenha structs, enums e seus `impl` juntos.
*   **Structs Coesas:** Desenhe structs pequenas e focadas. Quebre dados complexos em structs menores e composíveis.

### 2.2 Tipagem e Segurança
*   **Newtype Pattern:** Use "Newtypes" para IDs e índices para evitar trocas acidentais (ex: `pub struct UserId(u64)` ao invés de apenas `u64`).
*   **Blocos Unsafe:** Todo bloco `unsafe` **DEVE** ser precedido por um comentário `// SAFETY:` explicando por que a operação é segura e quais invariantes são mantidos.
*   **Tratamento de Erros:** Use `Result<T, E>` para erros recuperáveis. Reserve `panic!` apenas para bugs irrecuperáveis ou estados impossíveis.

### 2.3 Performance
*   **Coleções:** Use `Vec` e `HashMap` como padrão. Use `Vec::with_capacity` sempre que o tamanho aproximado for conhecido para evitar realocações.
*   **ECS (Bevy):** Agrupe componentes acessados juntos nas queries para otimizar o acesso à memória (cache locality).

## 3. Diretrizes Python (Frontend/API)

### 3.1 Estilo e Convenções
*   **PEP 8:** Siga estritamente o PEP 8. Use `snake_case` para variáveis/funções e `CamelCase` para classes.
*   **Imports:** Organize imports: Biblioteca Padrão > Terceiros > Locais. Use imports absolutos.

### 3.2 Tipagem (Type Hints)
*   **Obrigatório:** **Sempre use Type Hints**. Isso melhora a legibilidade e permite análise estática.
    *   ✅ `def process_data(data: List[str]) -> int:`
    *   ❌ `def process_data(data):`
*   **Aliases:** Use `TypeAlias` para assinaturas complexas.

### 3.3 Boas Práticas e Anti-Patterns
*   **Argumentos Padrão:** **Nunca** use objetos mutáveis como argumento padrão.
    *   ❌ `def add(item, lista=[]):`
    *   ✅ `def add(item, lista: Optional[List] = None): if lista is None: lista = []`
*   **Exceções:** Nunca use `except:` nu. Capture exceções específicas (`except ValueError:`).
*   **Idioms:** Use Context Managers (`with open(...)`), List Comprehensions e f-strings sempre que possível.

## 4. Workflow e Qualidade

*   **Testes:**
    *   Rust: Use `#[test]` unitários no mesmo arquivo (módulo `tests`) e testes de integração em `tests/`.
    *   Python: Use `pytest`. Escreva testes para novas funcionalidades.
*   **Documentação e Idioma:**
    *   A documentação de arquitetura e análise de alto nível deve ser mantida em **Português**.
    *   Comentários de código e nomes de variáveis devem ser mantidos em **Inglês** (padrão internacional).
*   **Prioridades:** Ao receber uma tarefa, verifique o `docs/GAP_ANALYSIS.md` para alinhar suas ações com o Roadmap.
