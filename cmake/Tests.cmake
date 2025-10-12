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
# 📥 GoogleTest FetchContent
# ---------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE) # Prevent GTest from being installed
FetchContent_MakeAvailable(googletest)

# ---------------------------------------------------------------
# 🎨 Required Qt Components for Tests
# ---------------------------------------------------------------
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
    Concurrent
    Test
)

# ---------------------------------------------------------------
# 🏗️ Test Runner Target
# ---------------------------------------------------------------
add_executable(TasqlyTestsRunner
    # 🔗 Integration Tests — Logging [Phase0]
    tests/integration/logging/test_error_reporter.cpp
    tests/integration/logging/test_error_reporter_feature_flag.cpp
    tests/integration/logging/test_feature_flag_logging.cpp
    tests/integration/logging/test_log_manager_extended.cpp
    tests/integration/logging/test_qml_error_to_toast_and_log.cpp

    # 🔗 Integration Tests — Navigation [Phase0]
    tests/integration/navigation/test_navigate_to_uc.cpp

    # 🔗 Integration Tests — Notifier [Phase0]
    tests/integration/notifier/test_notifier.cpp

    # 🔗 Integration Tests — Persistence [Phase0]
    tests/integration/persistence/test_seed_toggle.cpp

    # 🔗 Integration Tests — Settings [Phase0]
    tests/integration/settings/test_appsettingsmanager.cpp

    # 🔗 Integration Tests — Theme [Phase0]
    tests/integration/theme/test_theme_persistence.cpp

    # 🔗 Integration Tests — UseCases [Phase0]
    tests/integration/usecases/test_init_repositories.cpp
    tests/integration/usecases/test_save_user_settings.cpp


    # 🛠️ Unit Tests — Core [Phase0]
    tests/unit/core/test_app_context.cpp
    tests/unit/core/test_error.cpp
    tests/unit/core/test_goal.cpp
    tests/unit/core/test_page_state.cpp
    tests/unit/core/test_task.cpp
    tests/unit/core/test_theme_mode.cpp

    # 🛠️ Unit Tests — Domain [Phase0]
    tests/unit/domain/test_page_state.cpp

    # 🛠️ Unit Tests — Errors [Phase0]
    tests/unit/errors/test_app_errors.cpp

    # 🛠️ Unit Tests — Logging [Phase0]
    tests/unit/logging/test_autoflush.cpp
    tests/unit/logging/test_concurrency.cpp
    tests/unit/logging/test_error_reporter.cpp
    tests/unit/logging/test_fallback_writefile.cpp
    tests/unit/logging/test_log_facade.cpp
    tests/unit/logging/test_log_level_parsing.cpp
    tests/unit/logging/test_log_manager_failures.cpp
    tests/unit/logging/test_log_manager_filesink.cpp
    tests/unit/logging/test_log_manager_misc.cpp
    tests/unit/logging/test_masking.cpp
    tests/unit/logging/test_masking_concurrency.cpp
    tests/unit/logging/test_periodic_rotation.cpp
    tests/unit/logging/test_rotation_policy.cpp

    # 🛠️ Unit Tests — Navigation [Phase0]
    tests/unit/navigation/test_route_validation.cpp
    tests/unit/navigation/test_navigation_service_extended.cpp

    # 🛠️ Unit Tests — Persistence [Phase0]
    tests/unit/persistence/test_fake_uuid_clock.cpp
    tests/unit/persistence/test_inmemory_goal_repository.cpp
    tests/unit/persistence/test_inmemory_task_repository.cpp

    # 🛠️ Unit Tests — Settings [Phase0]
    tests/unit/settings/test_app_settings_manager.cpp
    tests/unit/settings/test_feature_flag_signal.cpp
    tests/unit/settings/test_feature_flags_manager.cpp

    # 🛠️ Unit Tests — UseCases [Phase0]
    tests/unit/usecases/test_switch_theme.cpp

    # 🛠️ Unit Tests — Theme [Phase0]
    tests/unit/theme/test_theme_manager.cpp

    # 🛠️ Unit Tests — Utils [Phase0]
    tests/unit/utils/test_system_clock.cpp
    tests/unit/utils/test_system_uuidgen.cpp
    # =========================================================
    # 🛠️ Unit Tests Domain — entities [Phase1][Slice1]
    tests/unit/domain/entities/test_DomainValidation.cpp

    # 🛠️ Unit Tests Domain — mappers [Phase1][Slice1]
    tests/unit/domain/mappers/test_TaskMapper.cpp

    # 🛠️ Integration Tests Domain [Phase1][Slice1]
    tests/integration/domain/test_FakeTaskRepository.cpp

    # 🧪 Main Test Files
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
        GTest::gtest
        GTest::gtest_main
        Qt6::Concurrent
        Qt6::Test
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
