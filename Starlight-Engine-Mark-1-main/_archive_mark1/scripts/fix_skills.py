import os

skills_dir = '.agent/skills'
count = 0

for folder in os.listdir(skills_dir):
    skill_path = os.path.join(skills_dir, folder, 'SKILL.md')
    if os.path.exists(skill_path):
        with open(skill_path, 'r', encoding='utf-8') as f:
            content = f.read()

        changed = False
        if ' ackend.pyd' in content:
            content = content.replace(' ackend.pyd', '`backend.pyd`')
            changed = True
        if ' starlight.framework' in content:
            content = content.replace(' starlight.framework', '`starlight.framework`')
            changed = True
        if ' ackend module' in content:
            content = content.replace(' ackend module', '`backend` module')
            changed = True
            
        if changed:
            with open(skill_path, 'w', encoding='utf-8') as f:
                f.write(content)
            count += 1

print(f"{count} arquivos corrigidos com sucesso.")
