# 🧩 Project Files — Phase1

**Snapshot:** Project_Tree_Phase1_Slice2_20251117_134307.json

**Snapshot time:** 2025-11-17 13:43:07

**Generated:** 2025-11-17 14:03:41

---

### 📊 Summary

| Category   | Count |
|------------|-------|
| Src        | 34    |
| Tests      | 32    |
| Benchmarks | 3     |
| Total      | 69    |

---

### 🧱 SRC (34)

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
| P1_S2_IDbConnection.h                   | src/infra/db/P1_S2_IDbConnection.h                              |
| P1_S2_PostgresConnection.cpp            | src/infra/db/P1_S2_PostgresConnection.cpp                       |
| P1_S2_PostgresConnection.h              | src/infra/db/P1_S2_PostgresConnection.h                         |
| P1_S2_migration_0001_tasks.sql          | src/infra/db/migrations/P1_S2_migration_0001_tasks.sql          |
| P1_S2_migration_rollback_0001_tasks.sql | src/infra/db/migrations/P1_S2_migration_rollback_0001_tasks.sql |
| P1_S2_TaskRepositoryFactory.cpp         | src/infra/factories/P1_S2_TaskRepositoryFactory.cpp             |
| P1_S2_TaskRepositoryFactory.h           | src/infra/factories/P1_S2_TaskRepositoryFactory.h               |
| P1_S2_create_tasks.sql                  | src/infra/migrations/P1_S2_create_tasks.sql                     |
| P1_S2_drop_tasks.sql                    | src/infra/migrations/P1_S2_drop_tasks.sql                       |
| P1_S2_DbTaskRepository.cpp              | src/infra/persistence/P1_S2_DbTaskRepository.cpp                |
| P1_S2_DbTaskRepository.h                | src/infra/persistence/P1_S2_DbTaskRepository.h                  |
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
| P1_S2_Result.h                          | src/infra/runtime/P1_S2_Result.h                                |

### 🧪 TESTS (32)

| File                                         | Path                                                                        |
|----------------------------------------------|-----------------------------------------------------------------------------|
| RuntimeDiagnostic.h                          | tests/common/RuntimeDiagnostic.h                                            |
| P1_S2_FakeDbConnection.cpp                   | tests/fakes/db/P1_S2_FakeDbConnection.cpp                                   |
| P1_S2_FakeDbConnection.h                     | tests/fakes/db/P1_S2_FakeDbConnection.h                                     |
| FakeTaskRepository.cpp                       | tests/fakes/domain/FakeTaskRepository.cpp                                   |
| FakeTaskRepository.h                         | tests/fakes/domain/FakeTaskRepository.h                                     |
| P1_S2_TestTaskRepositoryFactory.cpp          | tests/fakes/factories/P1_S2_TestTaskRepositoryFactory.cpp                   |
| P1_S2_TestTaskRepositoryFactory.h            | tests/fakes/factories/P1_S2_TestTaskRepositoryFactory.h                     |
| DatabaseIntegrationFixture.cpp               | tests/integration/common/DatabaseIntegrationFixture.cpp                     |
| DatabaseIntegrationFixture.h                 | tests/integration/common/DatabaseIntegrationFixture.h                       |
| test_P1_S2_PostgresConnectionIntegration.cpp | tests/integration/db/test_P1_S2_PostgresConnectionIntegration.cpp           |
| test_P1_TaskMapperRepository_RoundTrip.cpp   | tests/integration/domain/mappers/test_P1_TaskMapperRepository_RoundTrip.cpp |
| test_P1_TaskRepositoryIntegration.cpp        | tests/integration/domain/mappers/test_P1_TaskRepositoryIntegration.cpp      |
| test_P1_S2_TaskRepositoryFactory.cpp         | tests/integration/factories/test_P1_S2_TaskRepositoryFactory.cpp            |
| test_P1_S2_Migrations.cpp                    | tests/integration/migrations/test_P1_S2_Migrations.cpp                      |
| test_P1_S2_DbTaskRepositoryIntegration.cpp   | tests/integration/persistence/test_P1_S2_DbTaskRepositoryIntegration.cpp    |
| test_P1_S2_TaskRepositoryFactory.cpp         | tests/integration/persistence/test_P1_S2_TaskRepositoryFactory.cpp          |
| test_P1_S2_FakeDbConnection.cpp              | tests/unit/db/test_P1_S2_FakeDbConnection.cpp                               |
| test_P1_S2_PostgresConnectionUnit.cpp        | tests/unit/db/test_P1_S2_PostgresConnectionUnit.cpp                         |
| test_P1_DomainValidation.cpp                 | tests/unit/domain/entities/test_P1_DomainValidation.cpp                     |
| test_P1_TaskPriority.cpp                     | tests/unit/domain/entities/test_P1_TaskPriority.cpp                         |
| test_P1_TaskStatus.cpp                       | tests/unit/domain/entities/test_P1_TaskStatus.cpp                           |
| test_P1_DomainError.cpp                      | tests/unit/domain/errors/test_P1_DomainError.cpp                            |
| test_P1_DomainResult.cpp                     | tests/unit/domain/errors/test_P1_DomainResult.cpp                           |
| test_P1_TaskMapper.cpp                       | tests/unit/domain/mappers/test_P1_TaskMapper.cpp                            |
| test_P1_TaskMapper_Single.cpp                | tests/unit/domain/mappers/test_P1_TaskMapper_Single.cpp                     |
| test_P1_S2_DbTaskRepository.cpp              | tests/unit/persistence/test_P1_S2_DbTaskRepository.cpp                      |
| test_P1_S2_InMemoryTaskRepository.cpp        | tests/unit/persistence/test_P1_S2_InMemoryTaskRepository.cpp                |
| test_P1_AppSettings.cpp                      | tests/unit/runtime/test_P1_AppSettings.cpp                                  |
| test_P1_Error.cpp                            | tests/unit/runtime/test_P1_Error.cpp                                        |
| test_P1_Logger.cpp                           | tests/unit/runtime/test_P1_Logger.cpp                                       |
| test_P1_Notifier.cpp                         | tests/unit/runtime/test_P1_Notifier.cpp                                     |
| test_P1_S2_TaskRepositoryFactory.cpp         | tests/unit/runtime/test_P1_S2_TaskRepositoryFactory.cpp                     |

### 🚀 BENCHMARKS (3)

| File                             | Path                                                    |
|----------------------------------|---------------------------------------------------------|
| P1_S1_Bench_DomainValidation.cpp | benchmarks/unit/domain/P1_S1_Bench_DomainValidation.cpp |
| P1_S1_Bench_TaskEntity.cpp       | benchmarks/unit/domain/P1_S1_Bench_TaskEntity.cpp       |
| P1_S1_Bench_TaskMapper.cpp       | benchmarks/unit/domain/P1_S1_Bench_TaskMapper.cpp       |

