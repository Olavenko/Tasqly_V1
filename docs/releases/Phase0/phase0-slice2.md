# 📦 Release Notes — Phase 0 / Slice 2

## 🚀 Features Delivered
- ✅ **Settings Manager (AppSettingsManager)**
  - INI persistence with QSettings backend.
  - Debounced saves (250ms) and corruption recovery.
  - Exposed to QML via `App.Settings`.

- ✅ **Theme Manager (ThemeManager)**
  - Runtime Light/Dark palettes with validated tokens.
  - QML-friendly API via `App.Theme`.

- ✅ **UseCases**
  - `LoadUserSettings` → Apply persisted settings at startup.
  - `SaveUserSettings` → Explicit save of runtime state.
  - `SwitchTheme` → Toggle theme + persist immediately.

- ✅ **QML Facades**
  - Registered via `QmlSingletonProvider`:
    - `App.Settings`
    - `App.Theme`
    - `App.Log`
    - `App.Notifier`
  - Enum `ThemeMode` exposed to QML.

- ✅ **Settings Page (Stub)**
  - Toggle between Light/Dark with live palette preview.
  - Persists across restart.

## 🧪 Testing
- ✅ Unit Tests (AppSettingsManager, ThemeManager).
- ✅ Integration Tests (theme persistence across restart).
- ✅ Manual Test (stress toggle ×50) — *Pending full UI navigation*.

## ⚙️ Deployment
- Features enabled:
  - `features.settings` → **ON**
  - `features.theme` → **ON**

## 📝 Summary
Settings & Theme subsystems are now fully integrated and exposed to QML.  
This completes Slice 2 deliverables.
