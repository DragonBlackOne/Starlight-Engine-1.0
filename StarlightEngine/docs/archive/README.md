# Archive

This directory preserves historical documentation, skill prompts, and reference
material from earlier iterations of the Fusion ENGINE (formerly Starlight /
Titan). Content here is **out of date** for the current codebase and is kept
for traceability only.

## Structure

| Path | Source / Purpose |
|------|------------------|
| `legacy_reference/` | Architecture notes, blueprints, gap analyses and reports from previous engine iterations (some refer to a Rust / Bevy prototype that no longer exists). |
| `skills/` | AI prompt-engineering skill specifications (149 entries). They are not engine documentation and have no effect at compile or runtime. |

## When to consult

- Tracing design decisions made in earlier versions.
- Recovering rationale from gap analysis reports.

## When to ignore

- Implementing new features (use `../ARCHITECTURE.md`, `../MASTER_MANUAL.md`
  and `../SDK_MANUAL.md` instead).
- Onboarding new contributors (point them at the root `README.md`).
