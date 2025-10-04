# ADR 0009: Benchmark Log Cleanup Policy

- **Status**: Draft
- **Date**: 2025-09-28
- **Owners**: Tasqly Team
- **Related**: Phase0 — Slice0 (Benchmarks)
- **Tags**: benchmarks, logging, cleanup, ci-artifacts

---

## Context

During local and CI benchmark runs, several log files were being written directly to the project root, including:

```
bench_logcontinuous.log
bench_logcontinuous.log.[1-3]
bench_logflush.log
bench_logflush.log.[1-3]
LastCoverageResults.log
```

This polluted the repository root and conflicted with our **Project_Quality_Policy** which requires a clean workspace and structured artifact storage. Benchmarks in particular should be self-contained and not leak log files outside the designated `reports/` hierarchy.

## Decision

We introduced a **Benchmark Log Cleanup Policy** with the following rules:

1. **Dedicated Benchmark Log Directory**:
   - All logs generated during benchmark runs are redirected into:
     ```
     reports/benchmarks/logs/
     ```

2. **Automatic Cleanup**:
   - `bench_main.cpp` ensures the `reports/benchmarks/logs/` directory exists before each run.
   - Old `.log` files in this directory are deleted at startup to avoid accumulation across runs.

3. **Single Consolidated File**:
   - Instead of multiple scattered files, all benchmark logs are written to:
     ```
     reports/benchmarks/logs/benchmarks.log
     ```

4. **CI Integration**:
   - CI scripts (`run_bench_mingw_release.py`, `run_bench_msvc_release.py`) are updated to archive or clean this folder after each run.

5. **No Root Pollution**:
   - Project root remains free of benchmark log artifacts.

## Consequences

- ✅ **Benefits**:
  - Keeps repository root clean and compliant with quality gates.
  - Easier artifact collection in CI (logs are centralized).
  - Predictable, reproducible log location for debugging.

- ⚠️ **Drawbacks**:
  - Requires developers to check inside `reports/benchmarks/logs/` instead of root for logs.
  - Consolidating into a single file may reduce granularity of per-test log outputs.

- 🔄 **Trade-offs**:
  - Chose simplicity and cleanliness over per-benchmark file separation. Developers can still enable custom log sinks in specific benchmarks if required.

## References

- `benchmarks/bench_main.cpp` (updated to handle log directory and cleanup)
- `src/infra/logging/LogManager.{h,cpp}`
- `scripts/run_bench_mingw_release.py`, `scripts/run_bench_msvc_release.py`
- Project_Quality_Policy.md
- Subsystems_Unified.md (Logging subsystem)
