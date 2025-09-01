# ADR 0004: CI Workflows Definition

## Context
Tasqly requires continuous integration (CI) to validate builds, run tests, and ensure code quality.  
CI must support both Windows and Linux environments, with and without Qt, to ensure portability.  
Diagrams (PlantUML) must also be auto-exported and published.

## Decision
- Use **GitHub Actions** as the CI provider.
- Define workflows:
  - **ci.yml**: build & test matrix (Windows MinGW baseline, Linux GCC/Clang, optional MSVC).
  - **uml.yml**: auto-generate PlantUML diagrams.
  - **uml-pages.yml**: publish diagrams to GitHub Pages.
- Ensure CI runs both with Qt and without Qt (using stubs when Qt unavailable).
- Release notes updated automatically per phase.

## Consequence
- CI validates every PR and push.
- Builds are reproducible across environments.
- PlantUML diagrams stay synchronized with repository updates.
- Developers gain confidence in cross-platform stability.
