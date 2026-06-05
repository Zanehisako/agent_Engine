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
        int keyCount = 0;
        const bool* keys = SDL_GetKeyboardState(&keyCount);
        currentKeys.reserve(keyCount);
        prevKeys.reserve(keyCount);
        for (int i =0;i<keyCount;++i){
            currentKeys[i] = keys[i];
            prevKeys[i] = keys[i];
        }
        currentMouseButtons = SDL_GetMouseState(nullptr, nullptr);
        std::println("current mouse positions {}",currentMouseButtons);
        prevMouseButtons = currentMouseButtons;
    }
    void Input::Update(){
            prevMouseButtons = currentMouseButtons;
            prevKeys = currentKeys;
            int keyCount = 0;
            const bool* keys = SDL_GetKeyboardState(&keyCount);
            for (int i =0;i<keyCount;++i){
                currentKeys[i] = keys[i];
            }
            currentMouseButtons = SDL_GetMouseState(nullptr,nullptr);
            // if (currentMouseButtons!= 0){
            //     std::println("current mouse buttons :{}",currentMouseButtons);
            // }
    }
    void Input::EndFrame(){
        prevKeys = currentKeys;
        prevMouseButtons = currentMouseButtons;
    }
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
