# 🧩 Tasqly — Subsystems Overview (Enhanced + ASCII Diagram)

📌 This document summarizes all **Cross-Cutting / Supportive Subsystems** as described in  
**New_Tasqly_Master_Roadmap.md**, with extended details for roadmap alignment, dependencies, feature flags, testing hooks, and KPIs.

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

## 📊 Additional Documentation

### 🕑 Phase Timeline
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

### 🏳️ Feature Flags Matrix
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

### 🧪 Testing Hooks
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

### ⚡ Performance KPIs
- Log write: **<2ms**  
- Init: **<50ms**  
- Theme switch: **<100ms**  
- Navigation change: **<120ms**  
- Repo query (≤1k items): **<5ms**  
- Loader retry/timeout: monitored in **Phase 2.5**  

---

### 🚀 Future Expansion Notes
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
