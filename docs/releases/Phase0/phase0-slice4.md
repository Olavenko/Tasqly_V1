# 📦 Release Notes — Phase 0 / Slice 0

## 🚀 Features Delivered
- ✅ **Persistence / Repositories**
  - Introduced **In-memory repositories** (`InMemoryTaskRepository`, `InMemoryGoalRepository`).
  - Enabled via feature flag `features.inmemory = true` by default in Phase 0.
  - Supports rapid iteration without external DB dependency.

- ✅ **Testing Infrastructure**
  - Added **deterministic fakes** for `UuidGen` and `Clock`.
  - Ensures reproducible test results and eliminates flaky behavior.

## 🧪 Testing
- ✅ Unit Tests (`test_inmemory_task_repository.cpp`, `test_inmemory_goal_repository.cpp`, `test_fake_uuid_clock.cpp`).
- ✅ Integration Tests (QML TaskListPage early-phase skip mode, repository seeding toggles).
- ☐ Manual Test (UI flows blocked until TaskListPage UI Slice is implemented).

## ⚙️ Deployment
- Features enabled:
  - `features.inmemory` → **ON**

## ⚠️ Known Issues
- TaskListPage UI not yet implemented (tests in SKIP mode — see ADR-0005).
- Data is volatile (lost on restart), as persistence is in-memory only.

## 📝 Summary
Phase 0 / Slice 0 delivers **in-memory persistence** and **deterministic fakes** as the foundation for testing and early QML integration. This enables green CI pipelines and fast iteration before full UI and DB persistence are introduced.

