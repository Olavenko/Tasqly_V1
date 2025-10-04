# ADR 0004: Resources-vs-QML-Module

- **Status**: Accepted
- **Date**: 2025-09-06
- **Owners**: Tasqly Team
- **Related**: Phase0 — Slice3 (Navigation, Loader & Fallback)
- **Tags**: qml, resources, qrc, module

---

## Context
Conflict arose when QML files were included in both `resources.qrc` and `qt_add_qml_module`.

## Decision
Use `resources.qrc` only for non-QML assets (icons, images, data). QML files are handled exclusively by `qt_add_qml_module`.

## Consequences
- ✅ Avoids build/runtime duplication issues
- ✅ Clear separation: assets in qrc, QML in module
