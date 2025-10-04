# ADR 0008: Switching Coverage Builds from Release to Debug Mode

# 📝 Architecture Decision Record (ADR)
**Date:** 2025-09-21  
**Status:** Accepted  
**Context:** Reporting & Coverage Infrastructure

---

### 🎯 Context
We introduced automated coverage reports for both **unit tests** (GoogleTest) and **benchmarks** (Google Benchmark) using **OpenCppCoverage** on Windows and **lcov** on Linux.

During initial runs:
- Unit test coverage worked correctly under `Debug` builds.
- Benchmark coverage failed or produced empty/incorrect results when built under `Release` mode.

The root cause was identified as **OpenCppCoverage not being able to instrument optimized binaries** reliably. In `Release` mode, compiler optimizations (inlining, dead code elimination, loop unrolling, etc.) made it impossible for the coverage tool to map execution back to source lines.

---

### ✅ Decision
We will switch all coverage-related builds (tests and benchmarks) to use **`Debug` mode** instead of `Release` mode.

- Presets were updated to include `build-msvc-tests-reports-debug` and `build-msvc-benchmarks-reports-debug`.
- Coverage targets (`coverage_tests`, `coverage_benchmarks`) now depend on **debug executables**.
- `Release` mode will continue to be used for performance benchmarks and production binaries, but **not** for coverage runs.

---

### 📈 Consequences
- **Positive:**
  - Coverage reports for **tests** and **benchmarks** are now generated consistently.
  - OpenCppCoverage properly selects modules and instruments code without errors.
  - Debug symbols remain available, ensuring accurate line-level mapping.

- **Negative:**
  - Coverage benchmarks in `Debug` mode may not reflect real performance numbers (optimizations are disabled).
  - Slightly longer runtime for coverage tasks due to lack of compiler optimizations.

- **Neutral/Mitigation:**
  - For performance validation, we will continue to run benchmarks separately in `Release` mode (without coverage).
  - For coverage validation, we will run them in `Debug` mode.

---

### 📌 Status
Coverage reports are now functional and reliable. Benchmarks generate meaningful coverage data (only for benchmarked code paths). The separation between **coverage (Debug)** and **performance (Release)** is documented and intentional.

---

### 🔮 Future Considerations
- Explore hybrid builds (RelWithDebInfo) to balance optimization and debuggability.
- Consider excluding system/Qt DLLs explicitly to reduce coverage noise.
- Automate differential coverage reports between **tests** and **benchmarks** to track what each layer covers.

