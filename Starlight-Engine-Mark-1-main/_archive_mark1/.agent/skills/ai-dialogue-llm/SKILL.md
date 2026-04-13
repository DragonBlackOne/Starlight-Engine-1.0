---
name: ai-dialogue-llm
description: Local LLM Integration Hooks
---

# AI LLM Dialogue

## Overview
Managed by `starlight.ai.dialogue_llm.LLMDialogueAgent`.
Hooks for generating dynamic dialogue via Local LLM (e.g. Ollama/Llama3).

## Usage

```python
from starlight.ai.dialogue_llm import LLMDialogueAgent

agent = LLMDialogueAgent()

# Context is crucial for RPGs
ctx = {
    "role": "Town Guard",
    "location": "North Gate",
    "player_reputation": "Criminal"
}

response = agent.generate_response("Let me in!", context=ctx)
print(response) # "Halt! You are wanted for crimes against the jarl."
```

## Features
- **Context injection**: Pass game state to prompt.
- **History**: Maintains conversation turn history.


## 🌌 Starlight Engine Mark-1 Context
> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.
> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, Bevy ECS) exposed via PyO3 (ackend.pyd).
> - **Logic Layer:** Python Scripting and DearPyGui tooling.
> Always prioritize calling FFI Rust functions via`starlight.framework` or ackend module before writing pure Python logic for heavy tasks.
