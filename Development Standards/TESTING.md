# 🧪 Testing Guide — Tasqly

This document explains how to run and extend tests for the Tasqly project.

---

## Test Layout
- **Unit tests (`tests/unit/`)**  
  Fine‑grained tests for individual classes and components.

- **Integration tests (`tests/integration/`)**  
  Verify collaboration between subsystems (e.g., logging, navigation, theme).

- **Fakes (`tests/fakes/`)**  
  Lightweight implementations of interfaces for test isolation (e.g., `FakeClock`, `FakeUuidGen`).

- **Test Runner (`tests/test_main.cpp`)**  
  Central entry point for all test binaries.

---

## Running Tests

### Run All Tests
```bash
ctest --preset=test-gcc-debug --output-on-failure
```

### Run a Single Test File
```bash
ctest --preset=test-gcc-debug -R test_navigation_service -VV
```

### Run Tests by Name
```bash
ctest --preset=test-gcc-debug -R RotationPolicyTests -VV
```

---

## Adding New Tests
1. Create the test file under the appropriate directory (`unit/`, `integration/`, etc.).  
2. Follow the existing naming convention: `test_<feature>.cpp`.  
3. Use **GoogleTest** as the framework.  
4. Add any required fakes under `tests/fakes/`.  
5. Ensure new test files are picked up in `CMakeLists.txt`.  

---

## Guidelines
- Keep unit tests **fast and isolated**.  
- Use integration tests for cross‑module behavior.  
- Document new test suites in this file when significant features are added.  
