#include "AgentEngine/Engine.hpp"
// #include "AgentEngine/pch.hpp"
// #include "AgentEngine/Stopwatch.hpp"
// #include "AgentEngine/Timer.hpp"
#include "AgentEngine/Input.hpp"
#include "AgentEngine/Window.hpp"
// #include "AgentEngine/Stopwatch.hpp"
// #include <print>
#include <dlfcn.h>
#include <print>

int main()
{
    ae::Engine engine;
    engine.Initialize();
    ae::Window window(1000,100,"Agent Engine");
    engine.Run();
    engine.Shutdown();

    return 0;
}
