# ADR 0009: CI Quality Gates Phasing

- **Status**: Accepted  
- **Date**: 2025-09-28  
- **Owners**: Tasqly Team  
- **Related**: Phase0, Phase1  
- **Tags**: ci, quality, coverage, benchmarks, sanitizers, static-analysis, secrets-scan  

---

## Context
During Phase 0, the focus is on building and validating Tasqly's **core subsystems** (Logging, Error, Notifier, Settings, Theme, Navigation, Persistence InMemory). At this stage:
- No QML/UI components are present.
- The main need is to establish **baseline correctness and performance** for core logic.
- Enabling all CI quality gates (sanitizers, static analysis, secrets scan) would slow down development with limited added value, since core code is small and deterministic.

To balance **quality vs. velocity**, we decided to phase the activation of CI quality gates.

---

## Decision
- **Phase 0 (Core Systems)**:
  - ✅ **Coverage Reports** (≥60% baseline, GoogleTest, uploaded from MSVC).  
  - ✅ **Benchmarks** (Google Benchmark, CRUD & core flows, uploaded from MSVC).  
  - ❌ No sanitizers, static analysis, or secrets scan at this phase.

- **Phase 1+ (UI + Persistence)**:
  - ✅ Continue coverage + benchmarks.
  - ✅ Enable **Sanitizers** (ASan, UBSan) on Linux debug builds.
  - ✅ Enable **Static Analysis** (clang-tidy, cppcheck, format checks).
  - ✅ Enable **Secrets Scan** (git-secrets, trufflehog).

---

## Consequences
- ✅ Keeps Phase 0 CI fast and focused on establishing baselines.
- ✅ Ensures early visibility into **test coverage** and **performance trends**.
- ✅ Provides flexibility to defer heavier checks until the codebase (UI, DB integration) justifies them.
- ⚠️ Risk: Some undefined behaviors or style issues may slip through Phase 0 undetected (accepted trade-off).
- 🔄 Trade-off: Simpler CI now, stricter CI later.

---

## References
- [Project_Quality_Policy.md]  
- [phase0x_quality_policies_ordered.md]  
- [BuildPolicy.md]  
- [Testing_Standards_Combined.md]  
- ADR 0004 — CI Workflows (Phase0, Slice0)

