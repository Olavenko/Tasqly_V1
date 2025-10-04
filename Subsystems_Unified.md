# 🧩 Tasqly — Subsystems Overview (Unified)

📌 This document consolidates subsystem documentation, merging detailed descriptions with ASCII dependency diagrams.  
It aligns with the **Tasqly Master Roadmap**, including dependencies, feature flags, testing hooks, KPIs, and expansion notes.

---

## 🔗 ASCII Dependency Diagram

```text
                   ┌──────────────┐
                   │  LogManager  │
                   └───────┬──────┘
                           │
               ┌───────────▼───────────┐
               │     Error Handling    │
               └───────┬───────────────┘
                       │
        ┌──────────────▼───────────────┐
        │          Notifier            │
        │      (Toast / Fallback)      │
        └──────────────────────────────┘

 ┌───────────────┐       ┌─────────────────┐
 │ ThemeManager  │◀─────▶│ SettingsManager │
 └───────┬───────┘       └─────────────────┘
         │
         ▼
 ┌─────────────────┐
 │  Navigation     │
 │  (Service)      │
 └───────┬─────────┘
         │
         ▼
 ┌─────────────────────────┐
 │     Loading System      │
 │ (SmartPageLoader + FB)  │
 └─────────────────────────┘

 ┌─────────────────────────┐
 │   Persistence System    │
 │ (InMemory / Postgres)   │
 └───────┬─────────────────┘
         │
         ▼
 ┌─────────────────────────┐
 │    Error + Notifier     │
 └─────────────────────────┘

 CI/CD & Performance Bars → Validate ALL subsystems continuously
```

---

## 📑 Subsystems — Detailed Overview

### 1. Logging System (LogManager)
- **Phase 0 — Slice 1**
- Components: `ILogManager`, `LogManager`, masking, levels.  
- Integrations: ErrorReporter, QML Notifier, performance logging.  
- Feature flag: `features.logging`

### 2. Error Handling System
- **Phase 0 — Slice 1**
- Components: `Error`, `Result<T,Error>`, ErrorReporter.  
- Integrations: LogManager, SmartFallback, Toast.  
- Feature flag: `features.fallback`

### 3. Settings System (AppSettingsManager)
- **Phase 0 — Slice 2**
- Components: AppSettingsManager, QML facade.  
- Integrations: ThemeManager, Logging.  
- Feature flag: `features.settings`

### 4. Theme System (ThemeManager)
- **Phase 0 — Slice 2**
- Components: ThemeManager, QML bindings.  
- Integrations: SettingsManager, QML facades.  
- Feature flag: `features.theme`

### 5. Loading System (SmartPageLoader, AppLoadingManager)
- **Phase 0 — Slice 3**
- Components: PageState, SmartPageLoader, SmartFallback.  
- Extended: shimmer, retry/timeout, metrics logging.  
- Feature flags: `features.loading`, `features.loader.advanced`

### 6. Navigation System (NavigationService)
- **Phase 0 — Slice 3**
- Components: NavigationService with route guard.  
- Integrations: SmartPageLoader, Theme system.  
- Feature flag: `features.navigation`

### 7. Notifier System (Toast & Fallback Alerts)
- **Phase 0 — Slice 1+3**
- Components: Toast, SmartFallback integration.  
- Integrations: LogManager, ErrorReporter.  
- Feature flags: `features.notify`, within fallback.

### 8. Feature Flags System
- **Phase 0+ (global)**  
- Purpose: centralized control to enable/disable features.  
- Examples: logging, settings, theme, navigation, inmemory, db.postgres, planner, analytics.  
- Validated via CI matrix.

### 9. Persistence & Fallback System
- **Phase 0 — Slice 4** → InMemory repos.  
- **Phase 1 — Slice 2** → Postgres with fallback.  
- Components: InMemory repos, Postgres repos, interfaces.  
- Integrations: Error handling, Notifier.  
- Feature flags: `features.db.fallback_inmemory`, `features.db.postgres`

### 10. Performance Bars & Benchmarking
- **Phase 0+**
- Targets: Init <50ms, Log write <2ms, Theme switch <100ms, Navigation <120ms, Repo query <5ms.  
- Tools: Google Benchmark, CI/CD.  
- Logged via LogManager.

### 11. CI/CD & Testing Infrastructure
- **Phase 0+**
- Components: GitHub Actions, Unit, Integration, Snapshot, Regression.  
- Integrations: Logging, Error, Flags.  
- Evolves with roadmap.

### 12. Future Subsystems (Phase 2.5+)
- Offline cache, DB pooling, Redis caching.  
- Reminders, Scheduling, Habit/Timer/Mood systems.  

---

## 📊 Additional Documentation

### 🕑 Phase Timeline
| Phase    | Slice   | Subsystem(s)                                |
|----------|---------|---------------------------------------------|
| Phase 0  | Slice 1 | Logging, Error Handling, Notifier           |
| Phase 0  | Slice 2 | Settings, Theme                             |
| Phase 0  | Slice 3 | Loading, Navigation                         |
| Phase 0  | Slice 4 | Persistence (InMemory)                      |
| Phase 1  | Slice 2 | Persistence (Postgres + fallback)           |
| Phase 2.5|   -     | Loader UX upgrade, Performance logging,     |
|          |         | Offline cache, Pooling, Redis               |
| Phase 3+ |   -     | Reminders, Habits, Timers, Mood systems     |


### 🏳️ Feature Flags Matrix
| Subsystem      | Feature Flag(s)                                               |
|----------------|---------------------------------------------------------------|
| Logging        | `features.logging`                                            |
| Error Handling | `features.fallback`                                           |
| Settings       | `features.settings`                                           |
| Theme          | `features.theme`                                              |
| Loading        | `features.loading`, `features.loader.advanced`                |
| Navigation     | `features.navigation`                                         |
| Notifier       | `features.notify`                                             |
| Persistence    | `features.db.fallback_inmemory`, `features.db.postgres`       |
| Future Systems | `features.analytics`, `features.reminders`, `features.habits` |


### 🧪 Testing Hooks
- Logging → Smoke tests + perf benchmarks.  
- Error Handling → Unit + snapshot tests.  
- Settings → Persistence tests.  
- Theme → Snapshot + integration tests.  
- Loading → Snapshot tests (loading/empty/error).  
- Navigation → Integration tests (guards, sidebar).  
- Notifier → Snapshot tests for Toast queue.  
- Persistence → Unit + integration tests (InMemory, Postgres).  
- CI/CD → Regression, perf benchmarks.  

### ⚡ Performance KPIs
- Log write: <2ms  
- Init: <50ms  
- Theme switch: <100ms  
- Navigation: <120ms  
- Repo query (≤1k items): <5ms  
- Loader retry/timeout monitored in Phase 2.5  

### 🚀 Future Expansion Notes
- Loader UX Upgrade, Offline Cache, DB Pooling, Redis, Reminders, Habits, Timers, Mood systems.  

---

## 📑 Final Summary
- ✅ Core subsystems introduced early in Phase 0.  
- ✅ Dependencies + feature flags ensure modularity.  
- ✅ CI/CD validates continuously.  
- ✅ Performance targets keep UX consistent.  
- ✅ Expansion roadmap defined from Phase 2.5+ onward.  
