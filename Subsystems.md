# 🧩 Tasqly — Subsystems Overview (Enhanced)

📌 This document summarizes all **Cross-Cutting / Supportive Subsystems** as described in  
**New_Tasqly_Master_Roadmap.md**, with extended details for roadmap alignment, dependencies, feature flags, testing hooks, and KPIs.

---

---

## 📑 Table of Contents
1. [Root Layout](#root-layout)
2. [Notes](#notes)
3. [Build Toolchain Policy (Summary)](#build-toolchain-policy-summary)
4. [Next Steps](#next-steps)

---

## Root Layout
```
Tasqly_V1/
 ├── build/                   # Local build output (ignored in VCS)
 ├── src/                     # Application source code
 │    ├── main.cpp            # App entry point (C++20)
 │    └── Main.qml            # Root QML file
 ├── tests/                   # Unit & integration tests
 │    └── test_result.cpp     # Smoke test for Result<T,Error>
 ├── docs/                    # Documentation (UML + ADRs)
 │    ├── adr/                # Architecture Decision Records
 │    │    ├── 0001-plantuml-adoption.md
 │    │    ├── 0002-build-toolchain.md
 │    │    ├── 0003-google-test.md
 │    │    ├── 0004-ci-workflows.md
 │    │    ├── 0005-git-hygiene.md
 │    │    ├── 0006-debugger-policy.md
 │    │    └── README.md      # ADR index
 │    └── uml/                # UML Diagrams & Config
 │         ├── config/        # Shared style + includes
 │         │    └── style.puml
 │         ├── phase0/        # Phase 0 diagrams
 │         │    └── s0/
 │         │         ├── component.puml
 │         │         ├── sequence.puml
 │         │         ├── usecase.puml
 │         │         └── exports/      # Generated .svg diagrams
 │         ├── tools/        # Local tools (plantuml.jar)
 │         ├── uml-export.sh # Export script (SVG + index.html)
 │         └── README.md     # UML Authoring Guidelines
 ├── cmake/                   # Toolchain & build scripts
 │    └── toolchains/
 │         └── mingw.cmake
 ├── .clang-format            # Coding style config
 ├── .clang-tidy              # Static analysis config
 ├── .gitignore               # Git ignore rules
 ├── .gitattributes           # Git attributes (line endings, etc.)
 ├── MyAllmanClassic.xml      # Qt Creator formatting preset
 ├── CMakeLists.txt           # Root CMake config
 ├── CMakePresets.json        # Standardized CMake presets (with debuggerPresets)
 ├── .github/
 │    └── workflows/
 │         ├── ci.yml         # CI build & test (multi-platform matrix)
 │         ├── uml.yml        # UML artifact export
 │         └── uml-pages.yml  # UML GitHub Pages deployment
 └── Structure.md             # (This file)
```

---

## Notes
- **src/** → Application entry point (`main.cpp`, `Main.qml`).  
- **tests/** → Unit tests, starting with smoke test for `Result<T,Error>`.  
- **docs/adr/** → ADRs for traceability (toolchain, git hygiene, debugger, etc.).  
- **docs/uml/** → UML diagrams, with corporate style and CI export (`uml-export.sh`).  
- **.github/workflows/** → CI/CD automation (build + UML diagrams + Pages).  
- **cmake/toolchains/** → Toolchain definitions (baseline MinGW).  

---

## Build Toolchain Policy (Summary)

- **Windows (Baseline):** MSYS2 MinGW-w64 GCC 15.2 + Ninja + Qt 6.8.2 MinGW.  
- **Linux:** GCC / Clang (latest stable).  
- **Optional:** MSVC (CI only).  
- **Qt ABI constraint:** Qt-MinGW ↔ MinGW only, Qt-MSVC ↔ MSVC only.  
- **Sanitizers:** Enabled on Linux.  

**Build Method (Windows baseline):**
```powershell
rmdir /S /Q build
cmake --preset=mingw-debug
cmake --build --preset=build-debug
ctest --preset=test-debug
```

---

## Next Steps
- Future phases (phase1, phase2, …) will add new modules under `src/` and `tests/`.  
- CI workflows already cover multi-platform toolchain matrix.  
- GitHub Pages (UML Diagrams) live at:  
  👉 https://olavenko.github.io/Tasqly_V1/  

## 1. Logging System (LogManager)
- **Phase 0 — Slice 1**
- Components:
  - `ILogManager` interface
  - `LogManager` (console + rotating file logging)
  - Field masking for sensitive data
  - Unified levels: info, debug, warning, error, critical
- Integration:
  - ErrorReporter UC
  - QML Notifier (Toast)
  - Performance logging (Phase 1+, Phase 2)
- Feature flag: `features.logging`

---

## 2. Error Handling System
- **Phase 0 — Slice 1**
- Components:
  - `Error` struct
  - `Result<T,Error>` contract (used across all phases)
  - ErrorReporter UC (maps severity → log + UI toast)
- Integration:
  - LogManager
  - SmartFallback (QML)
  - Toast notifier
- Feature flag: `features.fallback`

---

## 3. Settings System (AppSettingsManager)
- **Phase 0 — Slice 2**
- Components:
  - `AppSettingsManager` (INI persistence)
  - QML facade: `App.Settings`
- Integration:
  - ThemeManager (persist theme)
  - Logging (persist log level)
- Feature flag: `features.settings`

---

## 4. Theme System (ThemeManager)
- **Phase 0 — Slice 2**
- Components:
  - `ThemeManager` (Light/Dark palettes)
  - QML binding: `App.Theme.palette.*`
- Integration:
  - SettingsManager
  - QML facades
- Feature flag: `features.theme`

---

## 5. Loading System (SmartPageLoader, AppLoadingManager)
- **Phase 0 — Slice 3**
- Components:
  - `PageState` (Ready/Loading/Empty/Error)
  - `SmartPageLoader.qml`
  - `SmartFallback.qml`
- Extended (Phase 2.5):
  - Shimmer loaders
  - Retry/Timeout logic
  - Metrics logging to LogManager
- Feature flag: `features.loading` / `features.loader.advanced`

---

## 6. Navigation System (NavigationService)
- **Phase 0 — Slice 3**
- Components:
  - `NavigationService` (with route guard)
  - Sidebar + StackView integration
- Integration:
  - SmartPageLoader
  - Theme system (UI consistency)
- Feature flag: `features.navigation`

---

## 7. Notifier System (Toast & Fallback Alerts)
- **Phase 0 — Slice 1+3**
- Components:
  - `Toast.qml` (queue, severity, auto-dismiss)
  - SmartFallback integration
- Integration:
  - LogManager
  - ErrorReporter
- Feature flag: within `features.fallback` / `features.notify`

---

## 8. Feature Flags System
- **Phase 0+ (global)**
- Purpose: centralized control to enable/disable features
- Examples:
  - logging, settings, theme, navigation, inmemory, db.postgres, planner, analytics
- Validated via CI matrix (flags on/off)

---

## 9. Persistence & Fallback System
- **Phase 0 — Slice 4 (InMemory repos)**
- **Phase 1 — Slice 2 (Postgres + fallback)**  
- Components:
  - InMemory repositories
  - Postgres repositories with runtime fallback to InMemory
  - Interfaces: `ITaskRepository`, `IGoalRepository`
- Integration:
  - Error handling (DB_INIT, NOT_FOUND)
  - Notifier (toast on fallback)
- Feature flag: `features.db.fallback_inmemory`

---

## 10. Performance Bars & Benchmarking
- **Phase 0+**
- Targets:
  - Init <50ms
  - Log write <2ms
  - Theme switch <100ms
  - Navigation change <120ms
  - Repo query <5ms (≤1k items)
- Tools:
  - Google Benchmark
  - CI/CD automated benchmarks
- Logged via LogManager (tags: PERF_UI, PERF_DB)

---

## 11. CI/CD & Testing Infrastructure
- **Phase 0+**
- Components:
  - GitHub Actions CI/CD
  - Unit, Integration, Snapshot, Regression tests
  - Smoke tests for `Result<T,Error>`
- Integration:
  - Logging + Error systems
  - Feature flags matrix
- Evolves with roadmap:
  - Phase 1 → regression & snapshot testing
  - Phase 2 → analytics validation
  - Phase 6 → QA harness

---

## 12. Future Subsystems (Phase 2.5+)
- Offline Cache (QSettings/JSON blobs)
- DB Connection Pooling + PgBouncer
- Redis Caching Layer (planner/analytics acceleration)
- Reminders & Scheduling Engine
- Habit/Timer/Mood Systems

---

# 📊 Additional Documentation

## 🔗 Dependency Map
```text
Error Handling ──▶ LogManager + Notifier (Toast + Fallback)
LogManager ──▶ QML Facade + CI/CD (logging checks)
ThemeManager ──▶ SettingsManager (persistence)
Loading System ──▶ NavigationService + LogManager + Error Handling
NavigationService ──▶ Loading System + ThemeManager
Persistence ──▶ Error Handling + Notifier
CI/CD ──▶ All Subsystems (logging, error, flags, performance)
Performance Bars ──▶ LogManager (perf tags)
```

---

## 🕑 Phase Timeline
| Phase | Slice | Subsystem(s) |
|-------|-------|--------------|
| Phase 0 | Slice 1 | Logging, Error Handling, Notifier |
| Phase 0 | Slice 2 | Settings, Theme |
| Phase 0 | Slice 3 | Loading, Navigation |
| Phase 0 | Slice 4 | Persistence (InMemory) |
| Phase 1 | Slice 2 | Persistence (Postgres + fallback) |
| Phase 2.5 | - | Loader UX upgrade, Performance logging, Offline cache, Pooling, Redis |
| Phase 3+ | - | Reminders, Habits, Timers, Mood systems |

---

## 🏳️ Feature Flags Matrix
| Subsystem        | Feature Flag(s) |
|------------------|-----------------|
| Logging          | `features.logging` |
| Error Handling   | `features.fallback` |
| Settings         | `features.settings` |
| Theme            | `features.theme` |
| Loading          | `features.loading`, `features.loader.advanced` |
| Navigation       | `features.navigation` |
| Notifier         | `features.notify` (within fallback) |
| Persistence      | `features.db.fallback_inmemory`, `features.db.postgres` |
| Future Systems   | `features.analytics`, `features.reminders`, `features.habits` |

---

## 🧪 Testing Hooks
- **Logging** → Smoke test for log levels, benchmark log latency.  
- **Error Handling** → Unit tests for `Result<T,Error>`; snapshot tests for QML fallback.  
- **Settings** → Persistence tests (INI file read/write).  
- **Theme** → Snapshot tests for Light/Dark mode; integration with QML binding.  
- **Loading** → Snapshot tests for states (loading/empty/error).  
- **Navigation** → Integration tests with route guards + sidebar/stack view.  
- **Notifier** → UI snapshot tests for Toast queue + severity display.  
- **Persistence** → Unit + integration tests for InMemory and Postgres fallback.  
- **CI/CD** → Regression + snapshot validation, perf benchmarks.  

---

## ⚡ Performance KPIs
- Log write: **<2ms**  
- Init: **<50ms**  
- Theme switch: **<100ms**  
- Navigation change: **<120ms**  
- Repo query (≤1k items): **<5ms**  
- Loader retry/timeout: monitored in **Phase 2.5**  

---

## 🚀 Future Expansion Notes
- **Loader UX Upgrade** (Phase 2.5): shimmer, retry, timeout.  
- **Offline Cache**: QSettings/JSON blobs for faster reloads.  
- **DB Pooling & PgBouncer**: scale Postgres efficiently.  
- **Redis Caching Layer**: optional for planner/analytics.  
- **Reminders Engine**: notifications & scheduling.  
- **Habit/Timer/Mood Systems**: future product expansions.  

---

## 📑 Final Summary
- ✅ Core subsystems introduced early in Phase 0.  
- ✅ Dependencies and feature flags ensure modularity.  
- ✅ CI/CD validates all subsystems continuously.  
- ✅ Performance benchmarks keep user experience consistent.  
- ✅ Future expansion paths clearly defined from Phase 2.5 onwards.  
