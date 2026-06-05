#include "AgentEngine/Input.hpp"
#include <print>
#include <utility>
namespace ae {
    bool Input::IsKeyPressed(int key){
        const bool* state = SDL_GetKeyboardState(NULL);
        return state[key];
    }
    bool Input::IsMouseButtonPressed(int button){
        float x,y;
        Uint32 buttons = SDL_GetMouseState(&x,&y);
        // std::println("mouse state {}",buttons);
        // std::println("mouse button Flags{}",SDL_MouseButtonFlags(button));
        return  SDL_MouseButtonFlags(button) & buttons ;
    }
    std::pair<int, int> Input::GetMousePosition(){
        float x, y;
        SDL_GetMouseState(&x, &y);
        return {x, y};
    }
}
