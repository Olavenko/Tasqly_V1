# ADR 0005: Git Hygiene

## Context
Consistent Git configuration is needed to maintain a clean and collaborative repository.  
We must prevent unwanted files (e.g., build artifacts) from being committed and enforce coding style standards.

## Decision
- Add a `.gitignore` to exclude:
  - build/ directories
  - *.user, *.pro.user (Qt Creator files)
  - *.o, *.obj, *.exe, *.dll, *.a
  - *.log, *.tmp
- Add `.gitattributes` for consistent line endings (LF for code, text=auto).
- Add `.clang-format` and `.clang-tidy` for coding standards enforcement.
- Add `MyAllmanClassic.xml` (Qt Creator preset) for editor consistency.

## Consequence
- Repository stays clean (no accidental binaries).
- Cross-platform consistency (line endings, formatting).
- Developers share consistent coding style and IDE settings.
