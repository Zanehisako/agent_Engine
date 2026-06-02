#include <agent_engine/agent_endpoint_server.hpp>
#include <agent_engine/engine_host.hpp>

#include <nlohmann/json.hpp>

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <string_view>
#include <thread>

namespace {

std::atomic_bool keep_running = true;

void stop_runtime(int)
{
    keep_running.store(false);
}

bool has_flag(int argc, char** argv, std::string_view flag)
{
    for (int index = 1; index < argc; ++index) {
        if (argv[index] == flag) {
            return true;
        }
    }
    return false;
}

} // namespace

int main(int argc, char** argv)
{
    agent_engine::EngineHost host;

#ifdef AGENT_ENGINE_SAMPLE_MODULE_PATH
    host.load_module(AGENT_ENGINE_SAMPLE_MODULE_PATH, false, "runtime_startup");
#endif

    if (has_flag(argc, argv, "--once")) {
        host.tick(0.0);
        std::cout << host.state_json().dump(2) << '\n';
        return 0;
    }

#if AGENT_ENGINE_ENABLE_AGENT_ENDPOINTS
    agent_engine::AgentEndpointServer endpoints(host);
    if (endpoints.start()) {
        std::cout << "Agent endpoints: " << endpoints.base_url() << '\n';
        std::cout << "MCP endpoint: " << endpoints.base_url() << "/mcp\n";
    } else {
        std::cerr << "Failed to start agent endpoints on 127.0.0.1.\n";
    }
#endif

    std::signal(SIGINT, stop_runtime);
    std::signal(SIGTERM, stop_runtime);

    auto previous = std::chrono::steady_clock::now();
    while (keep_running.load()) {
        const auto now = std::chrono::steady_clock::now();
        const double dt_seconds = std::chrono::duration<double>(now - previous).count();
        previous = now;

        if (host.consume_reload_request()) {
#ifdef AGENT_ENGINE_SAMPLE_MODULE_PATH
            host.load_module(AGENT_ENGINE_SAMPLE_MODULE_PATH, true, "runtime_reload");
#endif
        }

        host.tick(dt_seconds);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}
