# ADR 0009: Expand Clang-Format Scope to Entire Project

- **Status**: Accepted  
- **Date**: 2025-09-30  
- **Owners**: Tasqly Team  
- **Related**: Phase0 — Slice4  
- **Tags**: formatting, ci, quality  

---

## Context
Previously, our `scripts/check-format.sh` only validated source files in `src/` and a few root-level files.  
This excluded `tests/`, `benchmarks/`, and `main.cpp`.  

As a result, formatting inconsistencies could pass locally but fail on CI.  
Example: `main.cpp` in the project root was not formatted locally, but CI caught it.  

---

## Decision
We expand the scope of **clang-format enforcement** to cover the **entire project**:  
- `src/` (production code)  
- `tests/` (unit & integration tests)  
- `benchmarks/` (Google Benchmark code)  
- Root-level `.cpp`/`.h` files (e.g., `main.cpp`)  

Script update:

```bash
FILES=$(find src tests benchmarks -name '*.cpp' -o -name '*.h')
FILES="$FILES $(find . -maxdepth 1 -name '*.cpp' -o -name '*.h')"

clang-format-20 --dry-run --Werror $FILES
```

---

## Consequences
- ✅ Uniform formatting across **all C++ code**.  
- ✅ CI alignment with developer expectations.  
- ✅ Prevents regressions (no more unformatted test/benchmark code).  
- ⚠️ Slightly higher runtime in CI (more files to check).  
- ⚠️ Developers must ensure test/benchmark files are formatted before committing.  

---

## References
- `.clang-format` (root config)  
- `scripts/check-format.sh` (updated)  
- `Testing_Standards_Combined.md` — format_check policy  
- `Project_Quality_Policy.md` — CI quality gates  
