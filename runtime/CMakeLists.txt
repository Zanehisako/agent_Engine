file(GLOB_RECURSE RUNTIME_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(AgentRuntime
    ${RUNTIME_SOURCES}
)

target_link_libraries(AgentRuntime
    PRIVATE
        AgentEngine
)

target_compile_features(AgentRuntime
    PRIVATE
        cxx_std_26
)

if(TARGET SampleAgentModule)
    add_dependencies(AgentRuntime SampleAgentModule)
    target_compile_definitions(AgentRuntime
        PRIVATE
            AGENT_ENGINE_SAMPLE_MODULE_PATH="$<TARGET_FILE:SampleAgentModule>"
    )
endif()
