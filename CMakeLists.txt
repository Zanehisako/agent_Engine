cmake_minimum_required(VERSION 4.2)

project(AgentEngine VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # For IDE support

add_subdirectory(engine)
add_subdirectory(runtime)
add_subdirectory(game)
add_subdirectory(external/sdl)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
