# 🛡️ Phase X — Quality & Performance Policy (Improved Template)

📌 **Purpose**  
This file is applied **after completing any Core Phase (Phase 0 → Phase 9)** to build an **additional integration phase**.  
The purpose of this extra phase is to ensure that the code is not only functionally correct, but also:
- Properly covered by tests (Coverage).  
- Safe against memory issues and undefined behavior (Sanitizers).  
- Free from accidentally committed secrets (Security Scan).  
- Benchmarked and validated against defined Performance Bars.  
- Integrated into CI/CD as formal Quality Gates.

---

## 🔹 GLOBAL — Quality Bars / DoR / DoD

**Quality Bars (Target / Actual)**  
- [ ] Coverage ≥ **80% baseline** — Target: ≥80% | Actual: ___ %  
- [ ] AddressSanitizer/UBSan jobs pass — Target: 100% | Actual: ___ %  
- [ ] CRUD Benchmarks stable < **120ms** — Target: ≤120ms | Actual: ___ ms  
- [ ] git-secrets scan clean — Target: 0 secrets | Actual: ___  

**Definition of Ready (DoR)**  
- ✅ Previous Phase CI baseline green.  
- ✅ clang-tidy / cppcheck configs available.  
- ✅ Benchmarks infra (Google Benchmark) available.  

**Definition of Done (DoD)**  
- [ ] CI matrix updated with Coverage + Sanitizers + Security Scan + Benchmarks.  
- [ ] Reports automatically uploaded (Codecov or artifact).  
- [ ] Benchmark results stored as CI artifacts (JSON + HTML).  
- [ ] ADR added: “Quality & Performance Validation”.  

---

## 🔹 SECTION 1 — Quality Extensions

### Slice 1: Coverage Integration (~2 hrs)
- **Goal**: Generate code coverage reports for C++/QML tests in CI.  
- **Tools**: `gcov` + `lcov` (Linux GCC) / `llvm-cov` (Clang).  
- **Output**: HTML + LCOV uploaded as artifact + badge in README.  
- **Implementation**: Add `-DENABLE_COVERAGE=ON` to CMake + CI job.  
- **ADR**: “Coverage Policy — minimum threshold 80%”.

**Testing Matrix**  
| Type        | Description                   | Result |
|-------------|-------------------------------|--------|
| CI          | Coverage job runs on PR       | ☐      |
| Artifact    | HTML/LCOV report downloadable | ☐      |

---

### Slice 2: Sanitizers & Memory Safety (~2 hrs)
- **Goal**: Detect memory and UB issues early.  
- **Tools**: ASan + UBSan.  
- **Implementation**: Add `asan-ubsan` CMake preset. CI runs Debug build with sanitizers on Linux.  
- **ADR**: “Sanitizer Policy — required on Linux builds”.

**Testing Matrix**  
| Type   | Description                  | Result |
|--------|------------------------------|--------|
| CI     | ASan job passes all tests    | ☐      |
| CI     | UBSan job passes all tests   | ☐      |

---

### Slice 3: Security / Secrets Scan (~2 hrs)
- **Goal**: Prevent accidental leaks of secrets.  
- **Tools**: `git-secrets` or `trufflehog`.  
- **Implementation**: Add CI job scanning repo on every PR.  
- **ADR**: “Secrets Handling Policy — no hardcoded credentials”.

**Testing Matrix**  
| Type   | Description                | Result |
|--------|----------------------------|--------|
| CI     | Scan job runs on PR        | ☐      |
| CI     | Fail if secret detected    | ☐      |

---

### Slice 4: Benchmark Automation (~2–3 hrs)
- **Goal**: Automate benchmark runs in CI.  
- **Tools**: Google Benchmark with JSON export.  
- **Implementation**: Add benchmark target in CMake. CI job runs benchmarks and stores artifacts.  
- **ADR**: “Benchmark Policy — CRUD baseline tracked in CI”.

**Testing Matrix**  
| Type   | Description                       | Result |
|--------|-----------------------------------|--------|
| CI     | CRUD runs, stable timings         | ☐      |
| CI     | JSON uploaded as artifact         | ☐      |

---

## 🔹 SECTION 2 — Performance Validation

### Stage 1 — <Stage Name>
- **Scope**: Performance Bars defined in the previous Phase.  
- **Checklist**:  
  - Benchmark with **Empty dataset**.  
  - CI job stores results as JSON + HTML artifact.  
  - ADR entry: “<Stage Name> Performance Baseline”.

### Stage 2 — <Stage Name>
- **Scope**: Performance Bars defined in the previous Phase.  
- **Checklist**:  
  - Benchmark with **Medium dataset**.  
  - CI job stores results as JSON + HTML artifact.  
  - ADR entry: “<Stage Name> Performance Baseline”.

### Stage 3 — <Stage Name>
- **Scope**: Performance Bars defined in the previous Phase.  
- **Checklist**:  
  - Benchmark with **Stress dataset**.  
  - CI job stores results as JSON + HTML artifact.  
  - ADR entry: “<Stage Name> Performance Baseline”.

---

## ✅ Notes
- Quality Extensions = enforce Quality Gates in CI/CD.  
- Performance Validation = confirm Phase-specific Performance Bars (Empty/Medium/Stress).  
- CI must store coverage, sanitizer logs, and benchmark results as artifacts.  
- ADRs must follow the standard format (Context → Decision → Consequence).  

---

## 📚 References
- [TESTING.md](../../TESTING.md)  
- [BuildPolicy.md](BuildPolicy.md)  
- [phase0_quality_extensions_template.md](../Templates/phase0_quality_extensions_template.md)  
- [PhaseX_PerformanceValidation_Template.md](../Templates/PhaseX_PerformanceValidation_Template.md)  

