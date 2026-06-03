file(GLOB_RECURSE RUNTIME_SOURCES
    CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(Runtime
    ${RUNTIME_SOURCES}
)

target_link_libraries(
    Runtime
    PRIVATE
        AgentEngine
)
