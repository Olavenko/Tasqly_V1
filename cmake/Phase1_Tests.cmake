# ---------------------------------------------------------------
# 🧪 GoogleTest Integration — TasqlyTestsRunner
# ---------------------------------------------------------------
# 📌 Purpose : Integrate GoogleTest and define the Tasqly test target
# 🧱 Layer   : Build System (Testing Infrastructure)
# 👤 Author  : Mohamed Ali
# 🗓️ Updated : 2025-09-18
#
# 🧠 Description:
# - Downloads and configures GoogleTest (pinned version)
# - Defines the test runner target `TasqlyTestsRunner`
# - Links Qt + tasqly_core
# - Auto-discovers and registers tests with CTest
# ---------------------------------------------------------------

include(CTest)
enable_testing()

# ---------------------------------------------------------------
# 🏗️ Test Runner Target
# ---------------------------------------------------------------
add_executable(TasqlyTestsRunner

    #=========================================================
    # Phase[1]
    # =========================================================
    # 🛠️ integration Tests Domain — mappers [Phase1][Slice1]
    tests/integration/domain/mappers/test_P1_TaskRepositoryIntegration.cpp
    tests/integration/domain/mappers/test_P1_TaskMapperRepository_RoundTrip.cpp

    # 🧠 Integration Tests Infrastructure — Fallback Factory [Phase1][Slice2]
    tests/integration/persistence/test_P1_S2_TaskRepositoryFactory.cpp

    # 🧠 Integration Tests Infrastructure — Postgres Repository [Phase1][Slice2]
    tests/integration/persistence/test_P1_S2_PostgresTaskRepository.cpp

    # 🧠 Integration Tests migrations — Postgres SQL [Phase1][Slice2]
    tests/integration/migrations/test_P1_S2_Migrations.cpp

    # 🛠️ Unit Tests Domain — entities [Phase1][Slice1]
    tests/unit/domain/entities/test_P1_DomainValidation.cpp
    tests/unit/domain/entities/test_P1_TaskPriority.cpp
    tests/unit/domain/entities/test_P1_TaskStatus.cpp

    # 🛠️ Unit Tests Domain — errors [Phase1][Slice1]
    tests/unit/domain/errors/test_P1_DomainError.cpp
    tests/unit/domain/errors/test_P1_DomainResult.cpp

    # 🛠️ Unit Tests Domain — mappers [Phase1][Slice1]
    tests/unit/domain/mappers/test_P1_TaskMapper.cpp

    # 🧠 Unit Tests Infrastructure — Repositories [Phase1][Slice2]
    tests/unit/persistence/test_P1_S2_InMemoryTaskRepository.cpp

    # 🧪 Main Test Files
    tests/integration/common/DatabaseIntegrationFixture.h
    tests/integration/common/DatabaseIntegrationFixture.cpp
    tests/common/RuntimeDiagnostic.h
    tests/test_main.cpp
    tests/test_result.cpp
    tests/test_result_void.cpp
)

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
