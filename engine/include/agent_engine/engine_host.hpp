#pragma once

#include <agent_engine/module_api.hpp>
#include <nlohmann/json_fwd.hpp>

#include <atomic>
#include <chrono>
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace agent_engine {

struct Rgba {
    float r = 0.08F;
    float g = 0.09F;
    float b = 0.11F;
    float a = 1.0F;
};

struct ReloadResult {
    bool ok = false;
    std::string status;
    std::string detail;
};

class EngineHost {
public:
    EngineHost();
    ~EngineHost();

    EngineHost(const EngineHost&) = delete;
    EngineHost& operator=(const EngineHost&) = delete;

    void tick(double dt_seconds);

    ReloadResult load_module(
        const std::filesystem::path& module_path,
        bool preserve_state,
        std::string reason
    );

    ReloadResult rebuild_and_reload(
        const std::filesystem::path& build_dir,
        std::string target,
        const std::filesystem::path& module_path
    );

    void request_reload(std::string reason);
    bool consume_reload_request();
    void set_agent_endpoints(std::string http_url, std::string mcp_url);

    [[nodiscard]] Rgba clear_color() const;
    [[nodiscard]] nlohmann::json health_json() const;
    [[nodiscard]] nlohmann::json state_json() const;
    [[nodiscard]] nlohmann::json modules_json() const;
    [[nodiscard]] nlohmann::json diagnostics_json() const;

private:
    struct ModuleSlot;

    static void host_set_clear_color(void* user_data, AgentEngineColor color);
    static void host_log(void* user_data, const char* message);
    static double host_time_seconds(void* user_data);

    void set_clear_color(AgentEngineColor color);
    void log(std::string message);
    [[nodiscard]] std::string serialize_current_module();
    void unload_current_module();
    [[nodiscard]] std::filesystem::path copied_module_path(const std::filesystem::path& module_path);

    mutable std::mutex mutex_;
    std::unique_ptr<ModuleSlot> module_;
    AgentEngineModuleHost module_host_;
    std::chrono::steady_clock::time_point started_at_;
    Rgba clear_color_;
    std::uint64_t frame_ = 0;
    std::uint64_t module_generation_ = 0;
    std::string active_module_name_ = "none";
    std::string active_module_path_;
    std::string active_module_description_;
    std::string last_reload_status_ = "not_loaded";
    std::string last_reload_reason_;
    std::string last_reload_detail_;
    std::string last_build_command_;
    std::string last_build_output_;
    std::string http_endpoint_ = "http://127.0.0.1:7391";
    std::string mcp_endpoint_ = "http://127.0.0.1:7391/mcp";
    std::vector<std::string> logs_;
    std::atomic_bool reload_requested_ = false;
    std::string reload_reason_;
};

} // namespace agent_engine
