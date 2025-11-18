# Setup Coverage

Common installation logic for coverage tooling across CI jobs.

## Modes
- `linux` — installs `lcov`, `gcovr`, `ninja-build`, and Python helper libs.
- `windows` — installs `OpenCppCoverage` via Chocolatey and exposes it on the `PATH`.

## Inputs
- `mode` — `linux` or `windows` (default `linux`).
- `install-python-deps` — installs `plotly` + `pandas` when `true`.

## Outputs
- `coverage-bin` — `gcovr` on Linux or `OpenCppCoverage.exe` on Windows.
- `coverage-format` — string describing the active coverage backend.

Every run exports `COVERAGE_TOOL` for later shell steps.
