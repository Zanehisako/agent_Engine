#include <agent_engine/agent_endpoint_server.hpp>
#include <agent_engine/engine_host.hpp>

#include <SDL3/SDL.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>

namespace {

std::optional<std::filesystem::file_time_type> latest_write_time(const std::filesystem::path& root)
{
    if (root.empty() || !std::filesystem::exists(root)) {
        return std::nullopt;
    }

    std::optional<std::filesystem::file_time_type> latest;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(root)) {
        if (!entry.is_regular_file()) {
            continue;
        }
        const auto extension = entry.path().extension().string();
        if (extension != ".cpp" && extension != ".hpp" && extension != ".h") {
            continue;
        }
        const auto timestamp = entry.last_write_time();
        if (!latest.has_value() || timestamp > latest.value()) {
            latest = timestamp;
        }
    }
    return latest;
}

std::uint8_t to_channel(float value)
{
    const float clamped = std::clamp(value, 0.0F, 1.0F);
    return static_cast<std::uint8_t>(clamped * 255.0F);
}

} // namespace

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    agent_engine::EngineHost host;

#if AGENT_ENGINE_ENABLE_AGENT_ENDPOINTS
    agent_engine::AgentEndpointServer endpoints(host);
    if (endpoints.start()) {
        std::cout << "Agent endpoints: " << endpoints.base_url() << '\n';
        std::cout << "MCP endpoint: " << endpoints.base_url() << "/mcp\n";
    } else {
        std::cerr << "Failed to start agent endpoints on 127.0.0.1.\n";
    }
#endif

#ifdef AGENT_ENGINE_SAMPLE_MODULE_PATH
    std::filesystem::path module_path = AGENT_ENGINE_SAMPLE_MODULE_PATH;
    host.load_module(module_path, false, "startup");
#else
    std::filesystem::path module_path;
#endif

#ifdef AGENT_ENGINE_SAMPLE_MODULE_SOURCE_DIR
    std::filesystem::path watch_dir = AGENT_ENGINE_SAMPLE_MODULE_SOURCE_DIR;
    auto last_source_time = latest_write_time(watch_dir);
#else
    std::filesystem::path watch_dir;
    std::optional<std::filesystem::file_time_type> last_source_time;
#endif

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Agent Engine",
        960,
        540,
        SDL_WINDOW_RESIZABLE
    );
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    auto previous = std::chrono::steady_clock::now();
    auto next_watch_check = previous;
    bool running = true;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        const auto now = std::chrono::steady_clock::now();
        const double dt_seconds = std::chrono::duration<double>(now - previous).count();
        previous = now;

        bool should_reload = host.consume_reload_request();

        if (!watch_dir.empty() && now >= next_watch_check) {
            next_watch_check = now + std::chrono::milliseconds(250);
            auto current_source_time = latest_write_time(watch_dir);
            if (current_source_time.has_value()
                && last_source_time.has_value()
                && current_source_time.value() > last_source_time.value()) {
                should_reload = true;
            }
            if (current_source_time.has_value()) {
                last_source_time = current_source_time;
            }
        }

#if AGENT_ENGINE_ENABLE_HOT_RELOAD
        if (should_reload && !module_path.empty()) {
#ifdef AGENT_ENGINE_BUILD_DIR
            host.rebuild_and_reload(
                AGENT_ENGINE_BUILD_DIR,
                AGENT_ENGINE_RELOAD_TARGET,
                module_path
            );
#else
            host.load_module(module_path, true, "manual_reload");
#endif
        }
#else
        (void)should_reload;
#endif

        host.tick(dt_seconds);

        const auto color = host.clear_color();
        SDL_SetRenderDrawColor(
            renderer,
            to_channel(color.r),
            to_channel(color.g),
            to_channel(color.b),
            to_channel(color.a)
        );
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
