#include <agent_engine/engine_host.hpp>

#include <nlohmann/json.hpp>

#include <array>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

#if defined(_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#include <sys/wait.h>
#endif

namespace agent_engine {
namespace {

struct DynamicLibrary {
    explicit DynamicLibrary(std::filesystem::path path_in) : path(std::move(path_in)) {}

    ~DynamicLibrary()
    {
        close();
    }

    DynamicLibrary(const DynamicLibrary&) = delete;
    DynamicLibrary& operator=(const DynamicLibrary&) = delete;

    bool open()
    {
#if defined(_WIN32)
        handle = LoadLibraryA(path.string().c_str());
#else
        handle = dlopen(path.string().c_str(), RTLD_NOW | RTLD_LOCAL);
#endif
        return handle != nullptr;
    }

    void* symbol(const char* name) const
    {
#if defined(_WIN32)
        return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(handle), name));
#else
        return dlsym(handle, name);
#endif
    }

    void close()
    {
        if (handle == nullptr) {
            return;
        }
#if defined(_WIN32)
        FreeLibrary(static_cast<HMODULE>(handle));
#else
        dlclose(handle);
#endif
        handle = nullptr;
    }

    std::string error() const
    {
#if defined(_WIN32)
        return "LoadLibrary/GetProcAddress failed";
#else
        const char* message = dlerror();
        return message == nullptr ? "dlopen/dlsym failed" : std::string(message);
#endif
    }

    std::filesystem::path path;
    void* handle = nullptr;
};

struct CommandResult {
    int exit_code = 1;
    std::string output;
};

std::string shell_quote(const std::filesystem::path& path)
{
    std::string input = path.string();
    std::string quoted = "'";
    for (const char character : input) {
        if (character == '\'') {
            quoted += "'\\''";
        } else {
            quoted += character;
        }
    }
    quoted += "'";
    return quoted;
}

CommandResult run_command(const std::string& command)
{
    CommandResult result;
#if defined(_WIN32)
    result.exit_code = std::system(command.c_str());
    result.output = "Command output capture is not implemented on Windows.";
#else
    std::array<char, 512> buffer {};
    FILE* pipe = popen(command.c_str(), "r");
    if (pipe == nullptr) {
        result.output = "Failed to start command.";
        return result;
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        result.output += buffer.data();
    }
    const int raw_status = pclose(pipe);
    if (WIFEXITED(raw_status)) {
        result.exit_code = WEXITSTATUS(raw_status);
    } else {
        result.exit_code = raw_status;
    }
#endif
    return result;
}

double seconds_since(std::chrono::steady_clock::time_point point)
{
    const auto elapsed = std::chrono::steady_clock::now() - point;
    return std::chrono::duration<double>(elapsed).count();
}

nlohmann::json color_json(const Rgba& color)
{
    return {
        {"r", color.r},
        {"g", color.g},
        {"b", color.b},
        {"a", color.a},
    };
}

} // namespace

struct EngineHost::ModuleSlot {
    std::unique_ptr<DynamicLibrary> library;
    AgentEngineModuleApi* api = nullptr;
    std::filesystem::path loaded_path;
    std::filesystem::path source_path;
};

EngineHost::EngineHost()
    : module_host_ {
        .abi_version = AGENT_ENGINE_MODULE_ABI_VERSION,
        .user_data = this,
        .set_clear_color = &EngineHost::host_set_clear_color,
        .log = &EngineHost::host_log,
        .time_seconds = &EngineHost::host_time_seconds,
    },
      started_at_(std::chrono::steady_clock::now())
{
}

EngineHost::~EngineHost()
{
    unload_current_module();
}

void EngineHost::tick(double dt_seconds)
{
    AgentEngineModuleApi* api = nullptr;
    {
        std::lock_guard lock(mutex_);
        ++frame_;
        if (module_ != nullptr) {
            api = module_->api;
        }
    }

    if (api != nullptr && api->on_update != nullptr) {
        api->on_update(&module_host_, dt_seconds);
    }
    if (api != nullptr && api->on_render != nullptr) {
        api->on_render(&module_host_);
    }
}

ReloadResult EngineHost::load_module(
    const std::filesystem::path& module_path,
    const bool preserve_state,
    std::string reason
)
{
    if (module_path.empty()) {
        std::lock_guard lock(mutex_);
        last_reload_status_ = "failed";
        last_reload_reason_ = std::move(reason);
        last_reload_detail_ = "No module path was provided.";
        return {.ok = false, .status = last_reload_status_, .detail = last_reload_detail_};
    }

    std::string preserved_state;
    if (preserve_state) {
        preserved_state = serialize_current_module();
    }

    std::filesystem::path load_path;
    try {
        load_path = copied_module_path(module_path);
    } catch (const std::exception& error) {
        std::lock_guard lock(mutex_);
        last_reload_status_ = "failed";
        last_reload_reason_ = std::move(reason);
        last_reload_detail_ = error.what();
        return {.ok = false, .status = last_reload_status_, .detail = last_reload_detail_};
    }

    auto library = std::make_unique<DynamicLibrary>(load_path);
    if (!library->open()) {
        std::lock_guard lock(mutex_);
        last_reload_status_ = "failed";
        last_reload_reason_ = std::move(reason);
        last_reload_detail_ = library->error();
        return {.ok = false, .status = last_reload_status_, .detail = last_reload_detail_};
    }

    auto* entrypoint = reinterpret_cast<AgentEngineGetModuleApi>(
        library->symbol(AGENT_ENGINE_MODULE_ENTRYPOINT)
    );
    if (entrypoint == nullptr) {
        std::lock_guard lock(mutex_);
        last_reload_status_ = "failed";
        last_reload_reason_ = std::move(reason);
        last_reload_detail_ = "Module is missing " AGENT_ENGINE_MODULE_ENTRYPOINT ".";
        return {.ok = false, .status = last_reload_status_, .detail = last_reload_detail_};
    }

    AgentEngineModuleApi* api = entrypoint();
    if (api == nullptr || api->abi_version != AGENT_ENGINE_MODULE_ABI_VERSION) {
        std::lock_guard lock(mutex_);
        last_reload_status_ = "failed";
        last_reload_reason_ = std::move(reason);
        last_reload_detail_ = "Module ABI version does not match the engine.";
        return {.ok = false, .status = last_reload_status_, .detail = last_reload_detail_};
    }

    auto next_module = std::make_unique<ModuleSlot>();
    next_module->library = std::move(library);
    next_module->api = api;
    next_module->loaded_path = load_path;
    next_module->source_path = module_path;

    if (api->on_load != nullptr) {
        api->on_load(&module_host_);
    }
    if (preserve_state && !preserved_state.empty() && api->deserialize_state != nullptr) {
        api->deserialize_state(&module_host_, preserved_state.c_str());
    }

    unload_current_module();

    std::lock_guard lock(mutex_);
    module_ = std::move(next_module);
    ++module_generation_;
    active_module_name_ = api->name == nullptr ? "unnamed" : api->name;
    active_module_description_ = api->description == nullptr ? "" : api->description;
    active_module_path_ = module_path.string();
    last_reload_status_ = "loaded";
    last_reload_reason_ = std::move(reason);
    last_reload_detail_ = std::format("Loaded {}", active_module_name_);
    logs_.push_back(last_reload_detail_);
    if (logs_.size() > 128) {
        logs_.erase(logs_.begin());
    }
    return {.ok = true, .status = last_reload_status_, .detail = last_reload_detail_};
}

ReloadResult EngineHost::rebuild_and_reload(
    const std::filesystem::path& build_dir,
    std::string target,
    const std::filesystem::path& module_path
)
{
    const std::string command = std::format(
        "cmake --build {} --target {} 2>&1",
        shell_quote(build_dir),
        target
    );

    {
        std::lock_guard lock(mutex_);
        last_reload_status_ = "building";
        last_reload_reason_ = "hot_reload";
        last_reload_detail_ = "Building reloadable module.";
        last_build_command_ = command;
        last_build_output_.clear();
    }

    const CommandResult command_result = run_command(command);
    {
        std::lock_guard lock(mutex_);
        last_build_output_ = command_result.output;
    }

    if (command_result.exit_code != 0) {
        std::lock_guard lock(mutex_);
        last_reload_status_ = "build_failed";
        last_reload_reason_ = "hot_reload";
        last_reload_detail_ = std::format("Build failed with exit code {}.", command_result.exit_code);
        return {.ok = false, .status = last_reload_status_, .detail = last_reload_detail_};
    }

    return load_module(module_path, true, "hot_reload");
}

void EngineHost::request_reload(std::string reason)
{
    {
        std::lock_guard lock(mutex_);
        reload_reason_ = std::move(reason);
        last_reload_status_ = "requested";
        last_reload_reason_ = reload_reason_;
        last_reload_detail_ = "Reload requested by agent endpoint.";
    }
    reload_requested_.store(true);
}

bool EngineHost::consume_reload_request()
{
    return reload_requested_.exchange(false);
}

void EngineHost::set_agent_endpoints(std::string http_url, std::string mcp_url)
{
    std::lock_guard lock(mutex_);
    http_endpoint_ = std::move(http_url);
    mcp_endpoint_ = std::move(mcp_url);
}

Rgba EngineHost::clear_color() const
{
    std::lock_guard lock(mutex_);
    return clear_color_;
}

nlohmann::json EngineHost::health_json() const
{
    return {
        {"ok", true},
        {"engine", "AgentEngine"},
        {"version", "0.1.0"},
        {"uptimeSeconds", seconds_since(started_at_)},
    };
}

nlohmann::json EngineHost::state_json() const
{
    std::lock_guard lock(mutex_);
    return {
        {"engine", "AgentEngine"},
        {"version", "0.1.0"},
        {"frame", frame_},
        {"uptimeSeconds", seconds_since(started_at_)},
        {"clearColor", color_json(clear_color_)},
        {"hotReload", {
            {"enabled", true},
            {"reloadPending", reload_requested_.load()},
            {"lastStatus", last_reload_status_},
            {"lastReason", last_reload_reason_},
            {"lastDetail", last_reload_detail_},
        }},
        {"module", {
            {"loaded", module_ != nullptr},
            {"name", active_module_name_},
            {"description", active_module_description_},
            {"path", active_module_path_},
            {"generation", module_generation_},
            {"abiVersion", AGENT_ENGINE_MODULE_ABI_VERSION},
        }},
        {"agentEndpoints", {
            {"http", http_endpoint_},
            {"mcp", mcp_endpoint_},
        }},
    };
}

nlohmann::json EngineHost::modules_json() const
{
    std::lock_guard lock(mutex_);
    return {
        {"active", {
            {"loaded", module_ != nullptr},
            {"name", active_module_name_},
            {"description", active_module_description_},
            {"path", active_module_path_},
            {"generation", module_generation_},
            {"abiVersion", AGENT_ENGINE_MODULE_ABI_VERSION},
        }},
    };
}

nlohmann::json EngineHost::diagnostics_json() const
{
    std::lock_guard lock(mutex_);
    return {
        {"lastReloadStatus", last_reload_status_},
        {"lastReloadReason", last_reload_reason_},
        {"lastReloadDetail", last_reload_detail_},
        {"lastBuildCommand", last_build_command_},
        {"lastBuildOutput", last_build_output_},
        {"logs", logs_},
    };
}

void EngineHost::host_set_clear_color(void* user_data, AgentEngineColor color)
{
    static_cast<EngineHost*>(user_data)->set_clear_color(color);
}

void EngineHost::host_log(void* user_data, const char* message)
{
    static_cast<EngineHost*>(user_data)->log(message == nullptr ? "" : message);
}

double EngineHost::host_time_seconds(void* user_data)
{
    return seconds_since(static_cast<EngineHost*>(user_data)->started_at_);
}

void EngineHost::set_clear_color(AgentEngineColor color)
{
    std::lock_guard lock(mutex_);
    clear_color_ = {
        .r = color.r,
        .g = color.g,
        .b = color.b,
        .a = color.a,
    };
}

void EngineHost::log(std::string message)
{
    std::lock_guard lock(mutex_);
    logs_.push_back(std::move(message));
    if (logs_.size() > 128) {
        logs_.erase(logs_.begin());
    }
}

std::string EngineHost::serialize_current_module()
{
    AgentEngineModuleApi* api = nullptr;
    {
        std::lock_guard lock(mutex_);
        if (module_ == nullptr) {
            return {};
        }
        api = module_->api;
    }

    if (api == nullptr || api->serialize_state == nullptr) {
        return {};
    }
    const char* state = api->serialize_state(&module_host_);
    return state == nullptr ? std::string {} : std::string(state);
}

void EngineHost::unload_current_module()
{
    std::unique_ptr<ModuleSlot> old_module;
    {
        std::lock_guard lock(mutex_);
        old_module = std::move(module_);
    }

    if (old_module != nullptr && old_module->api != nullptr && old_module->api->on_unload != nullptr) {
        old_module->api->on_unload(&module_host_);
    }
}

std::filesystem::path EngineHost::copied_module_path(const std::filesystem::path& module_path)
{
    if (!std::filesystem::exists(module_path)) {
        throw std::runtime_error(std::format("Module does not exist: {}", module_path.string()));
    }

    const auto output_dir = std::filesystem::temp_directory_path() / "agent_engine_hot_reload";
    std::filesystem::create_directories(output_dir);
    const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto destination = output_dir / std::format(
        "{}_{}{}",
        module_path.stem().string(),
        stamp,
        module_path.extension().string()
    );
    std::filesystem::copy_file(module_path, destination, std::filesystem::copy_options::overwrite_existing);
    return destination;
}

} // namespace agent_engine
