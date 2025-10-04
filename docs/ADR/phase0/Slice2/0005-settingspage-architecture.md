# ADR 0005: SettingsPage-Architecture

- **Status**: Accepted
- **Date**: 2025-09-06
- **Owners**: Tasqly Team
- **Related**: Phase0 — Slice2 (Theme & Settings Integration)
- **Tags**: qml, settings, theme, architecture

---

## Context
In QML, SettingsPage attempted to call `Settings.setThemeMode()`, but AppSettingsManager lacked such invokable.

## Decision
For now, QML will call `Theme.switchTo(newMode)` and then `Settings.save()`. Later slice will add `setThemeMode()` to AppSettingsManager for cleaner design.

## Consequences
- ✅ Current slice works (theme toggles + persists)
- ⚠️ Needs refactor in future slice to align QML call with proper API
