#include "AgentEngine/Engine.hpp"
// #include "AgentEngine/pch.hpp"
// #include "AgentEngine/Stopwatch.hpp"
// #include "AgentEngine/Timer.hpp"
#include "AgentEngine/Window.hpp"
// #include "AgentEngine/Stopwatch.hpp"
// #include <print>
#include <dlfcn.h>

int main()
{
    ae::Engine engine;
    engine.Initialize();
    ae::Window window(1000,100,"Agent Engine");
    engine.Update();
    engine.Shutdown();

    return 0;
}
