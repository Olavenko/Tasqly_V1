# ADR 000X: upgrade-ci-clang-format-20

- **Status**: Accepted  
- **Date**: 2025-09-29  
- **Owners**: Tasqly Team  
- **Related**: Phase0 — Quality Gates  
- **Tags**: ci, formatting, clang-format, quality

---

## Context
The CI pipeline was failing at the `clang-format` step because of modern options (`AlignFunctionDeclarations`, etc.) that are not supported in the default Ubuntu GitHub Actions runner versions (clang-format 14/15).  
Locally, developers use `clang-format >=20`, which caused incompatibility.

---

## Decision
- Adopt **clang-format-20** as the official formatting version in CI.  
- Use the `scripts/check-format.sh` script to ensure consistent checks locally and in CI.  
- Inside the script, force execution via absolute path:  
  ```bash
  /usr/bin/clang-format-20 --dry-run --Werror
  ```  
  to avoid falling back to older preinstalled versions.  
- Add a debug step (`clang-format-20 --version`) in CI to explicitly show the version used in logs.

---

## Consequences
- ✅ CI and local environments are unified on clang-format 20.  
- ✅ Eliminates errors caused by unsupported `.clang-format` keys.  
- ⚠️ Relies on the LLVM apt repo for installation, which slightly increases install time.  
- 🔄 Future upgrades (e.g., clang-format 21) must be documented in a new ADR.

---

## References
- `.github/workflows/ci.yml`  
- `scripts/check-format.sh`  
- `Testing_Standards_Combined.md`  
- `Project_Quality_Policy.md`  
