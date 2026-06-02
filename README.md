# Agent Engine

AI-native C++26 engine foundation with hot-reloadable C++ modules and local agent endpoints.

## Build

```sh
cmake --preset dev
cmake --build --preset dev
ctest --preset dev
```

## Run

```sh
build/dev/bin/AgentEditor
```

The editor opens an SDL3 window and loads `SampleAgentModule`. Edits under `examples/sample_module/` trigger a rebuild of that module and reload it without restarting the editor.

For headless checks:

```sh
build/dev/bin/AgentRuntime --once
build/dev/bin/AgentRuntime
```

## Agent Endpoints

The runtime/editor bind to `127.0.0.1`, starting at port `7391`.

```sh
curl http://127.0.0.1:7391/state
curl -X POST http://127.0.0.1:7391/reload
curl http://127.0.0.1:7391/diagnostics
```

MCP Streamable HTTP is available at:

```txt
http://127.0.0.1:7391/mcp
```

Tools:

- `engine.get_state`
- `engine.request_reload`
- `engine.get_diagnostics`

Resources:

- `agent-engine://state`
- `agent-engine://modules`
- `agent-engine://diagnostics`
