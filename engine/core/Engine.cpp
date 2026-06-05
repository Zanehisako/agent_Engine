#include "AgentEngine/Engine.hpp"
#include "AgentEngine/Input.hpp"
#include <print>

#define SDL_EVENT_LIST(X)                    \
    X(SDL_EVENT_QUIT)                        \
    X(SDL_EVENT_KEY_DOWN)                    \
    X(SDL_EVENT_KEY_UP)                      \
    X(SDL_EVENT_MOUSE_BUTTON_DOWN)           \
    X(SDL_EVENT_MOUSE_BUTTON_UP)             \
    X(SDL_EVENT_MOUSE_MOTION)                \
    X(SDL_EVENT_MOUSE_WHEEL)                 \
    X(SDL_EVENT_WINDOW_RESIZED)              \
    X(SDL_EVENT_WINDOW_CLOSE_REQUESTED)

const char* GetEventName(Uint32 type)
{
    switch (type)
    {
#define X(event) case event: return #event;
        SDL_EVENT_LIST(X)
#undef X

        default:
            return "UNKNOWN_EVENT";
    }
}

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
            if (ae::Input::IsKeyPressed(SDL_BUTTON_LEFT)) {
                std::println("Escape key pressed");
                IsRunning = false;
            }
            if (ae::Input::IsMouseButtonPressed(SDL_BUTTON_LMASK)) {
                std::println("Left mouse button pressed");
            }
            if (ae::Input::IsMouseButtonPressed(SDL_BUTTON_RMASK)) {
                std::println("right mouse button pressed");
            }

            // std::println("mouse position :{}",ae::Input::GetMousePosition());
            while (SDL_PollEvent(&Event)) {
                // std::println("event polled {}",GetEventName(Event.type));
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
