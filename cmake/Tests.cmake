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

    # Phase[0]
    #=========================================
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

    #=========================================================
    # Phase[1]
    # =========================================================
    # 🛠️ integration Tests Domain — mappers [Phase1][Slice1]
    tests/integration/domain/mappers/test_P1_TaskRepositoryIntegration.cpp
    tests/integration/domain/mappers/test_P1_TaskMapperRepository_RoundTrip.cpp

    # 🛠️ Unit Tests Domain — entities [Phase1][Slice1]
    tests/unit/domain/entities/test_P1_DomainValidation.cpp
    tests/unit/domain/entities/test_P1_TaskPriority.cpp
    tests/unit/domain/entities/test_P1_TaskStatus.cpp

    # 🛠️ Unit Tests Domain — errors [Phase1][Slice1]
    tests/unit/domain/errors/test_P1_DomainError.cpp
    tests/unit/domain/errors/test_P1_DomainResult.cpp

    # 🛠️ Unit Tests Domain — mappers [Phase1][Slice1]
    tests/unit/domain/mappers/test_P1_TaskMapper.cpp

    # 🧪 Main Test Files
    tests/common/RuntimeDiagnostic.h
    tests/test_main.cpp
    tests/test_result.cpp
    tests/test_result_void.cpp
)

#=====================================================================
# 🧩 Diagnostic: print compiler configuration info
message(STATUS "====================================================")
message(STATUS " Tasqly Diagnostic Build Info")
message(STATUS "====================================================")
message(STATUS "C++ Compiler ID     : ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ Compiler Path   : ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ Flags (common)  : ${CMAKE_CXX_FLAGS}")
message(STATUS "C++ Flags (debug)   : ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "Generator           : ${CMAKE_GENERATOR}")
message(STATUS "System Name         : ${CMAKE_SYSTEM_NAME}")
message(STATUS "Build Type          : ${CMAKE_BUILD_TYPE}")

# ✅ Detect ABI definition at compile time
include(CheckCXXSourceCompiles)
check_cxx_source_compiles("
#include <iostream>
int main() {
#ifdef _GLIBCXX_USE_CXX11_ABI
# if _GLIBCXX_USE_CXX11_ABI == 1
#error ABI_IS_1
# else
#error ABI_IS_0
# endif
#else
#error ABI_UNDEFINED
#endif
return 0;
}
" TASQLY_ABI_DETECTED)

if (TASQLY_ABI_DETECTED)
    message(STATUS "ABI Mode (detected): _GLIBCXX_USE_CXX11_ABI = 1")
else()
    # try again manually to extract macro value (compile only)
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -dM -E -x c++ NUL
        OUTPUT_VARIABLE _macro_dump
    )
    if (_macro_dump MATCHES "_GLIBCXX_USE_CXX11_ABI 0")
        message(STATUS "ABI Mode (detected): _GLIBCXX_USE_CXX11_ABI = 0")
    elseif (_macro_dump MATCHES "_GLIBCXX_USE_CXX11_ABI 1")
        message(STATUS "ABI Mode (detected): _GLIBCXX_USE_CXX11_ABI = 1")
    else()
        message(STATUS "ABI Mode (detected): Not defined")
    endif()
endif()

message(STATUS "====================================================")

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
