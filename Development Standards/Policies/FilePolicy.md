# ADR 0006 — File Template & Coding Policy

* **Status**: Accepted  
* **Date**: 2025-09-07  
* **Author**: Mohamed Ali  

---

## Context
During Slice 4 development, multiple issues occurred due to inconsistent namespace usage and include path confusion.  
To prevent such errors, a unified file template and a set of mandatory rules must be followed whenever creating new source or header files.

---

## Decision
All new files must follow these conventions:

### 1. Header Files (`.h`)
- Always start with a **header comment block** (see template).  
- Use `#pragma once` for include guards.  
- Use **relative include paths from `src/`**.  
- Keep includes minimal (prefer forward declarations when possible).  
- Define everything inside the correct namespace.  
- Never use `using namespace` in header files.  
- Document stability, version, dependencies, related files, errors, performance, security, test coverage, and last major change.

### 2. Implementation Files (`.cpp`)
- Start with a **header comment block** mirroring the header file.  
- Always include the corresponding header first.  
- Use **relative include paths** for all includes.  
- In `.cpp` files, `using namespace tasqly::<layer>::<subsystem>;` is acceptable, but **never in headers**.  
- Implement all methods declared in the header file.  
- Add inline comments for small steps and Doxygen-style comments for larger blocks.  
- Document the same metadata fields as headers.

### 3. General Rules
- Every file must clearly specify its **layer** and **namespace** in the header comment.  
- Entities and repository interfaces → `tasqly::domain::core`  
- Application coordination classes → `tasqly::app::<subsystem>`  
- Infrastructure utilities → `tasqly::infra::<subsystem>`  
- Tests and fakes → `tests/fakes`  

---

## Consequences
- Ensures consistent project structure across all layers.  
- Prevents namespace-related compiler errors.  
- Guarantees predictable and portable includes.  
- Provides self-documented files with error, stability, and performance contracts.  
- Helps developers and reviewers quickly understand file responsibilities.

---

## Next Steps
- ✅ Apply this template and rules to all future files.  
- ✅ Refactor older files to comply.  
- 🔜 Automate checks in CI (namespace, includes, stability marker).

---

# ✅ Quick Checklist (For New Files)

☑ Start with header block (File, Purpose, Layer, Author, Date, Version, Stability).  
☑ Use `#pragma once` in headers.  
☑ Use **relative includes** only (from `src/`).  
☑ Minimize includes, prefer forward declarations.  
☑ No `using namespace` in headers.  
☑ Namespace must match layer (domain/app/infra).  
☑ Implement in `.cpp`, mirror header metadata.  
☑ Document: Errors, Performance, Security, Test Coverage.  
☑ Add "Last Major Change" note in header block.  
