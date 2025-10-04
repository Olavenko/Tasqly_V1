# ADR 0001: QML-Facades-vs-Direct-Infra-Exposure

- **Status**: Accepted
- **Date**: 2025-09-06
- **Owners**: Tasqly Team
- **Related**: Phase0 — Slice3 (Navigation, Loader & Fallback)
- **Tags**: qml, facades, infra, app-layer

---

## Context
Decided how to expose C++ managers (Settings, Theme, Logging, Notify) to QML. Options were:
- Expose managers directly
- Use QML singleton facades

## Decision
We expose managers via `QmlSingletonProvider` as `App.Settings`, `App.Theme`, `App.Log`, `App.Notifier`.

## Consequences
- ✅ UI can consume `App.*` directly without worrying about infra details
- ✅ Easier refactor of backend without breaking QML
- ⚠️ Slight boilerplate to register facades
