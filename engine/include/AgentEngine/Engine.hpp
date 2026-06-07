#pragma once
#include "AgentEngine/Timer.hpp"
#include "AgentEngine/Renderer.hpp"
namespace ae
{
    class Engine
    {
    public:
        Engine();
        void Initialize();
        void Run();
        void Shutdown();
    private:
        SDL_Window* m_window;
        ae::Renderer m_renderer;
        ae::Timer m_timer;
    };
}
