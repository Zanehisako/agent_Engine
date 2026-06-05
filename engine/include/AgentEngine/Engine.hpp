#pragma once
#include "AgentEngine/Timer.hpp"
namespace ae
{
    class Engine
    {
    public:
        void Initialize();
        void Run();
        void Shutdown();
    private:
        ae::Timer timer;
    };
}
