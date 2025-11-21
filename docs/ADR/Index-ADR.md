# ADR Index — Tasqly Project

This directory contains Architecture Decision Records (ADRs) for the Tasqly project.
Each ADR documents a significant architectural or tooling decision.

## ADR List

### phase0/Slice0 (9 ADRs)

| ID   | Title                                                             | Status | Last Modified | File                                                    |
|------|-------------------------------------------------------------------|--------|---------------|---------------------------------------------------------|
| 0000 | Analysis Document Project Initialization & Testing Foundation     | Draft  | 2025-09-06    | [0000](phase0/Slice0/0000-analysis.md)                  |
| 0001 | PlantUML Adoption                                                 | Draft  | 2025-08-29    | [0001](phase0/Slice0/0001-plantuml-adoption.md)         |
| 0002 | Build Toolchain Policy                                            | Draft  | 2025-09-01    | [0002](phase0/Slice0/0002-build-toolchain.md)           |
| 0003 | Google Test Framework                                             | Draft  | 2025-08-29    | [0003](phase0/Slice0/0003-google-test.md)               |
| 0004 | CI Workflows Definition                                           | Draft  | 2025-08-29    | [0004](phase0/Slice0/0004-ci-workflows.md)              |
| 0005 | Git Hygiene                                                       | Draft  | 2025-08-29    | [0005](phase0/Slice0/0005-git-hygiene.md)               |
| 0006 | Debugger Policy                                                   | Draft  | 2025-09-03    | [0006](phase0/Slice0/0006-debugger-policy.md)           |
| 0007 | Build Toolchain CI Policy                                         | Draft  | 2025-09-03    | [0007](phase0/Slice0/0007-Build-Toolchain-CI-Policy.md) |
| 0008 | Docker Dev Container — Development Environment Baseline (Phase 1) | Draft  | 2025-09-10    | [0008](phase0/Slice0/0008-docker-dev-container.md)      |

### phase0/Slice1 (2 ADRs)

| ID   | Title                                          | Status | Last Modified | File                                                                     |
|------|------------------------------------------------|--------|---------------|--------------------------------------------------------------------------|
| 0001 | Result-Everywhere-Error-Propagation-Policy     | Draft  | 2025-09-06    | [0001](phase0/Slice1/0001-Result-Everywhere-Error-Propagation-Policy.md) |
| 0002 | Feature-Flags-Manager (Runtime-vs-CMake-flags) | Draft  | 2025-09-06    | [0002](phase0/Slice1/0002-Feature-Flags-Manager.md)                      |

### phase0/Slice2 (6 ADRs)

| ID   | Title                                | Status | Last Modified | File                                                      |
|------|--------------------------------------|--------|---------------|-----------------------------------------------------------|
| 0001 | QML-Facades-vs-Direct-Infra-Exposure | Draft  | 2025-09-06    | [0001](phase0/Slice2/0001-qml-facades-vs-direct-infra.md) |
| 0002 | Theme-Persistence-Approach           | Draft  | 2025-09-06    | [0002](phase0/Slice2/0002-theme-persistence-approach.md)  |
| 0003 | QML-Paths-Resolution                 | Draft  | 2025-09-06    | [0003](phase0/Slice2/0003-qml-paths-resolution.md)        |
| 0004 | Resources-vs-QML-Module              | Draft  | 2025-09-06    | [0004](phase0/Slice2/0004-resources-vs-qml-module.md)     |
| 0005 | SettingsPage-Architecture            | Draft  | 2025-09-06    | [0005](phase0/Slice2/0005-settingspage-architecture.md)   |
| 0006 | Split-CI-Jobs-and-Enable-Build-Cache | Draft  | 2025-09-06    | [0006](phase0/Slice2/0006-adr-ci-cache-split.md)          |

### phase0/Slice3 (5 ADRs)

| ID   | Title                                                | Status | Last Modified | File                                                                   |
|------|------------------------------------------------------|--------|---------------|------------------------------------------------------------------------|
| 0001 | QML-Code-Model-False-Positives-in-Qt-Creator         | Draft  | 2025-09-06    | [0001](phase0/Slice3/0001-qml-code-model-false-positives.md)           |
| 0002 | Automation-Scripts-for-CMake-Sorting-and-Maintenance | Draft  | 2025-09-06    | [0002](phase0/Slice3/0002-cmake-sorting-scripts.md)                    |
| 0003 | Route-as-Value-and-Guard-Policy                      | Draft  | 2025-09-06    | [0003](phase0/Slice3/0003-route-guard-policy.md)                       |
| 0004 | qml-singleton-vs-named-element                       | Draft  | 2025-09-10    | [0004](phase0/Slice3/0004-qml-singleton-vs-named-element.md)           |
| 0005 | navigation-loader-fallback-testing-fixes             | Draft  | 2025-09-06    | [0005](phase0/Slice3/0005-navigation-loader-fallback-testing-fixes.md) |

### phase0/Slice4 (5 ADRs)

| ID   | Title                              | Status | Last Modified | File                                                       |
|------|------------------------------------|--------|---------------|------------------------------------------------------------|
| 0001 | Repository-Error-Codes             | Draft  | 2025-09-07    | [0001](phase0/Slice4/0001-repository-error-codes.md)       |
| 0002 | Include-Path-Policy                | Draft  | 2025-09-07    | [0002](phase0/Slice4/0002-include-path-policy.md)          |
| 0003 | Namespace-And-Naming-Policy        | Draft  | 2025-09-07    | [0003](phase0/Slice4/0003-namespace-naming-policy.md)      |
| 0004 | Tracking-Of-Newly-Introduced-Files | Draft  | 2025-09-07    | [0004](phase0/Slice4/0004-tracking-new-files.md)           |
| 0005 | qml-tasklistpage-tests-phase0      | Draft  | 2025-09-08    | [0005](phase0/Slice4/0005-adr_qml_task_list_page_tests.md) |

### phase1/slice1 (8 ADRs)

| ID   | Title                                                              | Status   | Last Modified | File                                                         |
|------|--------------------------------------------------------------------|----------|---------------|--------------------------------------------------------------|
| 0001 | Domain Entities, Contracts & Mappers                               | Draft    | 2025-10-09    | [0001](phase1/slice1/0001-phase_1_slice_1_analysis.md)       |
| 0002 | Task Entity & Contracts — No Qt/DB in Domain                       | Draft    | 2025-10-06    | [0002](phase1/slice1/0002-task_entity_contracts.md)          |
| 0003 | DTO Mappers vs Direct Entity Exposure                              | Draft    | 2025-10-06    | [0003](phase1/slice1/0003-dto_vs_direct_entity.md)           |
| 0004 | Mapper Strategy — Manual vs Codegen                                | Draft    | 2025-10-06    | [0004](phase1/slice1/0004-mapper_strategy.md)                |
| 0005 | Phase 1 Domain Isolation Policy                                    | Accepted | 2025-10-09    | [0005](phase1/slice1/0005-phase1_domain_isolation_policy.md) |
| 0006 | Task Mapper Strategy: Manual Conversion                            | :Doing;  | 2025-10-14    | [0006](phase1/slice1/0006-task-mapper-strategy-manual.md)    |
| 0007 | Phase0/Phase1 Header Collision in Task.h                           | Accepted | 2025-10-14    | [0007](phase1/slice1/0007-phase0_Phase1_Header_Collision.md) |
| 0008 | Namespace Isolation & Resolution Strategy in Domain/Core (Phase 1) | Accepted | 2025-10-14    | [0008](phase1/slice1/0008-Namespace-Isolation.md)            |

### phase1/slice2 (8 ADRs)

| ID   | Title                                                        | Status | Last Modified | File                                                                                        |
|------|--------------------------------------------------------------|--------|---------------|---------------------------------------------------------------------------------------------|
| 0001 | runtime-error-layer-adoption                                 | Draft  | 2025-10-21    | [0001](phase1/slice2/0001-runtime-error-layer-adoption.md)                                  |
| 0002 | PostgreSQL Connection Policy and Fallback to InMemory        | Draft  | 2025-10-23    | [0002](phase1/slice2/0002-db_connection_and_fallback.md)                                    |
| 0003 | handle-postgresql-tests-on-windows-ci                        | Draft  | 2025-11-03    | [0003](phase1/slice2/0003-handle-postgresql-tests-on-windows-ci.md)                         |
| 0004 | Conditional-Skip-For-PostgreSQL-Integration-Tests-On-Windows | Draft  | 2025-11-06    | [0004](phase1/slice2/0004-Conditional-Skip-For-PostgreSQL-Integration-Tests-On-Windows.md)  |
| 0005 | Conditional `final` Class for Testing Builds                 | Draft  | 2025-11-12    | [0005](phase1/slice2/0005-Conditional `final` Class for Testing Builds.md)                  |
| 0006 | testing-access-limitation-in-postgrestaskrepository          | Draft  | 2025-11-12    | [0006](phase1/slice2/0006 — Testing Access Limitation in `P1_S2_PostgresTaskRepository`.md) |
| 0007 | dependency-injection-for-db-access                           | Draft  | 2025-11-12    | [0007](phase1/slice2/0007 dependency-injection-for-db-access.md)                            |
| 0025 | db-abstraction-layer-and-repo-refactor                       | Draft  | 2025-11-15    | [0025](phase1/slice2/0008 db-abstraction-layer-and-repo-refactor.md)                        |

---

## Status Legend
- **Accepted** → Implemented and in use.
- **Planned** → Approved but not yet implemented fully.
- **Proposed** → Under discussion.
- **Draft** → Created but not yet reviewed.

*Index generated on 2025-11-21 16:52 UTC*
