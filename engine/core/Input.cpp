#include "AgentEngine/Input.hpp"
#include <print>
#include <utility>
#include <vector>

namespace ae {

    Input::KeyBits Input::currentKeys{};
    Input::KeyBits Input::prevKeys{};
    Input::KeyBits Input::pressedKeys{};
    Input::KeyBits Input::releasedKeys{};

    Uint32 Input::currentMouseButtons = 0;
    Uint32 Input::prevMouseButtons = 0;
    Uint32 Input::pressedMouseButtons = 0;
    Uint32 Input::releasedMouseButtons = 0;

    static bool GetKey(const Input::KeyBits& bits,int key){
        int index = static_cast<int>(key);
        if (index < 0 || index >= Input::MAX_KEYS) return false;
        int word = index / 64;
        int bit = index % 64;
        return (bits[word] & (1ULL << bit)) != 0;
    }

    static void SetKey(Input::KeyBits& bits,int key,bool value){
        int index = static_cast<int>(key);
        if (index < 0 || index >= Input::MAX_KEYS) return;
        int word = index / 64;
        int bit = index % 64;
        if (value){
            bits[word] |= (1ULL << bit);
        }else{
            bits[word] &= ~(1ULL <<bit);
        }
    }

    void Input::BeginFrame(){
        prevKeys = currentKeys;
        prevMouseButtons = currentMouseButtons;
    }
    void Input::SampleState(){
        int keyCount = 0;
        const bool* keys = SDL_GetKeyboardState(&keyCount);
        for (int i =0;i<keyCount;++i){
            SetKey(Input::currentKeys, i, keys[i]);
        }
        for (int i = 0; i < Input::MAX_WORDS; ++i){
            pressedKeys[i] = currentKeys[i] & ~prevKeys[i];
            releasedKeys[i] = ~currentKeys[i] & prevKeys[i];
        }
        currentMouseButtons = SDL_GetMouseState(nullptr, nullptr);
        pressedMouseButtons = currentMouseButtons & ~prevMouseButtons;
        releasedMouseButtons = ~currentMouseButtons & prevMouseButtons;
    }
    void Input::ProcessEvent(const SDL_Event* event){
        switch (event->type){
            case SDL_EVENT_KEY_DOWN:
            SetKey(Input::currentKeys, event->key.scancode, true);
            std::println("key was down: {}",static_cast<int>(event->key.scancode));
            break;
            case SDL_EVENT_KEY_UP:
            SetKey(Input::currentKeys, event->key.scancode, true);
            std::println("key was down:{}",static_cast<int>(event->key.scancode));
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
    bool Input::IsKeyDown(int key){
        return GetKey(Input::currentKeys,key);
    }
    bool Input::IsKeyPressed(int key){
        return GetKey(Input::pressedKeys,key);
    }
    bool Input::IsKeyReleased(int key){
        return GetKey(Input::releasedKeys,key);
    }

    bool Input::IsMouseButtonDown(SDL_MouseButtonFlags buttonMask){
        return (buttonMask & currentMouseButtons) != 0;
    }
    bool Input::IsMouseButtonClicked(SDL_MouseButtonFlags buttonMask){
        return (buttonMask & pressedMouseButtons ) ;
    }
    bool Input::IsMouseButtonReleased(SDL_MouseButtonFlags buttonMask){
        return (buttonMask & currentMouseButtons) == 0 && (buttonMask & prevMouseButtons) != 0;
    }

    std::pair<int, int> Input::GetMousePosition(){
        float x, y;
        SDL_GetMouseState(&x, &y);
        return {x, y};
    }
}
