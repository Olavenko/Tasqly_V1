# Setup PostgreSQL

Utility action that blocks until PostgreSQL is reachable and optionally runs a migration script.

## Inputs
- `enabled` — skip everything when `false`.
- `host`, `port`, `user`, `password`, `database` — connection parameters used by `pg_isready` and `psql`.
- `migration-script` — relative path to a `.sql` file executed once the server answers.

## Outputs
- `connection-uri` — canonical URI string for logging / debugging.
- `psql-bin` — resolved `psql` path on Linux runners (empty elsewhere).

The action also exports PG environment variables to `$GITHUB_ENV`, so later steps can call `psql` or tests without re-stating credentials.
