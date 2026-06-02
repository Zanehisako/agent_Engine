#pragma once

#include <memory>
#include <string>

namespace agent_engine {

class EngineHost;

class AgentEndpointServer {
public:
    explicit AgentEndpointServer(EngineHost& host);
    ~AgentEndpointServer();

    AgentEndpointServer(const AgentEndpointServer&) = delete;
    AgentEndpointServer& operator=(const AgentEndpointServer&) = delete;

    bool start(std::string address = "127.0.0.1", int preferred_port = 7391);
    void stop();

    [[nodiscard]] int port() const;
    [[nodiscard]] std::string address() const;
    [[nodiscard]] std::string base_url() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace agent_engine
