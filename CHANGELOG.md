# Changelog

All notable changes to the **Fusion ENGINE** ecosystem are documented in this
file. The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and the project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Changed
- Repository audit and cleanup: hardened `.gitignore`, added `.gitattributes`,
  archived legacy reference material under `StarlightEngine/docs/archive/`.

### Planned
- Build system unification: parameterize `DEPS_DIR`, extract shared CMake
  helper, deduplicate the four game `CMakeLists.txt`.
- Fix runtime shader lookup bug.
- Track and enable `StarlightEngine/tests/` in CI.
- C++ source audit (dead code, unused includes, system registration).

## [1.0.6] - 2026

### Added
- SBA v2.0 Lua framework (`core.lua`, `sba_bridge.lua`).
- Showcase example, four game projects (Pong, Snake, Tetris, CapitalOdyssey).
- Documentation overhaul with full bilingual support (PT-BR / EN).

## [1.0.5] and earlier

See git history: `git log --oneline --all`.
