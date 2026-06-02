add_library(SampleAgentModule MODULE
    sample_agent_module.cpp
)

target_include_directories(SampleAgentModule
    PRIVATE
        ${PROJECT_SOURCE_DIR}/engine/include
)

target_compile_features(SampleAgentModule
    PRIVATE
        cxx_std_26
)

set_target_properties(SampleAgentModule PROPERTIES
    PREFIX ""
    OUTPUT_NAME "sample_agent_module"
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/modules
)
