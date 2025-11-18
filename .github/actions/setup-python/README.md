# Setup Python

Thin wrapper around `actions/setup-python` that standardizes package installation and caching.

## Inputs
- `python-version` — semantic version or release line (default `3.13`).
- `requirements-file` — relative path to install with `pip install -r`. Fails if set but missing.
- `extra-packages` — space separated adhoc packages.

## Outputs
- `python-path` — interpreter path from `actions/setup-python`.
- `pip-cache-hit` — bubble up from the cache step to let jobs know if they need to warm caches.

Every run exports `PYTHONPATH=${{ github.workspace }}/scripts:$PYTHONPATH` so helper modules resolve automatically.
