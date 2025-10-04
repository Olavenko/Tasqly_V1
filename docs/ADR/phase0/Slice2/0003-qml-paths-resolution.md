# ADR 0003: QML-Paths-Resolution

- **Status**: Accepted
- **Date**: 2025-09-06
- **Owners**: Tasqly Team
- **Related**: Phase0 — Slice3 (Navigation, Loader & Fallback)
- **Tags**: qml, paths, stackview, module

---

## Context
StackView failed to resolve `SettingsPage.qml` when using relative/qrc paths.

## Decision
Use `qt_add_qml_module` (URI `Tasqly_V1 1.0`) to register QML files. Load pages via `SettingsPage {}` instead of file paths.

## Consequences
- ✅ Cleaner QML code, no fragile paths
- ✅ Pages are part of QML module, discoverable in IDE/Design Studio
- ⚠️ Requires maintaining `qt_add_qml_module` entries
