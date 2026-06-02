file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

add_library(AgentEngine SHARED
    ${ENGINE_SOURCES}
)

target_include_directories(AgentEngine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(AgentEngine
    PUBLIC
        nlohmann_json::nlohmann_json
    PRIVATE
        httplib::httplib
)

if(APPLE)
    find_library(CFNETWORK_FRAMEWORK CFNetwork REQUIRED)
    target_link_libraries(AgentEngine
        PRIVATE
            ${CFNETWORK_FRAMEWORK}
    )
endif()

target_compile_features(AgentEngine
    PUBLIC
        cxx_std_26
)

target_compile_definitions(AgentEngine
    PUBLIC
        $<$<BOOL:${AGENT_ENGINE_ENABLE_HOT_RELOAD}>:AGENT_ENGINE_ENABLE_HOT_RELOAD=1>
        $<$<BOOL:${AGENT_ENGINE_ENABLE_AGENT_ENDPOINTS}>:AGENT_ENGINE_ENABLE_AGENT_ENDPOINTS=1>
)

target_precompile_headers(AgentEngine PRIVATE
    <atomic>
    <chrono>
    <filesystem>
    <functional>
    <memory>
    <mutex>
    <optional>
    <string>
    <string_view>
    <vector>
)
