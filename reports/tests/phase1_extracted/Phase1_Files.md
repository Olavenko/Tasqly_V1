# 🧩 Project Files — Phase1

**Snapshot:** Project_Tree_Phase1_Slice2_20251110_142102.json

**Snapshot time:** 2025-11-10 14:21:02

**Generated:** 2025-11-11 14:07:03

---

### 📊 Summary

| Category   | Count |
|------------|-------|
| Src        | 30    |
| Tests      | 25    |
| Benchmarks | 3     |
| Total      | 58    |

---

### 🧱 SRC (30)

| File                                    | Path                                                            |
|-----------------------------------------|-----------------------------------------------------------------|
| P1_ITaskRepository.h                    | src/domain/core/contracts/P1_ITaskRepository.h                  |
| P1_DomainValidation.cpp                 | src/domain/core/entities/P1_DomainValidation.cpp                |
| P1_DomainValidation.h                   | src/domain/core/entities/P1_DomainValidation.h                  |
| P1_Task.h                               | src/domain/core/entities/P1_Task.h                              |
| P1_TaskPriority.h                       | src/domain/core/entities/P1_TaskPriority.h                      |
| P1_TaskStatus.h                         | src/domain/core/entities/P1_TaskStatus.h                        |
| P1_DomainError.h                        | src/domain/core/errors/P1_DomainError.h                         |
| P1_DomainResult.h                       | src/domain/core/errors/P1_DomainResult.h                        |
| P1_TaskDto.h                            | src/domain/core/mappers/P1_TaskDto.h                            |
| P1_TaskMapper.cpp                       | src/domain/core/mappers/P1_TaskMapper.cpp                       |
| P1_TaskMapper.h                         | src/domain/core/mappers/P1_TaskMapper.h                         |
| P1_S2_PostgresTaskRepository.cpp        | src/infra/db/P1_S2_PostgresTaskRepository.cpp                   |
| P1_S2_PostgresTaskRepository.h          | src/infra/db/P1_S2_PostgresTaskRepository.h                     |
| P1_S2_migration_0001_tasks.sql          | src/infra/db/migrations/P1_S2_migration_0001_tasks.sql          |
| P1_S2_migration_rollback_0001_tasks.sql | src/infra/db/migrations/P1_S2_migration_rollback_0001_tasks.sql |
| P1_S2_TaskRepositoryFactory.cpp         | src/infra/factories/P1_S2_TaskRepositoryFactory.cpp             |
| P1_S2_TaskRepositoryFactory.h           | src/infra/factories/P1_S2_TaskRepositoryFactory.h               |
| P1_S2_create_tasks.sql                  | src/infra/migrations/P1_S2_create_tasks.sql                     |
| P1_S2_drop_tasks.sql                    | src/infra/migrations/P1_S2_drop_tasks.sql                       |
| P1_S2_InMemoryTaskRepository.cpp        | src/infra/persistence/P1_S2_InMemoryTaskRepository.cpp          |
| P1_S2_InMemoryTaskRepository.h          | src/infra/persistence/P1_S2_InMemoryTaskRepository.h            |
| P1_AppSettings.cpp                      | src/infra/runtime/P1_AppSettings.cpp                            |
| P1_AppSettings.h                        | src/infra/runtime/P1_AppSettings.h                              |
| P1_Error.cpp                            | src/infra/runtime/P1_Error.cpp                                  |
| P1_Error.h                              | src/infra/runtime/P1_Error.h                                    |
| P1_LogLevel.h                           | src/infra/runtime/P1_LogLevel.h                                 |
| P1_Logger.cpp                           | src/infra/runtime/P1_Logger.cpp                                 |
| P1_Logger.h                             | src/infra/runtime/P1_Logger.h                                   |
| P1_Notifier.cpp                         | src/infra/runtime/P1_Notifier.cpp                               |
| P1_Notifier.h                           | src/infra/runtime/P1_Notifier.h                                 |

### 🧪 TESTS (25)

| File                                       | Path                                                                        |
|--------------------------------------------|-----------------------------------------------------------------------------|
| RuntimeDiagnostic.h                        | tests/common/RuntimeDiagnostic.h                                            |
| FakeTaskRepository.cpp                     | tests/fakes/domain/FakeTaskRepository.cpp                                   |
| FakeTaskRepository.h                       | tests/fakes/domain/FakeTaskRepository.h                                     |
| DatabaseIntegrationFixture.cpp             | tests/integration/common/DatabaseIntegrationFixture.cpp                     |
| DatabaseIntegrationFixture.h               | tests/integration/common/DatabaseIntegrationFixture.h                       |
| test_P1_TaskMapperRepository_RoundTrip.cpp | tests/integration/domain/mappers/test_P1_TaskMapperRepository_RoundTrip.cpp |
| test_P1_TaskRepositoryIntegration.cpp      | tests/integration/domain/mappers/test_P1_TaskRepositoryIntegration.cpp      |
| test_P1_S2_TaskRepositoryFactory.cpp       | tests/integration/factories/test_P1_S2_TaskRepositoryFactory.cpp            |
| test_P1_S2_Migrations.cpp                  | tests/integration/migrations/test_P1_S2_Migrations.cpp                      |
| test_P1_S2_PostgresTaskRepository.cpp      | tests/integration/persistence/test_P1_S2_PostgresTaskRepository.cpp         |
| test_P1_S2_TaskRepositoryFactory.cpp       | tests/integration/persistence/test_P1_S2_TaskRepositoryFactory.cpp          |
| test_P1_DomainValidation.cpp               | tests/unit/domain/entities/test_P1_DomainValidation.cpp                     |
| test_P1_TaskPriority.cpp                   | tests/unit/domain/entities/test_P1_TaskPriority.cpp                         |
| test_P1_TaskStatus.cpp                     | tests/unit/domain/entities/test_P1_TaskStatus.cpp                           |
| test_P1_DomainError.cpp                    | tests/unit/domain/errors/test_P1_DomainError.cpp                            |
| test_P1_DomainResult.cpp                   | tests/unit/domain/errors/test_P1_DomainResult.cpp                           |
| test_P1_TaskMapper.cpp                     | tests/unit/domain/mappers/test_P1_TaskMapper.cpp                            |
| test_P1_TaskMapper_Single.cpp              | tests/unit/domain/mappers/test_P1_TaskMapper_Single.cpp                     |
| test_P1_S2_InMemoryTaskRepository.cpp      | tests/unit/persistence/test_P1_S2_InMemoryTaskRepository.cpp                |
| test_P1_S2_PostgresTaskRepository.cpp      | tests/unit/persistence/test_P1_S2_PostgresTaskRepository.cpp                |
| test_P1_AppSettings.cpp                    | tests/unit/runtime/test_P1_AppSettings.cpp                                  |
| test_P1_Error.cpp                          | tests/unit/runtime/test_P1_Error.cpp                                        |
| test_P1_Logger.cpp                         | tests/unit/runtime/test_P1_Logger.cpp                                       |
| test_P1_Notifier.cpp                       | tests/unit/runtime/test_P1_Notifier.cpp                                     |
| test_P1_S2_TaskRepositoryFactory.cpp       | tests/unit/runtime/test_P1_S2_TaskRepositoryFactory.cpp                     |

### 🚀 BENCHMARKS (3)

| File                             | Path                                                    |
|----------------------------------|---------------------------------------------------------|
| P1_S1_Bench_DomainValidation.cpp | benchmarks/unit/domain/P1_S1_Bench_DomainValidation.cpp |
| P1_S1_Bench_TaskEntity.cpp       | benchmarks/unit/domain/P1_S1_Bench_TaskEntity.cpp       |
| P1_S1_Bench_TaskMapper.cpp       | benchmarks/unit/domain/P1_S1_Bench_TaskMapper.cpp       |

