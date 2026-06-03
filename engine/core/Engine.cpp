#include "AgentEngine/Engine.hpp"

namespace ae
{
    void Engine::Initialize() {
        SDL_Init(SDL_INIT_VIDEO);
        timer = Timer();
    }
    void Engine::Update() {
        bool IsRunning = true;
         SDL_Event Event;
         while (IsRunning) {
           while (SDL_PollEvent(&Event)) {
             if (Event.type == SDL_EVENT_QUIT) {
               IsRunning = false;
             }
             timer.update();
           }
         }
    }
    void Engine::Shutdown() {
        SDL_Quit();
    }
}
