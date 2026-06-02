#pragma once

#include <cstdint>

#define AGENT_ENGINE_MODULE_ABI_VERSION 1u
#define AGENT_ENGINE_MODULE_ENTRYPOINT "agent_engine_get_module_api"

#if defined(_WIN32)
#define AGENT_ENGINE_MODULE_EXPORT extern "C" __declspec(dllexport)
#else
#define AGENT_ENGINE_MODULE_EXPORT extern "C" __attribute__((visibility("default")))
#endif

struct AgentEngineColor {
    float r;
    float g;
    float b;
    float a;
};

struct AgentEngineModuleHost {
    std::uint32_t abi_version;
    void* user_data;
    void (*set_clear_color)(void* user_data, AgentEngineColor color);
    void (*log)(void* user_data, const char* message);
    double (*time_seconds)(void* user_data);
};

struct AgentEngineModuleApi {
    std::uint32_t abi_version;
    const char* name;
    const char* description;
    void (*on_load)(AgentEngineModuleHost* host);
    void (*on_unload)(AgentEngineModuleHost* host);
    void (*on_update)(AgentEngineModuleHost* host, double dt_seconds);
    void (*on_render)(AgentEngineModuleHost* host);
    const char* (*serialize_state)(AgentEngineModuleHost* host);
    void (*deserialize_state)(AgentEngineModuleHost* host, const char* state_json);
};

using AgentEngineGetModuleApi = AgentEngineModuleApi* (*)();
