file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(AgentEngineTests
    ${TEST_SOURCES}
)

target_link_libraries(AgentEngineTests
    PRIVATE
        AgentEngine
)

target_compile_features(AgentEngineTests
    PRIVATE
        cxx_std_26
)

add_test(NAME AgentEngineTests COMMAND AgentEngineTests)
