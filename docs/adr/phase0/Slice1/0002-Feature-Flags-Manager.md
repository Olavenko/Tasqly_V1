# ADR 0002 — Feature Flags Manager

- **Date:** 2025-09-03
- **Phase:** 0 (Slice 1 — Logging & Error Backbone)
- **Status:** Accepted
- **Authors:** Mohamed Ali

---

## 🎯 Context
In Slice 1, we introduced the `features.logging` flag to control whether the Logging subsystem should be active.  
There are multiple possible ways to manage feature flags:

1. **CMake Options** — compile-time only (`-DFEATURE_LOGGING=ON`).
2. **CMakePresets.json** — more flexible, but still compile-time.
3. **Toolchain Definitions** — environment-specific (e.g., mingw.cmake).
4. **Runtime Feature Manager** — managed by the app itself (QML + C++).

---

## 🤔 Decision
We decided to introduce a **FeatureFlagsManager** subsystem inside the `app/settings` layer.  
This manager exposes feature flags as **runtime configuration** accessible from both **C++** and **QML**.  
Initial implementation will use static defaults (or a JSON file), while in **Slice 2 (Settings)** it will integrate with `AppSettingsManager`.

---

## ✅ Consequences

### Benefits
- **Runtime flexibility:** Enable/disable subsystems without rebuild.
- **QML Integration:** UI can react to features via bindings (`Features.isEnabled("features.logging")`).
- **Consistency:** Aligns with Subsystems doc, where every subsystem is tied to a `features.*` flag:contentReference[oaicite:1]{index=1}.
- **Testability:** CI can run scenarios with different features toggled.

### Drawbacks
- Slight runtime overhead for lookups (negligible).
- One extra subsystem to maintain.

---

## 📌 Status in Roadmap
- **Phase 0 Slice 1:** Add ADR + stub of FeatureFlagsManager.
- **Phase 0 Slice 2:** Connect to `AppSettingsManager` for persistent feature flags.
- **Phase 1+:** Allow dynamic feature toggling via config UI.

---

## 📎 References
- [Subsystems Overview (Enhanced)](../../../../Subsystems_EN_Enhanced_ASCII.md) — Feature Flags Matrix
- [Build Policy](../../../../building.md)
