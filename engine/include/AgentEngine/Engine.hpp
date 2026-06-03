#pragma once
#include "AgentEngine/Timer.hpp"
namespace ae
{
    class Engine
    {
    public:
        void Initialize();
        void Update();
        void Shutdown();
    private:
        ae::Timer timer;
    };
}
