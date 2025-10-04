# ---------------------------------------------------------------
# 📊 Reports & Output Configuration (Updated / Fixed)
# ---------------------------------------------------------------
# 📌 Purpose : Define output directories & reporting options
# 🧱 Layer   : Build System (Reporting Infrastructure)
# 👤 Author  : Mohamed Ali
# 🗓️ Updated : 2025-09-21 (Fixed: removed --modules for Windows)
#
# 🧠 Description:
# - Adds options to enable/disable different test categories
# - Sets unified reports directory under ${CMAKE_SOURCE_DIR}/reports
# - Splits coverage into tests vs benchmarks reports
# ---------------------------------------------------------------

# 🔘 Options per test category
option(ENABLE_CPP_TESTS    "Enable GoogleTest C++ unit/integration tests" ON)
option(ENABLE_BENCHMARKS   "Enable Google Benchmark tests"                ON)
option(ENABLE_QML_TESTS    "Enable QML UI tests"                          OFF) # Phase 1
option(ENABLE_E2E_TESTS    "Enable End-to-End tests"                      OFF) # Phase 2
option(ENABLE_STRESS_TESTS "Enable Stress tests"                          OFF) # Phase 3+
option(ENABLE_COVERAGE     "Enable code coverage reports"                 OFF)

# 🗂️ Base reports directory
set(REPORTS_DIR ${CMAKE_SOURCE_DIR}/reports)
file(MAKE_DIRECTORY ${REPORTS_DIR})

# ---------------------------------------------------------------
# 🧪 GoogleTest Reports
# ---------------------------------------------------------------
if(ENABLE_CPP_TESTS)
    message(STATUS "[Reports] GoogleTest enabled ${REPORTS_DIR}/tests/gtest")
    file(MAKE_DIRECTORY ${REPORTS_DIR}/tests/gtest)

    add_test(NAME Tasqly_GTest_All
        COMMAND TasqlyTestsRunner
            --gtest_output=xml:${REPORTS_DIR}/tests/gtest/gtest_report.xml
    )
endif()

# ---------------------------------------------------------------
# ⚡ Benchmark Reports
# ---------------------------------------------------------------
if(ENABLE_BENCHMARKS)
    message(STATUS "[Reports] Benchmarks enabled ${REPORTS_DIR}/benchmarks")
    file(MAKE_DIRECTORY ${REPORTS_DIR}/benchmarks)

    add_custom_target(run_benchmarks
        COMMAND TasqlyBenchmarksRunner
            --benchmark_format=json
            --benchmark_out=${REPORTS_DIR}/benchmarks/benchmarks.json
        DEPENDS TasqlyBenchmarksRunner
        COMMENT "Running Google Benchmarks and exporting JSON report"
    )
endif()

# ---------------------------------------------------------------
# 📈 Code Coverage Integration
# ---------------------------------------------------------------
if(ENABLE_COVERAGE)
    message(STATUS "[Reports] Coverage enabled ${REPORTS_DIR}/coverage")
    file(MAKE_DIRECTORY ${REPORTS_DIR}/coverage)

    # 🟢 Windows: OpenCppCoverage
    if(WIN32)
        file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/src" SRC_PATH_NATIVE)
        file(TO_NATIVE_PATH "${REPORTS_DIR}/coverage" REPORTS_PATH_NATIVE)
        file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}" BINARY_DIR_NATIVE)

        # ✅ Coverage for Tests
        if(ENABLE_CPP_TESTS AND TARGET TasqlyTestsRunner)
            file(TO_NATIVE_PATH "${REPORTS_DIR}/coverage/tests/html" REPORT_TESTS_HTML)
            file(TO_NATIVE_PATH "${REPORTS_DIR}/coverage/tests/coverage.xml" REPORT_TESTS_XML)
            file(MAKE_DIRECTORY ${REPORTS_DIR}/coverage/tests/html)
            file(TO_NATIVE_PATH "$<TARGET_FILE:TasqlyTestsRunner>" TEST_RUNNER_NATIVE)

            add_custom_target(coverage_tests
                COMMAND OpenCppCoverage
                    --export_type=html:${REPORT_TESTS_HTML}
                    --export_type=cobertura:${REPORT_TESTS_XML}
                    --working_dir "${BINARY_DIR_NATIVE}"
                    --cover_children
                    --sources "${SRC_PATH_NATIVE}"
                    -- "${TEST_RUNNER_NATIVE}"
                DEPENDS TasqlyTestsRunner
                COMMENT "Running Tests with OpenCppCoverage reports/coverage/tests/"
            )
        endif()

        # ✅ Coverage for Benchmarks
        if(ENABLE_BENCHMARKS AND TARGET TasqlyBenchmarksRunner)
            file(TO_NATIVE_PATH "${REPORTS_DIR}/coverage/benchmarks/html" REPORT_BENCH_HTML)
            file(TO_NATIVE_PATH "${REPORTS_DIR}/coverage/benchmarks/coverage.xml" REPORT_BENCH_XML)
            file(MAKE_DIRECTORY ${REPORTS_DIR}/coverage/benchmarks/html)
            file(TO_NATIVE_PATH "$<TARGET_FILE:TasqlyBenchmarksRunner>" BENCH_RUNNER_NATIVE)

            add_custom_target(coverage_benchmarks
                COMMAND OpenCppCoverage
                    --export_type=html:${REPORT_BENCH_HTML}
                    --export_type=cobertura:${REPORT_BENCH_XML}
                    --working_dir "${BINARY_DIR_NATIVE}"
                    --cover_children
                    --sources "${SRC_PATH_NATIVE}"
                    -- "${BENCH_RUNNER_NATIVE}"
                DEPENDS TasqlyBenchmarksRunner
                COMMENT "Running Benchmarks with OpenCppCoverage reports/coverage/benchmarks/"
            )
        endif()

    # 🟠 Linux: GCC/Clang + lcov
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        set(COVERAGE_TARGETS
            appTasqly_V1
            tasqly_core
            TasqlyTestsRunner
            TasqlyBenchmarksRunner
        )
        foreach(tgt IN LISTS COVERAGE_TARGETS)
            if(TARGET ${tgt})
                target_compile_options(${tgt} PRIVATE --coverage)
                target_link_options(${tgt} PRIVATE --coverage)
            endif()
        endforeach()

        add_custom_target(coverage_tests
            COMMAND ctest --test-dir ${CMAKE_BINARY_DIR} --output-on-failure
            COMMAND lcov --capture --directory ${CMAKE_BINARY_DIR} --output-file ${REPORTS_DIR}/coverage/tests/coverage.info
            COMMAND genhtml ${REPORTS_DIR}/coverage/tests/coverage.info --output-directory ${REPORTS_DIR}/coverage/tests/html
            DEPENDS TasqlyTestsRunner
            COMMENT "Running Tests and generating LCOV HTML report reports/coverage/tests/"
        )

        add_custom_target(coverage_benchmarks
            COMMAND ${CMAKE_BINARY_DIR}/TasqlyBenchmarksRunner
            COMMAND lcov --capture --directory ${CMAKE_BINARY_DIR} --output-file ${REPORTS_DIR}/coverage/benchmarks/coverage.info
            COMMAND genhtml ${REPORTS_DIR}/coverage/benchmarks/coverage.info --output-directory ${REPORTS_DIR}/coverage/benchmarks/html
            DEPENDS TasqlyBenchmarksRunner
            COMMENT "Running Benchmarks and generating LCOV HTML report reports/coverage/benchmarks/"
        )
    endif()
endif()
