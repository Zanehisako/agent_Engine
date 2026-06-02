#include <agent_engine/engine_host.hpp>
#include <agent_engine/module_api.hpp>

#include <nlohmann/json.hpp>

#include <iostream>

namespace {

int expect(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        return 1;
    }
    return 0;
}

} // namespace

int main()
{
    agent_engine::EngineHost host;

    int failures = 0;
    failures += expect(AGENT_ENGINE_MODULE_ABI_VERSION == 1u, "module ABI version is stable");

    host.tick(0.016);
    auto state = host.state_json();
    failures += expect(state.at("engine") == "AgentEngine", "state exposes engine name");
    failures += expect(state.at("frame") == 1, "tick advances frame");
    failures += expect(state.at("module").at("loaded") == false, "module starts unloaded");

    host.request_reload("test");
    failures += expect(host.consume_reload_request(), "reload request is consumable");
    failures += expect(!host.consume_reload_request(), "reload request is consumed once");

    auto diagnostics = host.diagnostics_json();
    failures += expect(
        diagnostics.at("lastReloadStatus") == "requested",
        "diagnostics expose reload requests"
    );

    return failures == 0 ? 0 : 1;
}
