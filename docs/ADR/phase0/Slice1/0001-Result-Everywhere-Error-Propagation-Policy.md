# ADR 0001: Result-Everywhere-Error-Propagation-Policy

- **Status**: Accepted
- **Date**: 2025-09-01
- **Owners**: Tasqly Team
- **Related**: Phase0 — Slice1 (Logging & Error Backbone)
- **Tags**: error-handling, result-type, consistency

---

## Context
The project requires a unified and predictable way of propagating errors across
all layers (domain, application, infrastructure, UI). Traditional exception-based
error handling was considered but rejected due to:

* Exceptions in Qt/C++ may cross library boundaries unpredictably.
* Exceptions break determinism in async/QML contexts.
* Harder to test compared to value-based results.

## Decision
We adopt `Result<T, Error>` as the **exclusive error propagation mechanism**
across the codebase.

* Every function that may fail **must** return `Result<T, Error>`.
* `Error` is a structured type with:

  * `code` (int or enum),
  * `message` (QString, defaults to "Unknown error" if empty),
  * `context` (QVariantMap for extra info).
* Propagation follows "explicit bubble-up":

  ```cpp
  Result<User, Error> user = repo.fetchUser(id);
  if (!user) {
      return Err(user.error());
  }
````

* No `throw`/`catch` is allowed except:

  * In 3rd-party library interop,
  * In `main()` as last-resort crash handler.

## Consequences

* ✅ Predictable error flow, no hidden control transfers.
* ✅ Simplifies integration with QML (errors become plain values).
* ✅ Easier unit testing (can match on Ok/Err).
* ❌ Slightly more verbose than exceptions.
* ❌ Requires discipline to avoid bypassing with raw types.

## Alternatives Considered

* **Exceptions**: rejected (unreliable across Qt signal/slots and async).
* **Error codes (int enums)**: rejected (no context payload, no message).

## References

* `Result.h` in project source.
* Unit tests under `tests/unit/result/`.