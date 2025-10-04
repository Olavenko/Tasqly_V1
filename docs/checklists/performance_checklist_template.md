# 📑 Performance Checklist Template

**Purpose:** Standard template for creating new performance optimization checklists.

---

## 🎯 Goal
[Brief description of the performance problem to solve]

---

## 🪜 Execution Steps

### 1-Impact Analysis
- [ ] Map each function in the target component to related tests (Unit + Integration).
- [ ] Identify which tests will likely fail if implementation changes.

### 2-Implementation
- [ ] Modify implementation files: `src/...`
- [ ] Update headers if needed.

### 3-Testing
- [ ] Run Unit Tests:
  - `tests/unit/...`
- [ ] Run Integration Tests:
  - `tests/integration/...`

### 4-Benchmarks
- [ ] Run benchmark: `performance/benchmarks/...`
- [ ] Compare results with baseline.

### 5-Documentation
- [ ] Create new ADR in `docs/adr/...`
- [ ] Document results (before/after).
- [ ] Save benchmark dashboard snapshot.

---

## ✅ Definition of Done
- [ ] All tests pass.
- [ ] Benchmarks stable and improved.
- [ ] ADR documented.