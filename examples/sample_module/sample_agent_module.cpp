#include <agent_engine/module_api.hpp>

#include <cmath>
#include <format>
#include <string>

namespace {

constexpr AgentEngineColor kEditorColor {
    .r = 0.10F,
    .g = 0.20F,
    .b = 0.34F,
    .a = 1.0F,
};

std::string serialized_state = R"({"sample":"agent-module"})";

void on_load(AgentEngineModuleHost* host)
{
    host->log(host->user_data, "SampleAgentModule loaded.");
    host->set_clear_color(host->user_data, kEditorColor);
}

void on_unload(AgentEngineModuleHost* host)
{
    host->log(host->user_data, "SampleAgentModule unloaded.");
}

void on_update(AgentEngineModuleHost* host, double)
{
    const double t = host->time_seconds(host->user_data);
    const float pulse = static_cast<float>((std::sin(t * 1.5) + 1.0) * 0.5);
    AgentEngineColor color = kEditorColor;
    color.g = 0.16F + pulse * 0.18F;
    host->set_clear_color(host->user_data, color);
}

void on_render(AgentEngineModuleHost*) {}

const char* serialize_state(AgentEngineModuleHost*)
{
    return serialized_state.c_str();
}

void deserialize_state(AgentEngineModuleHost*, const char* state_json)
{
    if (state_json != nullptr) {
        serialized_state = state_json;
    }
}

AgentEngineModuleApi api {
    .abi_version = AGENT_ENGINE_MODULE_ABI_VERSION,
    .name = "SampleAgentModule",
    .description = "Minimal reloadable C++ module that controls the editor clear color.",
    .on_load = &on_load,
    .on_unload = &on_unload,
    .on_update = &on_update,
    .on_render = &on_render,
    .serialize_state = &serialize_state,
    .deserialize_state = &deserialize_state,
};

} // namespace

AGENT_ENGINE_MODULE_EXPORT AgentEngineModuleApi* agent_engine_get_module_api()
{
    return &api;
}
