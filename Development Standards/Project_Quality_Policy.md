# 📑 PROJECT_QUALITY_POLICY.md

## 🎯 Purpose  
This document defines the **Quality Assurance Policy** for the Tasqly project.  
It ensures correctness, stability, performance, and maintainability across all phases of development.  
The policy is part of the **Definition of Done (DoD)** and applies to all contributors and CI/CD pipelines.  

---

## 🔹 Execution Order (High-Level Workflow)
1. **Unit & Integration Tests** → correctness of core logic.  
2. **QML Tests** → correctness of UI components.  
3. **Sanitizers & Memory Safety** → runtime safety.  
4. **Code Coverage Check** → ensure sufficient test coverage.  
5. **Security / Secrets Scan** → prevent leaks of credentials/configs.  
6. **Benchmarks** → performance regression prevention.  
7. **Profiling** → deep analysis of runtime performance.  
8. **Refactoring & Documentation** → maintain clean, sustainable codebase.  

---

## POLICY — Unit & Integration Tests
**Tool(s):** Google Test, CTest  
**Scope:** C++ domain logic, application services, infrastructure modules  
**Enforcement:**  
- CI target: `TasqlyTestsRunner.exe`  
- Tests run on every PR and push to `main`  
**Thresholds / Targets:** 100% of core domain logic covered  
**Reporting:** CI logs, HTML reports  
**Exceptions:** None — all new code must include tests  
**Maintenance:** Reviewed each phase in ADR  

---

## POLICY — QML Tests
**Tool(s):** Qt Quick Test (`qmltestrunner`), Squish (optional UI automation)  
**Scope:** QML components, bindings, UI logic  
**Enforcement:**  
- CI target: `TasqlyQmlTestsRunner.exe`  
- All new QML components must include at least one test case  
**Thresholds / Targets:** 100% of core QML components covered with functional tests  
**Reporting:** CI logs, HTML results as artifact  
**Exceptions:** Pure styling (colors, anchors) may be excluded  
**Maintenance:** ADR: QML Testing Policy  

---

## POLICY — Sanitizers & Memory Safety
**Tool(s):** AddressSanitizer (ASan), UndefinedBehaviorSanitizer (UBSan)  
**Scope:** Debug builds on Linux CI  
**Enforcement:**  
- CI builds with `-fsanitize=address,undefined`  
- All tests must pass with sanitizers enabled  
**Thresholds / Targets:** Zero sanitizer errors allowed  
**Reporting:** CI logs stored as artifact  
**Exceptions:** None  
**Maintenance:** ADR: Sanitizer Policy  

---

## POLICY — Code Coverage
**Tool(s):** gcov, lcov, llvm-cov, Codecov (GitHub Action)  
**Scope:** Unit + Integration Tests  
**Enforcement:**  
- CI job builds with `-DENABLE_COVERAGE=ON`  
- Reports uploaded to Codecov, badge in README  
**Thresholds / Targets:** ≥60% baseline (Phase 0.x), increase by 5–10% each phase  
**Reporting:** HTML report in CI artifacts + Codecov dashboard  
**Exceptions:** Generated files (moc, rcc, UI auto-gen)  
**Maintenance:** ADR: Coverage Tool Policy  

---

## POLICY — Security / Secrets Scan
**Tool(s):** git-secrets, trufflehog  
**Scope:** Entire repository (code, configs, docs)  
**Enforcement:**  
- CI job runs on every push/PR  
- Pre-commit hooks available for local dev  
**Thresholds / Targets:** Zero committed secrets  
**Reporting:** CI logs, failed build if secrets detected  
**Exceptions:** Test credentials explicitly whitelisted  
**Maintenance:** ADR: Secrets Handling Policy  

---

## POLICY — Benchmarks
**Tool(s):** Google Benchmark  
**Scope:** CRUD operations, domain-heavy logic  
**Enforcement:**  
- CI target: `TasqlyBenchmarksRunner.exe`  
- Run CRUD ×1k and ×10k operations  
**Thresholds / Targets:** CRUD ×10k < 500ms baseline  
**Reporting:** JSON/CSV artifacts in CI  
**Exceptions:** Profiling benchmarks optional in early PRs, mandatory at Phase completion  
**Maintenance:** ADR: Benchmark Policy  

---

## POLICY — Profiling
**Tool(s):** QML Profiler, perf, heaptrack  
**Scope:** UI frame rate, memory usage, CPU hotspots  
**Enforcement:** Manual runs required at end of each phase  
**Thresholds / Targets:** UI ≥60fps baseline  
**Reporting:** Profiling results documented in ADRs  
**Exceptions:** None  
**Maintenance:** ADR: Profiling Policy  

---

## POLICY — Refactoring & Documentation
**Scope:** Entire codebase, ADRs, knowledge base  
**Enforcement:** Refactor and update docs before Phase closure  
**Thresholds / Targets:**  
- No duplicated code blocks  
- All architectural decisions recorded in ADRs  
**Reporting:** Updated ADRs + KnowledgeBase  
**Exceptions:** Emergency hotfixes (must be cleaned up in next phase)  
**Maintenance:** Continuous review  
