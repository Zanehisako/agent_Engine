#include "AgentEngine/Engine.hpp"
// #include "AgentEngine/pch.hpp"
#include "AgentEngine/Stopwatch.hpp"
#include "AgentEngine/Timer.hpp"
// #include "AgentEngine/Stopwatch.hpp"
#include <print>
#include <dlfcn.h>
#include <print>

int main()
{
    ae::Engine engine;
    ae::Timer timer;
    ae::Stopwatch stopwatch;
    std::println("time at the start {}",timer.GetTotalTime());
    std::println("stopwatch time at the start {}",stopwatch.GetElapsedTime());
    stopwatch.Start();

    engine.Initialize();
    timer.update();
    std::println("time at the end {}",timer.GetTotalTime());
    stopwatch.Stop();
    std::println("stopwatch time at the end {}",stopwatch.GetElapsedTime());
    engine.Shutdown();

    return 0;
}
