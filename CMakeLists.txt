# CMakeLists.txt - DAG RPG Attribute System Build Configuration
cmake_minimum_required(VERSION 3.16)
project(DAG_RPG_System VERSION 1.0.0 LANGUAGES CXX)

# ═══════════════════════════════════════════════════════════════
# C++ Standard Configuration
# ═══════════════════════════════════════════════════════════════
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable compile commands for IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ═══════════════════════════════════════════════════════════════
# Output Directory Configuration
# ═══════════════════════════════════════════════════════════════
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ═══════════════════════════════════════════════════════════════
# Platform-Specific Configuration
# ═══════════════════════════════════════════════════════════════
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)  # Windows 7+
    set(PLATFORM_LIBS ws2_32)
else()
    set(PLATFORM_LIBS pthread)
endif()

# ═══════════════════════════════════════════════════════════════
# Fetch nlohmann/json
# ═══════════════════════════════════════════════════════════════
include(FetchContent)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# ═══════════════════════════════════════════════════════════════
# Include Directories
# ═══════════════════════════════════════════════════════════════
include_directories(${CMAKE_SOURCE_DIR}/include)

# ═══════════════════════════════════════════════════════════════
# Main Executable
# ═══════════════════════════════════════════════════════════════
add_executable(dag_rpg_system
    src/main.cpp
)

target_link_libraries(dag_rpg_system PRIVATE
    nlohmann_json::nlohmann_json
    ${PLATFORM_LIBS}
)

# ═══════════════════════════════════════════════════════════════
# Compiler Warnings
# ═══════════════════════════════════════════════════════════════
if(MSVC)
    target_compile_options(dag_rpg_system PRIVATE /W4 /WX-)
else()
    target_compile_options(dag_rpg_system PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ═══════════════════════════════════════════════════════════════
# Copy HTML Visualizer to Build Directory
# ═══════════════════════════════════════════════════════════════
configure_file(
    ${CMAKE_SOURCE_DIR}/index.html
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/index.html
    COPYONLY
)

# ═══════════════════════════════════════════════════════════════
# Installation
# ═══════════════════════════════════════════════════════════════
install(TARGETS dag_rpg_system DESTINATION bin)
install(FILES index.html DESTINATION bin)

# ═══════════════════════════════════════════════════════════════
# Print Configuration Summary
# ═══════════════════════════════════════════════════════════════
message(STATUS "")
message(STATUS "╔═══════════════════════════════════════════════════════════╗")
message(STATUS "║         DAG RPG Attribute System - Build Config           ║")
message(STATUS "╠═══════════════════════════════════════════════════════════╣")
message(STATUS "║  C++ Standard:    C++${CMAKE_CXX_STANDARD}")
message(STATUS "║  Build Type:      ${CMAKE_BUILD_TYPE}")
message(STATUS "║  Output Dir:      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "╚═══════════════════════════════════════════════════════════╝")
message(STATUS "")
