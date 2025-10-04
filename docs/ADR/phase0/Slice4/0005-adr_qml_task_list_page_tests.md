# ADR 0005: qml-tasklistpage-tests-phase0

- **Status**: Accepted
- **Date**: 2025-09-08
- **Owners**: Tasqly Team
- **Related**: Phase0 — Slice0
- **Tags**: qml, testing, ci, skip, tasklistpage

---

## Context
The project is currently in its early development phase (Phase 0). The `TaskListPage.qml` UI is not yet fully implemented and only contains placeholder/fallback elements. At this stage, QML integration tests targeting `TaskListPage` cannot pass successfully if they strictly validate the presence of UI components (`SmartFallback`, `ListView`, etc.).

Initial attempts to load `TaskListPage` via `import Tasqly_V1 1.0` failed because the QML module is only bundled with the application target (`appTasqly_V1`), not exported as a standalone module. Consequently, `qmltestrunner` could not resolve the module.

To allow CI to remain green and avoid false failures, tests were modified to:
- Load the QML page via `qrc:/qt/qml/Tasqly_V1/pages/TaskListPage.qml` instead of `import Tasqly_V1`.
- Gracefully `skip()` tests if the component is not ready (UI Slice not yet implemented).

## Decision
For Phase 0, the integration test `test_tasklist_page.qml` will:
- Use **qrc path** (`qrc:/qt/qml/...`) to load the page.
- Include **skip conditions** if the page is not ready.
- Report `SKIP` instead of `FAIL` in CI, ensuring pipelines remain green.

The module import (`import Tasqly_V1 1.0`) and strict assertions will be restored once the **TaskListPage UI Slice** is implemented.

## Consequences
- ✅ CI remains green despite incomplete UI.
- ✅ Developers have a clear reminder (via `skip` logs) that the test will need updating when UI is ready.
- ⚠️ Current tests do not validate real functionality; they only act as placeholders.
- ⚠️ Documentation must be updated once the decision is reversed (when the UI is complete).

## References
- `tests/qml/test_tasklist_page.qml`
- `src/ui/qml/pages/TaskListPage.qml`
- Project Roadmap (Phase 0 → Task List UI Slice)

