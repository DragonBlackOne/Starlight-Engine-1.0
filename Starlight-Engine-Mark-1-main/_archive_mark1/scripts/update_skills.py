import os

skills_dir = '.agent/skills'
count = 0
for folder in os.listdir(skills_dir):
    skill_path = os.path.join(skills_dir, folder, 'SKILL.md')
    if os.path.exists(skill_path):
        with open(skill_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Se ja tiver sido atualizado por nos
        if 'Starlight Engine Mark-1 Context' in content:
            continue
            
        # Append context
        footer = '\n\n## 🌌 Starlight Engine Mark-1 Context\n> **Attention Agent:** This skill applies to the **Starlight Engine Mark-1** hybrid architecture.\n> - **Performance Layer:** Rust Core (WGPU Forward+, Rapier3D, ECS) exposed via PyO3 (`backend.pyd`).\n> - **Logic Layer:** Python Scripting and DearPyGui tooling.\n> Always prioritize calling FFI Rust functions via `starlight.framework` or `backend` module before writing pure Python logic for heavy tasks.\n'
        
        with open(skill_path, 'a', encoding='utf-8') as f:
            f.write(footer)
        count += 1

print(f'{count} SKILL.md files updated with Starlight Mark-1 Context Footer.')
