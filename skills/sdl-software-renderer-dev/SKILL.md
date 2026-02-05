---
name: sdl-software-renderer-dev
description: Maintain and evolve the SDL2 CPU software renderer in this repo. Use when changing src/ rendering or math code, updating docs/ research notes, or editing CMake for Windows/macOS builds and platform-specific libraries.
---

# SDL Software Renderer Dev

## Overview

Maintain the SDL2-based CPU renderer and keep docs/ research notes and multi-platform CMake builds aligned with code changes.

## Workflow

1. Read `docs/Memo.md` first to align terminology and pipeline assumptions.
2. Identify impacted areas: `src/`, `CMakeLists.txt`, `lib/SDL2/`, and any `docs/` files.
3. Implement code or build changes with platform guards as needed.
4. Update `docs/` with the new decision, algorithm, or build change.
5. Report any untested steps explicitly.

## Docs Rules

- Keep research notes and decisions in `docs/` (not in issues or only in commit messages).
- Update `docs/Memo.md` when changing rendering stages, coordinate systems, or math conventions.
- Add a date stamp (`YYYY-MM-DD`) to new sections or updates.
- When creating a new document, use a clear name like `docs/Build.md` or `docs/Shader-Notes.md` and add a short link entry in `docs/Memo.md`.

## Multi-Platform CMake

- Inspect `CMakeLists.txt` before changing build logic.
- Keep Windows and macOS branches in sync when introducing new libraries or compile options.
- Guard platform-specific logic with `CMAKE_SYSTEM_NAME`, `WIN32`, or `APPLE` checks.
- Update architecture-specific paths (`x86_64`, `arm64`) consistently with existing SDL2 layout under `lib/SDL2/`.
- If you add or change build steps, update `docs/Build.md` (create it if missing).

## CPU Shader / Software Pipeline

- Keep the software pipeline stages explicit (vertex, primitive, rasterization, fragment).
- When prototyping CPU shaders (water, fire, etc.), document inputs, outputs, and math in `docs/Shader-Notes.md` (create if missing) and link from `docs/Memo.md`.
- Preserve the OpenGL-style coordinate system assumptions already documented in `docs/Memo.md`.

## Definition of Done

- Code changes are consistent with existing math conventions.
- Docs reflect what changed and why, with dates.
- Build guidance is updated when CMake changes.
