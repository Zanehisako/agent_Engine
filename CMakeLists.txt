cmake_minimum_required(VERSION 3.31)

project(
    AgentEngine
    VERSION 0.1.0
    DESCRIPTION "AI-native game engine"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(AGENT_ENGINE_BUILD_EDITOR "Build editor" ON)
option(AGENT_ENGINE_BUILD_EXAMPLES "Build examples" ON)
option(AGENT_ENGINE_BUILD_TESTS "Build tests" ON)
option(AGENT_ENGINE_ENABLE_HOT_RELOAD "Enable compiled module hot reload" ON)
option(AGENT_ENGINE_ENABLE_AGENT_ENDPOINTS "Enable HTTP and MCP endpoints for agents" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)

FetchContent_Declare(
    cpp_httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.26.0
)

if(AGENT_ENGINE_BUILD_EDITOR)
    set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
    set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.24
    )
    FetchContent_MakeAvailable(SDL3)
endif()

FetchContent_MakeAvailable(nlohmann_json cpp_httplib)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
    )
endif()

add_subdirectory(engine)

if(AGENT_ENGINE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

add_subdirectory(runtime)

if(AGENT_ENGINE_BUILD_EDITOR)
    add_subdirectory(editor)
endif()

if(AGENT_ENGINE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
