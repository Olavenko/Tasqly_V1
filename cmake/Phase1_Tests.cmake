# ---------------------------------------------------------------
# 🧪 GoogleTest Integration — TasqlyTestsRunner
# ---------------------------------------------------------------
# 📌 Purpose : Integrate GoogleTest and define the Tasqly test target
# 🧱 Layer   : Build System (Testing Infrastructure)
# 👤 Author  : Mohamed Ali
# 🗓️ Updated : 2025-11-03
#
# 🧠 Description:
# - Downloads and configures GoogleTest (pinned version)
# - Defines the test runner target `TasqlyTestsRunner`
# - Links Qt + tasqly_core
# - Includes all tests except PostgreSQL integrations on Windows CI
# ---------------------------------------------------------------

include(CTest)
enable_testing()

# ---------------------------------------------------------------
# 🏗️ Test Runner Target (Dynamic PostgreSQL Detection)
# ---------------------------------------------------------------
set(_test_sources
    #=========================================================
    # Phase[1]
    #=========================================================
    # 🛠️ Integration Tests — Domain (Mappers)
    tests/integration/domain/mappers/test_P1_TaskRepositoryIntegration.cpp
    tests/integration/domain/mappers/test_P1_TaskMapperRepository_RoundTrip.cpp

    # 🧠 Integration Tests — Persistence Fallback Factory
    tests/integration/persistence/test_P1_S2_TaskRepositoryFactory.cpp

    # 🛠️ Unit Tests — Domain Entities
    tests/unit/domain/entities/test_P1_DomainValidation.cpp
    tests/unit/domain/entities/test_P1_TaskPriority.cpp
    tests/unit/domain/entities/test_P1_TaskStatus.cpp

    # 🧠 Unit Tests — Domain Errors
    tests/unit/domain/errors/test_P1_DomainError.cpp
    tests/unit/domain/errors/test_P1_DomainResult.cpp

    # 🧠 Unit Tests — Domain Mappers
    tests/unit/domain/mappers/test_P1_TaskMapper.cpp

    # 🧠 Unit Tests — Persistence (In-Memory Repository)
    tests/unit/persistence/test_P1_S2_InMemoryTaskRepository.cpp

    # 🧠 Unit Tests — Persistence (Postgres Repository - Error Handling)
    tests/unit/persistence/test_P1_S2_PostgresTaskRepository.cpp

    # 🧩 Unit Tests — Runtime (Phase 1)
    tests/unit/runtime/test_P1_Error.cpp
    tests/unit/runtime/test_P1_Notifier.cpp
    tests/unit/runtime/test_P1_AppSettings.cpp
    tests/unit/runtime/test_P1_Logger.cpp
    tests/unit/runtime/test_P1_S2_TaskRepositoryFactory.cpp

    # 🧩 Shared Common
    tests/common/RuntimeDiagnostic.h
    tests/test_main.cpp
    tests/test_result.cpp
    tests/test_result_void.cpp
)

# ---------------------------------------------------------------
# 🐘 Conditional PostgreSQL Integration Tests
# ---------------------------------------------------------------
if (TASQLY_PG_AVAILABLE)
    message(STATUS "[Tests] PostgreSQL detected — including DB integration tests")
    list(APPEND _test_sources
        tests/integration/common/DatabaseIntegrationFixture.cpp
        tests/integration/common/DatabaseIntegrationFixture.h
        tests/integration/migrations/test_P1_S2_Migrations.cpp
        tests/integration/persistence/test_P1_S2_PostgresTaskRepository.cpp
    )
else()
    message(WARNING "[Tests] PostgreSQL not available — skipping DB integration tests")
endif()

# ---------------------------------------------------------------
# 🧱 Create Test Runner Target
# ---------------------------------------------------------------
add_executable(TasqlyTestsRunner ${_test_sources})

# ---------------------------------------------------------------
# 🧩 Define conditional compile-time flag for DB skipping
# ---------------------------------------------------------------
if (NOT TASQLY_PG_AVAILABLE)
    message(WARNING "[Tests] PostgreSQL unavailable — defining TASQLY_SKIP_DB_TESTS for conditional compilation.")
    target_compile_definitions(TasqlyTestsRunner PRIVATE TASQLY_SKIP_DB_TESTS)
endif()

# ---------------------------------------------------------------
# 🔗 Test Runner Dependencies
# ---------------------------------------------------------------
target_link_libraries(TasqlyTestsRunner
    PRIVATE
        tasqly_core
        tasqly_fakes
        gtest
        gmock
        Qt6::Core
        Qt6::Test
        Qt6::Quick
        Qt6::QuickControls2
)

# ---------------------------------------------------------------
# 📂 Include Directories
# ---------------------------------------------------------------
target_include_directories(TasqlyTestsRunner PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

# ---------------------------------------------------------------
# ⚙️ Target Properties
# ---------------------------------------------------------------
set_target_properties(TasqlyTestsRunner PROPERTIES AUTOMOC ON)
target_compile_definitions(TasqlyTestsRunner PRIVATE
    PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

# ---------------------------------------------------------------
# 🛡️ Quality Gates
# ---------------------------------------------------------------
if (COMMAND tasqly_apply_warnings)
    tasqly_apply_warnings(TasqlyTestsRunner)
endif()
if (COMMAND tasqly_apply_debug_sanitizers)
    tasqly_apply_debug_sanitizers(TasqlyTestsRunner)
endif()

# ---------------------------------------------------------------
# 🔍 GoogleTest Integration with CTest
# ---------------------------------------------------------------
include(GoogleTest)
gtest_discover_tests(TasqlyTestsRunner
    TEST_PREFIX TasqlyTests.
    WORKING_DIRECTORY $<TARGET_FILE_DIR:TasqlyTestsRunner>
    DISCOVERY_MODE PRE_TEST
    PROPERTIES ENVIRONMENT
        "QT_QPA_PLATFORM=offscreen;QSG_RHI_BACKEND=software"
)

message(STATUS "[Tests] TasqlyTestsRunner enabled")
message(STATUS "[Tests] Linked against tasqly_core + Qt + GoogleTest from ExternalDeps")
