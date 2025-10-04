# 🧪 Testing Standards — Tasqly

This document defines the full testing strategy for the Tasqly project.  
It unifies all testing types (C++, QML, benchmarks, stress, static analysis) and connects them to their respective CMake targets.  
All tests are part of the **Definition of Done**: no feature or bugfix is complete without tests.  

---

## 🔹 1. General Rules
- All new features must include at least one test.  
- All tests must run automatically in CI.  
- No merge is allowed if any test fails.  
- Coverage thresholds must be respected as defined in [PROJECT_QUALITY_POLICY.md](../PROJECT_QUALITY_POLICY.md).  
- Every bugfix must add a regression test to prevent recurrence.  

---

## 🔹 2. C++ Unit & Integration Tests
**Target:** `TasqlyTestsRunner`  

- Covers unit tests and integration tests for core logic.  
- Based on **GoogleTest** and **Qt Test**.  
- Uses `gtest_discover_tests` for automatic test discovery.  
- Naming convention: `test_<Scenario>`.  

**Example (GoogleTest):**
```cpp
TEST(TaskServiceTest, AddTask) {
    TaskService service;
    auto id = service.addTask("Buy groceries");
    EXPECT_FALSE(id.empty());
}
```

---

## 🔹 3. QML UI Tests
**Target:** `TasqlyQmlTestsRunner` (to be added in CMake)  

- Runs QML test cases via `qmltestrunner`.  
- Every QML component/page must have at least one test case.  
- Naming convention: `<ComponentName>Test.qml`.  

**Example (QtTest):**
```qml
TestCase {
    name: "SidebarTest"
    Sidebar { id: sidebar }
    function test_defaultVisible() {
        compare(sidebar.visible, true)
    }
}
```

---

## 🔹 4. Benchmarks
**Target:** `TasqlyBenchmarksRunner`  

- Measures performance of core functions/classes.  
- Based on **Google Benchmark**.  
- Benchmarks must cover Empty/Medium/Stress datasets.  

**Example:**
```cpp
static void BM_StringCreation(benchmark::State& state) {
    for (auto _ : state) {
        std::string empty_string;
        benchmark::DoNotOptimize(empty_string);
    }
}
BENCHMARK(BM_StringCreation);
```

---

## 🔹 5. Integration Tests
### a) C++ ↔ QML Boundary
- Ensures QML correctly binds to C++ classes (`Q_PROPERTY`, signals/slots).  
- Example: Add Task in C++ → visible in QML ListView.  

### b) Database (PostgreSQL)
- Ensures infrastructure layer interacts correctly with DB.  
- Example: insert/select/update/delete tasks against test DB schema.  
- CI should spin up a temporary PostgreSQL instance (e.g., Docker).  

---

## 🔹 6. End-to-End Tests (E2E)
- Simulate full user flows across the app.  
- Example:
  1. Launch application.  
  2. Click "Add Task".  
  3. Enter "Buy groceries".  
  4. Save → Task appears in list.  
- Tools: Qt Quick Test, or UI automation (Squish).  

---

## 🔹 7. Regression Tests
- Every bug fix must include a regression test.  
- Example: If "empty task title" bug is fixed → add test ensuring empty strings are rejected.  

---

## 🔹 8. Static Analysis (Code Quality Gates)
**Targets to add in CMake:**  
- `clang_tidy_check` → runs clang-tidy with `.clang-tidy`.  
- `cppcheck_analysis` → runs cppcheck with `cppcheck-suppressions.txt`.  
- `format_check` → verifies style with `.clang-format`.  

**Rule:** No merge is allowed if there are non-suppressed violations.  

Config files:  
- [.clang-format](../.clang-format)  
- [.clang-tidy](../.clang-tidy)  
- [cppcheck-suppressions.txt](../cppcheck-suppressions.txt)  

---

## 🔹 9. Load / Stress Tests
**Target:** `TasqlyStressTestsRunner` (to be added in CMake)  

- Validates system performance under heavy load.  
- Examples:  
  - Insert 10,000 tasks into DB → latency < X ms.  
  - Open 20 QML pages sequentially → app stays responsive.  
  - Simulate 100 concurrent signals → no crash.  
- Should run as a **nightly job** in CI (not every commit).  

---

## 🔹 10. Test Doubles (Fakes, Mocks, Stubs, Spies)
- All external dependencies (Clock, UUIDs, Database connections, Network calls) must have **Fake or Mock equivalents** for tests.  
- Fakes must live under `tests/fakes/`.  
- Production code must never include files from `tests/fakes/`.  

**Examples:**  
- `FakeClock.h` → controlled time for deterministic tests.  
- `FakeUuidGen.h` → predictable UUIDs instead of random.  

---

## 🔹 11. Maintenance & References
- Review test suite at every **Phase end**.  
- Remove obsolete tests and update snapshot tests.  
- Keep regression tests for all fixed bugs.  
- References:  
  - [PROJECT_QUALITY_POLICY.md](../PROJECT_QUALITY_POLICY.md)  

---
