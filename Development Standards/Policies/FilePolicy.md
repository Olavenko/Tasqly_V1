# ADR 0010 — Global Naming Template & Namespace Policy (Unified Backend + Frontend)

* **Status**: Accepted  
* **Date**: 2025-10-15  
* **Author**: Mohamed Ali  
* **Supersedes**: ADR 0006 (File Template & Coding Policy)  
* **Related**: 0003-namespace-naming-policy.md, 0002-include-path-policy.md, 0007-QML File Policy.md  

---

## Context
Throughout Phase 0 and early Phase 1, both C++ and QML layers suffered from inconsistent file naming and namespace usage.  
These inconsistencies caused linking and QML runtime conflicts (e.g., duplicate TaskPage, TaskRepo, etc.).  

This policy **replaces FilePolicy.md** and enforces a **Phase–Slice aware naming system** across all layers — C++, QML, Tests, Benchmarks, and Docs.

---

## Decision

### 1- File Naming Convention
All files **must** start with a Phase–Slice prefix:

```
P<Phase>_S<Slice>_<DescriptiveName>.<ext>
```

**Examples**

| Layer | File Type | Example |
|--------|------------|----------|
| Domain | Header | `P1_S1_TaskEntity.h` |
| Domain | Impl | `P1_S1_TaskEntity.cpp` |
| Application | Header | `P1_S1_LoadUserSettings.h` |
| Application | Impl | `P1_S1_LoadUserSettings.cpp` |
| QML Page | QML | `P1_S1_TaskPage.qml` |
| QML Component | QML | `P1_S1_TaskItem.qml` |
| Unit Test | C++ | `P1_S1_TaskEntityTest.cpp` |
| Benchmark | C++ | `P1_S1_Bench_TaskRepo.cpp` |
| ADR Doc | Markdown | `P1_S1_0010_naming-template-policy.md` |

**Rules**
- Prefix uses uppercase `P` and `S`.
- Separate phase, slice, and name with underscores `_`.
- Use PascalCase for the descriptive name.
- Extensions are always lowercase (`.cpp`, `.h`, `.qml`, `.md`).
- No spaces, no hyphens.

---

### 2️- Namespace Convention
Every namespace must encode **Phase and Slice identifiers** to prevent cross-phase collisions.

#### Format:
```cpp
namespace tasqly::p<phase>::s<slice>::<layer>::<subsystem> {
    // Implementation
}
```

#### Examples:
```cpp
namespace tasqly::p1::s1::domain::core {
    class TaskEntity;
}

namespace tasqly::p1::s1::app::settings {
    class LoadUserSettings;
}

namespace tasqly::p1::s1::infra::logging {
    class FileSink;
}
```

#### Rules:
- Prefix namespaces with lowercase `p` and `s` (e.g., `p1::s1`).
- Layer names (`domain`, `app`, `infra`) must match folder structure.
- No `using namespace` in headers; allowed only in `.cpp` scope.
- Unit tests and benchmarks must follow:
  - `tasqly::p1::s1::tests::<module>`
  - `tasqly::p1::s1::benchmarks::<module>`

---

### 3- QML Naming Rules (Frontend Alignment)

#### 3.1 File Naming
QML files follow the same phase–slice prefix rule:
```
P<Phase>_S<Slice>_<ComponentOrPageName>.qml
```

**Examples:**
- `P1_S1_Sidebar.qml`
- `P1_S1_TaskListPage.qml`
- `P1_S1_ThemeToggle.qml`

#### 3.2 ID & ObjectName
Every `id` and `objectName` in QML must include the same prefix.

**Example:**
```qml
Page {
    id: p1s1_taskListPage
    objectName: "p1s1_taskListPage"

    Button {
        id: p1s1_addTaskButton
        objectName: "p1s1_addTaskButton"
    }
}
```

**Rules**
- IDs must start with lowercase `p<phase>s<slice>_`.
- ObjectNames mirror IDs.
- Avoid generic names like `button1` or `rect2`.
- Imported modules in QML should use Phase–Slice context:
  ```qml
  import Tasqly.P1.S1.Components 1.0
  ```

---

### 4- Folder → Namespace Mapping

| Folder Path | Namespace Root |
|--------------|----------------|
| `src/domain/core` | `tasqly::pX::sY::domain::core` |
| `src/app/settings` | `tasqly::pX::sY::app::settings` |
| `src/infra/logging` | `tasqly::pX::sY::infra::logging` |
| `tests/unit/settings` | `tasqly::pX::sY::tests::settings` |
| `benchmarks/unit/persistence` | `tasqly::pX::sY::benchmarks::persistence` |
| `qml/ui/pages` | `Tasqly.PX.SY.Ui.Pages` |
| `qml/ui/components` | `Tasqly.PX.SY.Ui.Components` |

---

### 5- Documentation & ADRs
All ADRs, diagrams, and test documentation must also include Phase–Slice prefixes.

**Examples**
- ADR: `P1_S1_0010-global-naming-template-and-namespace-policy.md`
- Diagram: `P1_S1_component.puml`
- UML Export: `P1_S1_sequence.svg`

---

### 6- CI/CD Integration
- CI will include a **namespace and prefix validator** to ensure:
  - Filename matches regex: `^P\d+_S\d+_.*`
  - Namespace path aligns with `tasqly::pX::sY`
  - QML files have valid `id` prefix
- Violations fail `clang-tidy` + QML lint stages.

---

## Consequences
✅ Prevents namespace and ID collisions across slices.  
✅ Guarantees one-to-one mapping between phase/slice and code scope.  
✅ Improves traceability in CI, testing, and debugging.  
✅ Enables multi-slice parallel development safely.  
⚠️ Requires adaptation of CI, CMake, and QML imports.  

---

## ✅ Quick Checklist (For Any New File)

☑ Filename starts with `P#_S#_`.  
☑ Namespace includes `p#::s#::layer::subsystem`.  
☑ Folder structure matches namespace.  
☑ Header includes `#pragma once`.  
☑ No `using namespace` in headers.  
☑ QML IDs/ObjectNames start with `p#s#_`.  
☑ Imports reference correct phase/slice.  
☑ Document phase/slice in header comment.  
☑ Add "Last Major Change" field in metadata.  
☑ Tests/benchmarks/docs follow prefix rules.  

---

## Next Steps
- ✅ Replace old `FilePolicy.md` with this document.  
- ✅ Update CI naming checks.  
- ✅ Refactor QML IDs gradually to comply.  
- 🔜 Add namespace validator script (`tools/check_namespace_prefix.py`).  

---
