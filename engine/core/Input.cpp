#include "AgentEngine/Input.hpp"
#include <iterator>
#include <print>
#include <utility>
#include <vector>
namespace ae {
    std::vector<bool> Input::currentKeys;
    std::vector<bool> Input::prevKeys;
    Uint32 Input::currentMouseButtons = 0;
    Uint32 Input::prevMouseButtons = 0;
    void Input::BeginFrame(){
        prevKeys = currentKeys;
        prevMouseButtons = currentMouseButtons;
        int keyCount = 0;
        const bool* keys = SDL_GetKeyboardState(&keyCount);
        if (currentKeys.size()!= static_cast<size_t>(keyCount)){
            currentKeys.resize(keyCount);
            prevKeys.resize(keyCount);
        }
        for (int i =0;i<keyCount;++i){
            currentKeys[i] = keys[i];
        }
        currentMouseButtons = SDL_GetMouseState(nullptr, nullptr);
        // std::println("current mouse positions {}",currentMouseButtons);
    }
    void Input::ProcessEvent(SDL_Event* event){
        switch (event->type){
            case SDL_EVENT_KEY_DOWN:
            currentKeys[event->key.scancode] = true;
            break;
            case SDL_EVENT_KEY_UP:
            currentKeys[event->key.scancode] = false;
            break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
            break;

            default:
            break;
        }
    }
    // TODO :Implement in the future when we have more complex input
    // void Input::EndFrame(){
    //     prevKeys = currentKeys;
    //     prevMouseButtons = currentMouseButtons;
    // }
    bool Input::IsKeyPressed(int key){
        return currentKeys[key] && !prevKeys[key];
    }

    bool Input::IsKeyDown(int key){
        return currentKeys[key];
    }
    bool Input::IsMouseButtonDown(SDL_MouseButtonFlags buttonMask){
        float x,y;
        return  buttonMask & currentMouseButtons ;
    }
    bool Input::IsMouseButtonClicked(SDL_MouseButtonFlags buttonMask){
        // std::println("current mouse buttons: {}, prev mouse buttons: {}", currentMouseButtons, prevMouseButtons);
        return (buttonMask & currentMouseButtons) && !(buttonMask & prevMouseButtons);
    }
    std::pair<int, int> Input::GetMousePosition(){
        float x, y;
        SDL_GetMouseState(&x, &y);
        return {x, y};
    }
}
