# 📑 Quality Extension Policies for Phase 0.x

## Template: Quality Extension Policy

```markdown

## POLICY — Sanitizers & Memory Safety
**Tool(s):** AddressSanitizer (ASan), UndefinedBehaviorSanitizer (UBSan).  
**Scope:** Debug builds on Linux CI.  
**Enforcement:**  
- CI job builds with `-fsanitize=address,undefined`.  
- All tests must pass with sanitizers enabled.  
**Thresholds / Targets:** Zero sanitizer errors allowed.  
**Reporting:** Logs stored as CI artifact.  
**Exceptions:** None. Any sanitizer warning = test failure.  
**Maintenance:** ADR: “Sanitizer Policy — required for Linux builds”.

---

## POLICY — Code Coverage
**Tool(s):** `gcov`, `lcov`, `llvm-cov`, Codecov (GitHub Action).  
**Scope:** Unit + Integration tests.  
**Enforcement:**  
- CI job builds with `-DENABLE_COVERAGE=ON`.  
- Reports uploaded to Codecov, badge in README.  
**Thresholds / Targets:** Minimum 60% line coverage (Phase 0.x baseline), increasing by 5–10% in later phases.  
**Reporting:** HTML report as CI artifact + Codecov dashboard.  
**Exceptions:** Generated code, UI auto-generated files (e.g. moc, rcc).  
**Maintenance:** ADR: “Coverage Tool — ≥60% baseline, periodic increase”.

---

## POLICY — Security / Secrets Scan
**Tool(s):** `git-secrets` (AWS/GCP/Generic patterns), `trufflehog`.  
**Scope:** Entire repo (code, configs, docs).  
**Enforcement:**  
- CI job runs on every push/PR.  
- Pre-commit hook available for local dev.  
**Thresholds / Targets:** Zero committed secrets.  
**Reporting:** CI logs fail build if secrets detected.  
**Exceptions:** Test credentials must be explicitly whitelisted in config.  
**Maintenance:** ADR: “Secrets Handling Policy — no hardcoded credentials”.

---

## POLICY — Benchmarks & Profiling
**Tool(s):** Google Benchmark, QML Profiler, `perf`, `heaptrack`.  
**Scope:** CRUD operations, repo performance, UI frame times.  
**Enforcement:**  
- Benchmarks built as separate target.  
- CI runs CRUD ×1k and ×10k, exports JSON/CSV artifacts.  
- Profiling runs documented manually in ADRs.  
**Thresholds / Targets:**  
- CRUD ×10k < 500ms.  
- UI frame rate ≥ 60fps baseline.  
**Reporting:** JSON artifacts uploaded in CI + logs.  
**Exceptions:** Profiling not required in every PR, but must be repeated at end of each Phase.  
**Maintenance:** ADR: “Benchmark Policy — CRUD baseline tracked in CI”.

---

# ✅ How to Use
- For every new quality tool → copy the Template and fill in details.  
- Add ADR with ID (e.g., ADR-0010: Coverage Policy).  
- Link in Phase 0.x as part of **Definition of Done (DoD)** so all future features comply.

