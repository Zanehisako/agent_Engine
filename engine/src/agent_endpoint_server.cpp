#include <agent_engine/agent_endpoint_server.hpp>

#include <agent_engine/engine_host.hpp>

#include <httplib.h>
#include <nlohmann/json.hpp>

#include <format>
#include <stdexcept>
#include <thread>
#include <utility>

namespace agent_engine {
namespace {

constexpr const char* kMcpProtocolVersion = "2025-06-18";

void set_json(httplib::Response& response, const nlohmann::json& body)
{
    response.set_content(body.dump(2), "application/json");
}

nlohmann::json json_rpc_result(const nlohmann::json& id, const nlohmann::json& result)
{
    return {
        {"jsonrpc", "2.0"},
        {"id", id},
        {"result", result},
    };
}

nlohmann::json json_rpc_error(const nlohmann::json& id, int code, std::string message)
{
    return {
        {"jsonrpc", "2.0"},
        {"id", id},
        {"error", {
            {"code", code},
            {"message", std::move(message)},
        }},
    };
}

nlohmann::json mcp_text_result(const nlohmann::json& value)
{
    return {
        {"content", {{
            {"type", "text"},
            {"text", value.dump(2)},
        }}},
        {"structuredContent", value},
        {"isError", false},
    };
}

nlohmann::json tools_list()
{
    const nlohmann::json empty_object_schema = {
        {"type", "object"},
        {"properties", nlohmann::json::object()},
        {"additionalProperties", false},
    };

    return {
        {"tools", {
            {
                {"name", "engine.get_state"},
                {"description", "Return the current engine state snapshot."},
                {"inputSchema", empty_object_schema},
            },
            {
                {"name", "engine.request_reload"},
                {"description", "Ask the running editor/runtime to rebuild and hot reload its active module."},
                {"inputSchema", {
                    {"type", "object"},
                    {"properties", {
                        {"reason", {
                            {"type", "string"},
                            {"description", "Optional reason for diagnostics."},
                        }},
                    }},
                    {"additionalProperties", false},
                }},
            },
            {
                {"name", "engine.get_diagnostics"},
                {"description", "Return reload, build, and runtime diagnostics."},
                {"inputSchema", empty_object_schema},
            },
        }},
    };
}

nlohmann::json resources_list()
{
    return {
        {"resources", {
            {
                {"uri", "agent-engine://state"},
                {"name", "Engine State"},
                {"mimeType", "application/json"},
                {"description", "Current engine frame, module, and visual state."},
            },
            {
                {"uri", "agent-engine://modules"},
                {"name", "Engine Modules"},
                {"mimeType", "application/json"},
                {"description", "Loaded module metadata and ABI version."},
            },
            {
                {"uri", "agent-engine://diagnostics"},
                {"name", "Engine Diagnostics"},
                {"mimeType", "application/json"},
                {"description", "Hot reload and build diagnostics."},
            },
        }},
    };
}

nlohmann::json resource_read(EngineHost& host, const std::string& uri)
{
    nlohmann::json payload;
    if (uri == "agent-engine://state") {
        payload = host.state_json();
    } else if (uri == "agent-engine://modules") {
        payload = host.modules_json();
    } else if (uri == "agent-engine://diagnostics") {
        payload = host.diagnostics_json();
    } else {
        throw std::invalid_argument("Unknown resource URI.");
    }

    return {
        {"contents", {{
            {"uri", uri},
            {"mimeType", "application/json"},
            {"text", payload.dump(2)},
        }}},
    };
}

} // namespace

struct AgentEndpointServer::Impl {
    explicit Impl(EngineHost& host_in) : host(host_in) {}

    void install_routes()
    {
        server.Get("/health", [this](const httplib::Request&, httplib::Response& response) {
            set_json(response, host.health_json());
        });

        server.Get("/state", [this](const httplib::Request&, httplib::Response& response) {
            set_json(response, host.state_json());
        });

        server.Get("/modules", [this](const httplib::Request&, httplib::Response& response) {
            set_json(response, host.modules_json());
        });

        server.Get("/diagnostics", [this](const httplib::Request&, httplib::Response& response) {
            set_json(response, host.diagnostics_json());
        });

        server.Post("/reload", [this](const httplib::Request& request, httplib::Response& response) {
            std::string reason = "http";
            if (!request.body.empty()) {
                try {
                    const auto body = nlohmann::json::parse(request.body);
                    reason = body.value("reason", reason);
                } catch (const nlohmann::json::exception&) {
                    reason = "http";
                }
            }
            host.request_reload(reason);
            set_json(response, {
                {"ok", true},
                {"reloadPending", true},
                {"reason", reason},
            });
        });

        server.Get("/events", [this](const httplib::Request&, httplib::Response& response) {
            response.set_header("Cache-Control", "no-store");
            response.set_content(
                std::format("event: state\ndata: {}\n\n", host.state_json().dump()),
                "text/event-stream"
            );
        });

        server.Get("/mcp", [](const httplib::Request&, httplib::Response& response) {
            response.set_header("Cache-Control", "no-store");
            response.set_content(
                "event: endpoint\ndata: {\"endpoint\":\"/mcp\",\"transport\":\"streamable-http\"}\n\n",
                "text/event-stream"
            );
        });

        server.Post("/mcp", [this](const httplib::Request& request, httplib::Response& response) {
            handle_mcp_post(request, response);
        });
    }

    void handle_mcp_post(const httplib::Request& request, httplib::Response& response)
    {
        nlohmann::json message;
        try {
            message = nlohmann::json::parse(request.body);
        } catch (const nlohmann::json::exception&) {
            set_json(response, json_rpc_error(nullptr, -32700, "Parse error"));
            return;
        }

        if (!message.contains("id")) {
            response.status = 202;
            return;
        }

        const nlohmann::json id = message.at("id");
        const std::string method = message.value("method", "");
        const nlohmann::json params = message.value("params", nlohmann::json::object());

        try {
            if (method == "initialize") {
                set_json(response, json_rpc_result(id, {
                    {"protocolVersion", kMcpProtocolVersion},
                    {"capabilities", {
                        {"tools", {{"listChanged", true}}},
                        {"resources", {{"listChanged", true}}},
                    }},
                    {"serverInfo", {
                        {"name", "agent-engine"},
                        {"version", "0.1.0"},
                    }},
                }));
                return;
            }

            if (method == "ping") {
                set_json(response, json_rpc_result(id, nlohmann::json::object()));
                return;
            }

            if (method == "tools/list") {
                set_json(response, json_rpc_result(id, tools_list()));
                return;
            }

            if (method == "tools/call") {
                const std::string tool_name = params.value("name", "");
                if (tool_name == "engine.get_state") {
                    set_json(response, json_rpc_result(id, mcp_text_result(host.state_json())));
                    return;
                }
                if (tool_name == "engine.request_reload") {
                    const auto arguments = params.value("arguments", nlohmann::json::object());
                    host.request_reload(arguments.value("reason", "mcp"));
                    set_json(response, json_rpc_result(id, mcp_text_result({
                        {"ok", true},
                        {"reloadPending", true},
                    })));
                    return;
                }
                if (tool_name == "engine.get_diagnostics") {
                    set_json(response, json_rpc_result(id, mcp_text_result(host.diagnostics_json())));
                    return;
                }
                set_json(response, json_rpc_error(id, -32602, "Unknown tool."));
                return;
            }

            if (method == "resources/list") {
                set_json(response, json_rpc_result(id, resources_list()));
                return;
            }

            if (method == "resources/read") {
                const std::string uri = params.value("uri", "");
                set_json(response, json_rpc_result(id, resource_read(host, uri)));
                return;
            }

            set_json(response, json_rpc_error(id, -32601, "Method not found."));
        } catch (const std::exception& error) {
            set_json(response, json_rpc_error(id, -32603, error.what()));
        }
    }

    EngineHost& host;
    httplib::Server server;
    std::thread thread;
    std::string bind_address = "127.0.0.1";
    int bind_port = 0;
};

AgentEndpointServer::AgentEndpointServer(EngineHost& host)
    : impl_(std::make_unique<Impl>(host))
{
}

AgentEndpointServer::~AgentEndpointServer()
{
    stop();
}

bool AgentEndpointServer::start(std::string address, int preferred_port)
{
    impl_->bind_address = std::move(address);
    impl_->install_routes();

    for (int candidate = preferred_port; candidate < preferred_port + 10; ++candidate) {
        if (impl_->server.bind_to_port(impl_->bind_address, candidate)) {
            impl_->bind_port = candidate;
            impl_->host.set_agent_endpoints(base_url(), base_url() + "/mcp");
            impl_->thread = std::thread([this] {
                impl_->server.listen_after_bind();
            });
            return true;
        }
    }
    return false;
}

void AgentEndpointServer::stop()
{
    if (impl_ == nullptr) {
        return;
    }
    impl_->server.stop();
    if (impl_->thread.joinable()) {
        impl_->thread.join();
    }
}

int AgentEndpointServer::port() const
{
    return impl_->bind_port;
}

std::string AgentEndpointServer::address() const
{
    return impl_->bind_address;
}

std::string AgentEndpointServer::base_url() const
{
    return std::format("http://{}:{}", address(), port());
}

} // namespace agent_engine
