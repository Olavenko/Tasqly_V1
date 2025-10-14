# ADR 0008: Namespace Isolation & Resolution Strategy in Domain/Core (Phase 1)

- **Status:** Accepted 
- **Date:** 2025-10-14 
- **Owners:** Tasqly Team     
- **Related:** Domain/Core Entities & Contracts
---

## 1. 🧠 Context

During the transition from **Phase 0 (legacy core)** to **Phase 1 (isolated domain core)**,  
the team introduced new domain entities and repository contracts (e.g., `P1_Task.h`, `P1_ITaskRepository.h`)  
under the root namespace:

```cpp
namespace tasqly::domain::core::P1 { ... }
```

However, some legacy or transitional files (especially `.cpp` implementations)  
still referenced the older namespace path:

```cpp
using namespace tasqly::domain::core;
```

This caused subtle linkage and include-resolution issues during the build process.  
Specifically, when both `Task.h` (old) and `P1_Task.h` (new) coexisted in the same include paths.

---

## 2. ⚠️ Problem Statement

Symptoms observed during the migration:

- Build succeeded but linked against the **old `Task` definition**.
- Functions such as `TaskMapper::toDto()` crashed at runtime (segmentation fault).  
- Debug symbols pointed to mismatched RTTI types:
  ```
  tasqly::domain::core::Task ≠ tasqly::domain::core::P1::Task
  ```
- Changes to new entities (`P1_Task`) were not reflected in linked tests.

This created **a phantom include binding** — the compiler silently resolved headers  
from the wrong namespace path due to overlapping directory structures.

---

## 3. 🧩 Root Cause

1. Multiple headers with similar names existed:
   - `domain/core/entities/Task.h` (old)
   - `domain/core/entities/P1_Task.h` (new)

2. Inconsistent namespace usage:
   ```cpp
   using namespace tasqly::domain::core;   // OLD
   ```
   instead of
   ```cpp
   using namespace tasqly::domain::core::P1; // NEW
   ```

3. Some CMake target include paths still exposed `domain/core/entities` without the `P1_` isolation.

4. The linker didn’t detect mismatch because both structs had identical field layouts.

---

## 4. 🔍 Debugging Process (Using GDB)

The root cause was confirmed using **GDB runtime introspection**:

### Step 1 — Run the test binary
```bash
gdb ./build/mingw-debug/TasqlyTestsRunner.exe
```

### Step 2 — Break before crash
```gdb
(gdb) break TaskMapper::toDto
(gdb) run
```

### Step 3 — Inspect type info
```gdb
(gdb) print typeid(task).name
```
**Output:**
```
tasqly::domain::core::Task
```
❌ This revealed that the object belonged to the *old* namespace.

### Step 4 — Trace include origin
```gdb
(gdb) info sources
```
and verified via:
```bash
grep -R "Task.h" build/CMakeFiles | grep include
```
which showed the compiler was resolving the wrong header file.

### Step 5 — Validate correct rebuild
After correcting the namespaces and headers:
```gdb
(gdb) print typeid(task).name
```
✅ Output:
```
tasqly::domain::core::v1::Task
```

---

## 5. 🛠️ Solution

### ✅ 5.1 Unified Namespace Convention

All new Phase 1 domain files now live under:
```cpp
namespace tasqly::domain::core::v1 { ... }
```

This replaced older mixed prefixes like `P1_` and `tasqly::domain::core::P1`.

### ✅ 5.2 Header & Source Synchronization

Each `.cpp` was verified to include and use the same namespace as its `.h` file, e.g.:

```cpp
// domain/core/mappers/P1_TaskMapper.cpp
namespace tasqly::domain::core::v1 {
    ...
}
```

No `using namespace` directives from parent scopes are permitted in `.cpp` files.

### ✅ 5.3 Build Isolation

CMake include paths were refactored:
```cmake
target_include_directories(TasqlyDomainCore PRIVATE
    ${CMAKE_SOURCE_DIR}/src/domain/core
)
```
and the old include directory `domain/core/entities/` was removed from the global scope.

### ✅ 5.4 Diagnostic Verification

- Recompiled all affected units (`Task`, `TaskMapper`, `ITaskRepository`, `FakeTaskRepository`).
- Added `Diagnostic Trace Build` for `TaskMapper` using `TRACE()` macros.
- Verified via GDB that all objects originate from `tasqly::domain::core::v1`.

---

## 6. 🧩 Results

| Check | Status |
|--------|---------|
| Build integrity | ✅ Stable |
| Namespace isolation | ✅ Enforced |
| GDB RTTI consistency | ✅ Verified |
| Unit Tests | ✅ All passed |
| Integration tests | ✅ Passed under Release build |

---

## 7. 📚 Lessons Learned

- Always **namespace-isolate migration phases** (`v1`, `v2`) to prevent ODR violations.  
- Never use `using namespace` in `.cpp` during transitional phases.  
- Verify actual runtime types via **GDB + `typeid`** when header confusion is suspected.  
- Introduce a **namespace linter** rule in CI to detect inconsistent scopes.  
- Maintain a strict one-to-one mapping between `.h` and `.cpp` namespaces.

---

## 8. ✅ References

- Commit: `b8c2f3a (Namespace Correction + Phase1 Stabilization)`
- Docs: `/docs/adr/phase1/domain-namespace-isolation.md`
- Related ADRs: `ADR-2025-10-09-TaskMapper-Diagnostics.md`
