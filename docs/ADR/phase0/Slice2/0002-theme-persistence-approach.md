# ADR 0002: Theme-Persistence-Approach

- **Status**: Accepted
- **Date**: 2025-09-06
- **Owners**: Tasqly Team
- **Related**: Phase0 — Slice2 (Theme & Settings Integration)
- **Tags**: theme, persistence, settings, ui

---

## Context
Need to handle light/dark theme and persistence across restarts. Options:
- Store palette directly in Settings
- Separate ThemeManager (runtime) and SettingsManager (persistence)

## Decision
Keep `ThemeManager` for runtime palette logic. Keep `AppSettingsManager` for persistence. Settings persist ThemeMode, not full palette.

## Consequences
- ✅ Separation of concerns (runtime vs persistence)
- ✅ Easier testing for each component
- ⚠️ Requires sync logic: ThemeManager must update from Settings on startup
