# ADR 0002 — FeatureFlagsManager (Runtime vs. CMake flags)

- **Date:** 2025-09-04  
- **Phase:** 0 (Slice 1 — Logging & Error Backbone)  
- **Status:** Accepted  
- **Authors:** Mohamed Ali  

---

## 🎯 Context
Initially, feature flags such as `features.logging` were considered for activation via:
1. **CMake Options** (e.g., `-DFEATURE_LOGGING=ON`)  
2. **CMakePresets.json** (preset-based flags)  
3. **Toolchain Definitions** (environment-specific)  

However, all of these approaches only allow **compile-time toggling**.  
This conflicts with our roadmap where feature flags must be **runtime configurable** and later integrated with `AppSettingsManager`.

---

## 🤔 Decision
We introduced a new subsystem: **FeatureFlagsManager**.  
- Implemented in `src/app/settings/FeatureFlagsManager.{h,cpp}`  
- Exposed to QML as a singleton `Features`  
- Provides methods:
  - `bool isEnabled(QString key)`  
  - `void setFeature(QString key, bool value)`  
- Defaults to `true` for core subsystems (`features.logging`, `features.settings`, etc.)  
- Integrated into:
  - `LogManager::log()` → skips logging if disabled  
  - `ErrorReporter::report()` → always emits toast, logs only if enabled  

---

## ✅ Consequences

### Benefits
- **Runtime flexibility:** toggle subsystems without rebuild.  
- **Testability:** CI can cover scenarios with logging enabled/disabled.  
- **Consistency:** aligns with Subsystems doc (`features.*` per subsystem).  
- **Future Integration:** will connect to `AppSettingsManager` in Slice 2.  

### Drawbacks
- Slight runtime overhead (QHash lookup).  
- New subsystem to maintain.  

---

## 📌 Status in Roadmap
- **Slice 1:** FeatureFlagsManager with static defaults, integrated into LogManager + ErrorReporter.  
- **Slice 2:** Connect with `AppSettingsManager` for persistent runtime configuration.  
- **Later:** Provide QML UI to toggle features dynamically.  

---

## 📎 References
- [Subsystems Overview (Enhanced)](../../../../Subsystems_EN_Enhanced_ASCII.md)  
- [Slice 1 Design](../../../../Diagrams-README.md)  
