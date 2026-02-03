# CMakeLists.txt for tests
cmake_minimum_required(VERSION 3.16)

# ═══════════════════════════════════════════════════════════════
# Test Executable
# ═══════════════════════════════════════════════════════════════
add_executable(dag_rpg_tests
    test_main.cpp
    test_StatNode.cpp
    test_Modifier.cpp
    test_Producer.cpp
    # TODO: Fix API mismatches in these tests:
    # test_GraphManager.cpp - needs CreateBaseStat/CreateDerivedStat instead of CreateNode
    # test_HistoryNode.cpp - needs HistoryStatNode instead of HistoryNode
    # test_GameplayTag.cpp - needs GetName() instead of ToString(), no IsEmpty()/GetTagCount()
)

# ═══════════════════════════════════════════════════════════════
# Link Libraries
# ═══════════════════════════════════════════════════════════════
target_link_libraries(dag_rpg_tests PRIVATE
    gtest
    gtest_main
    nlohmann_json::nlohmann_json
    ${PLATFORM_LIBS}
)

# ═══════════════════════════════════════════════════════════════
# Include Directories
# ═══════════════════════════════════════════════════════════════
target_include_directories(dag_rpg_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# ═══════════════════════════════════════════════════════════════
# Compiler Options
# ═══════════════════════════════════════════════════════════════
if(MSVC)
    target_compile_options(dag_rpg_tests PRIVATE /W4)
    if(ENABLE_COVERAGE)
        message(STATUS "Code coverage on MSVC: Use OpenCppCoverage")
    endif()
else()
    target_compile_options(dag_rpg_tests PRIVATE -Wall -Wextra -Wpedantic)
    if(ENABLE_COVERAGE)
        target_compile_options(dag_rpg_tests PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_libraries(dag_rpg_tests PRIVATE --coverage)
    endif()
endif()

# ═══════════════════════════════════════════════════════════════
# Register Tests with CTest
# ═══════════════════════════════════════════════════════════════
gtest_discover_tests(dag_rpg_tests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

# ═══════════════════════════════════════════════════════════════
# Coverage Target (Linux/GCC/Clang only)
# ═══════════════════════════════════════════════════════════════
if(ENABLE_COVERAGE AND NOT MSVC)
    # Find lcov
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)

    if(LCOV_PATH AND GENHTML_PATH)
        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
            COMMAND ${LCOV_PATH} --directory . --zerocounters
            COMMAND $<TARGET_FILE:dag_rpg_tests>
            COMMAND ${LCOV_PATH} --directory . --capture --output-file ${CMAKE_BINARY_DIR}/coverage/coverage.info
            COMMAND ${LCOV_PATH} --remove ${CMAKE_BINARY_DIR}/coverage/coverage.info '/usr/*' '*/googletest/*' '*/tests/*' --output-file ${CMAKE_BINARY_DIR}/coverage/coverage_filtered.info
            COMMAND ${GENHTML_PATH} ${CMAKE_BINARY_DIR}/coverage/coverage_filtered.info --output-directory ${CMAKE_BINARY_DIR}/coverage
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating code coverage report"
        )

        message(STATUS "Code coverage target 'coverage' available")
    else()
        message(WARNING "lcov or genhtml not found. Coverage target not available.")
    endif()
endif()
