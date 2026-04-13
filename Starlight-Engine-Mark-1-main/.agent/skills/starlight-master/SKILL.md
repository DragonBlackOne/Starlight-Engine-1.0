---
name: starlight-master
description: Master Blueprint for the Starlight Engine Mark-C. Enforces absolute consistency across code, assets, and design.
---
# SKILL: starlight-master

## 1. Auditoria Obrigatória
Antes de qualquer modificação, você DEVE ler:
- `starlight_master_design_doc.md` (Padrões estéticos e físicos)
- `starlight-standard` (Padrões de codificação)

## 2. Regras de Ouro (Blindagem Mark-C)
- **Consistência Visual**: NUNCA mude a cor `COLOR_CYAN` ou `COLOR_MAGENTA`.
- **Consistência Física**: NUNCA mude o `PLAYER_SPD` ou `BALL_SPD` sem aviso prévio.
- **Data Integrity (Saves)**: NUNCA serialize dados de save escrevendo bruto em disco (`fopen "wb"`). É obrigatório o uso do Módulo Transacional de C (`write into .tmp -> fclose -> rename`) para impedir corrupção total do arquivo por crash de hardware.
- **Memory Safety (Limpeza Global)**: NUNCA renderize/initialize Texturas (`glGenTextures`), Fontes TTS ou canais estáticos de Áudio (`miniaudio`) sem obrigatoriamente acoplar as funções nativas de destruição à Callback `.on_shutdown` do contexto *Starlight Framework (SLF)*.
- **Thread Blocking UI**: PROIBIDO invocar nativos de pausa síncronos da janela (como `SDL_Delay`) dentro da malha das funções de UI/Menu para Debounce. A captura de input pontual DEVE ser registrada externamente via `starlight_is_action_just_pressed()` a todo instante.

## 3. Estrutura de Arquivos
Siga o Layout Mark-C:
- `src/`: Código fonte.
- `assets/`: Ativos padronizados.
- `.agent/skills/`: Este manual.

## 4. Checklist de Turno
- [ ] O código usa `#define` para todas as novas constantes?
- [ ] Os nomes de funções seguem o prefixo `starlight_`?
- [ ] A UI mantém o tema Neon padronizado?
- [ ] Novo Walkthrough foi criado?
