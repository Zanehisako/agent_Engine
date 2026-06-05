#include "AgentEngine/Input.hpp"
#include <utility>
namespace ae {
    bool Input::IsKeyPressed(int key){
        const bool* state = SDL_GetKeyboardState(NULL);
        return state[key];
    }
    bool Input::IsMouseButtonPressed(int button){
        const auto state = SDL_GetMouseState(NULL,NULL);
        return state & SDL_MouseButtonFlags(button);
    }
    std::pair<int, int> Input::GetMousePosition(){
        float x, y;
        SDL_GetMouseState(&x, &y);
        return {x, y};
    }

}
