# ADR 0006: Task Mapper Strategy: Manual Conversion

- **Status:** Accepted  
- **Date:** 2025-10-06  
- **Owner:** Tasqly Team  
- **Related:** Phase 1 — Slice 1  
- **Tags:** Domain Entities, Contracts & Mappers
---

## 🧩 Context

In this phase, we introduced a separation between domain entities (`Task`) and transfer representations (`TaskDto`)  
to enable persistence, transport, and decoupling from UI and DB technologies.  
The next design decision was how to **map** between these two layers.

There are two mainstream strategies:
1. **Manual mapping** — Explicit code for every field conversion.
2. **Automated mapping** — Code generation or reflection-based tools.

We evaluated both in terms of performance, dependency weight, and maintainability.

---

## 🎯 Decision

We decided to implement **Manual Mapping** for `Task ↔ TaskDto` using a dedicated utility class  
`TaskMapper` inside the `domain/core/mappers` module.

This approach ensures:
- No external dependency (no reflection / JSON libraries required).
- Full control over type conversions (`enum ↔ string`, `chrono ↔ ISO8601 string`).
- Deterministic and testable conversions.
- Perfect compliance with Clean Architecture isolation rules.

---

## 🧠 Rationale

| Criterion | Manual Mapping | Code Generation |
|------------|----------------|------------------|
| **Performance** | ✅ Zero reflection overhead | ❌ Slower at runtime |
| **Transparency** | ✅ Explicit and easy to debug | ❌ Hidden logic / opaque |
| **Dependencies** | ✅ None (STL only) | ❌ Requires build tools or external libs |
| **Domain Safety** | ✅ Full control on defaults | ⚠️ Depends on schema correctness |
| **Portability** | ✅ Works across all targets (Qt, CLI, Server) | ❌ May break on cross-platform builds |

Manual mapping also aligns with Tasqly’s architectural constraint:
> “Domain layer must remain technology-agnostic and fully deterministic.”

---

## 🧩 Implementation Summary

| Component | Description |
|------------|--------------|
| `TaskMapper.h/.cpp` | Contains `toDto()` and `fromDto()` functions for bidirectional conversion. |
| `TaskDto.h` | Lightweight data structure for transfer (no Qt/DB types). |
| Time conversion | Done using `std::put_time` / `std::get_time` in ISO8601 UTC format. |
| Enum conversion | Done using `toString()` and `fromString()` helpers from `TaskStatus` & `TaskPriority`. |

---

## ⚙️ Example

```cpp
Task task;
task.title = "Write ADR";
task.status = TaskStatus::Doing;

TaskDto dto = TaskMapper::toDto(task);
Task roundtrip = TaskMapper::fromDto(dto);
```

✅ `roundtrip` equals original `task` (excluding timestamps precision).

---

## 🧪 Testing Strategy

| Test Case | Type | Expected Result |
|------------|------|------------------|
| `Task → Dto → Task` roundtrip | Unit | All fields match |
| Invalid `status` string | Unit | Defaults to `Todo` |
| Missing `deadline` | Unit | Converts to `nullopt` |
| Enum string conversion | Unit | `"High"` ↔ `TaskPriority::High` |

---

## 🚀 Consequences

### ✅ Positive
- Full transparency and traceability.
- Easy debugging during early domain design.
- Unit-testable conversions.

### ⚠️ Negative
- Slightly higher maintenance cost (manual code for each field).
- Requires discipline during future entity changes.

---

## 🔗 References

- **ADR Template:** `/docs/adr/ADR-template.md`
- **Subsystem:** `/src/domain/core/mappers/`
- **Related Docs:**  
  - `/src/domain/core/entities/Task.h`  
  - `/src/domain/core/mappers/TaskMapper.h`  
  - `/src/domain/core/mappers/TaskDto.h`
- **Standards:** `C++_Standards_Combined.md`, `FilePolicy.md`, `Project_Quality_Policy.md`

---

## 🏁 Status

✅ **Accepted & Implemented**  
This decision is frozen for Phase 1 but can be revisited in Phase 3 (Automation Layer)  
if a safe code-generation framework is adopted.

---
