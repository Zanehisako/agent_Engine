file(GLOB_RECURSE ENGINE_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${ENGINE_SOURCES}
)

add_library(AgentEngine STATIC
    ${ENGINE_SOURCES}
)

target_include_directories(
    AgentEngine
    PUBLIC
        include
)

target_link_libraries(
    AgentEngine
    PUBLIC
        SDL3::SDL3
)

target_precompile_headers(
    AgentEngine
    PUBLIC
        include/AgentEngine/pch.hpp
)
