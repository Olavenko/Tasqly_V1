# ADR 0005: Conditional `final` Class for Testing Builds

- **Status**: Accepted  
- **Date**: 2025-11-12  
- **Owners**: Tasqly Core Team  
- **Related**: Phase1 — Slice2  
- **Tags**: testing, build-flags, cpp-infra, architecture-safety

---

## Context

During the development and testing of **`P1_S2_PostgresTaskRepository`**,  
it became necessary to inherit from the class in testing environments (Unit and Integration)  
in order to access and verify the behavior of the protected method `executeQuery()` responsible for safely executing SQL statements.

However, the class was originally declared as `final`,  
which strictly prevents any subclassing — including test harnesses.  
The goal was to maintain **strict design integrity in production**,  
while allowing **flexibility in testing environments** for proper coverage.

---

## Decision

A **conditional `final` control mechanism** has been adopted using a dedicated build-time macro for testing environments:

cpp
#ifndef TASQLY_TESTING_BUILD
class P1_S2_PostgresTaskRepository final
#else
class P1_S2_PostgresTaskRepository
#endif
{
    ...
};
The macro is activated exclusively in testing builds (both Unit and Integration) via CMake:

cmake
Copy code
target_compile_definitions(TasqlyTestsRunner PRIVATE TASQLY_TESTING_BUILD=1)
In production builds → the class remains final, fully sealed from inheritance.

In testing builds → the class becomes open for inheritance, allowing access to protected members such as executeQuery() for controlled testing.

Consequences
✅ Benefits
Enables direct testing of protected methods without breaking encapsulation in production.

No impact on production code or ABI.

Clear intent — subclassing is explicitly restricted to testing builds.

Fully compliant with BuildPolicy.md and Testing_Standards_Combined.md.

⚠️ Drawbacks
Requires maintaining the macro definition within the testing CMake configuration.

Slightly increases header complexity with conditional preprocessor logic.

🔄 Trade-offs
This approach was chosen instead of permanently removing final,
to preserve strict architectural boundaries in production
while allowing full flexibility for testing coverage.

References
src/infra/db/P1_S2_PostgresTaskRepository.h

tests/Phase1_Tests.cmake

BuildPolicy.md

Testing_Standards_Combined.md

yaml
Copy code

---

📂 **Recommended file path:**  
docs/architecture/adr/0005-conditional-final-class-for-testing-builds.md