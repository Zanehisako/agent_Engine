file(GLOB_RECURSE EDITOR_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(AgentEditor
    ${EDITOR_SOURCES}
)

target_link_libraries(AgentEditor
    PRIVATE
        AgentEngine
        SDL3::SDL3
)

target_compile_features(AgentEditor
    PRIVATE
        cxx_std_26
)

if(TARGET SampleAgentModule)
    add_dependencies(AgentEditor SampleAgentModule)
    target_compile_definitions(AgentEditor
        PRIVATE
            AGENT_ENGINE_SAMPLE_MODULE_PATH="$<TARGET_FILE:SampleAgentModule>"
            AGENT_ENGINE_SAMPLE_MODULE_SOURCE_DIR="${CMAKE_SOURCE_DIR}/examples/sample_module"
            AGENT_ENGINE_BUILD_DIR="${CMAKE_BINARY_DIR}"
            AGENT_ENGINE_RELOAD_TARGET="SampleAgentModule"
    )
endif()
