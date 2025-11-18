# Setup Reports

Lightweight action that standardizes report folders inside `reports/` so downstream jobs can rely on consistent artifact paths.

## Inputs
- `root` — base reports directory (default `reports`).

## Outputs
- `tests-pattern` — `${root}/tests/**/*`
- `benchmarks-pattern` — `${root}/benchmarks/**/*`
- `coverage-pattern` — `${root}/coverage/**/*`

Each invocation creates the subdirectories if they do not yet exist.
