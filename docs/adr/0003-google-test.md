# ADR 0003: Google Test Framework

## Context
Tasqly requires automated unit testing to validate core components and maintain quality. 
Tests must be easy to run locally and in CI. The framework should be modern, actively maintained, 
cross-platform, and compatible with C++20.

## Decision
- Adopt **Google Test (gtest)** as the official testing framework.
- Integrate via `FetchContent` in CMake for reproducible builds.
- Add test targets (e.g., `TasqlyTests`) alongside the main app target.
- Organize tests under `tests/` folder, following clear naming conventions.
- Enable test discovery with `gtest_discover_tests()`.
- Ensure tests run both locally (`ctest`) and in CI pipelines.

## Consequence
- Developers can run unit tests easily with `ctest`.
- CI validates new commits automatically.
- Testing becomes part of the Definition of Done for each vertical slice.
- Project gains a reliable testing foundation with a widely adopted framework.
