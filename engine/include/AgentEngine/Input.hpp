#pragma once
#include <sys/stat.h>
#include <vector>
namespace ae {
    class Input {
    public:
        static constexpr int MAX_KEYS = SDL_SCANCODE_COUNT;
        static constexpr int MAX_WORDS = (MAX_KEYS +63)  / 64;
        using KeyBits = std::array<Uint64, MAX_WORDS>;
        static void BeginFrame();
        static void SampleState();
        // static void EndFrame();
        static void ProcessEvent(const SDL_Event* event);
        static bool IsKeyDown(int key);
        static bool IsKeyPressed(int key);
        static bool IsKeyReleased(int key);
        static bool IsMouseButtonDown(SDL_MouseButtonFlags buttonMask);
        static bool IsMouseButtonClicked(SDL_MouseButtonFlags buttonMask);
        static bool IsMouseButtonReleased(SDL_MouseButtonFlags buttonMask);
        static std::pair<int, int> GetMousePosition();
    private:
        static KeyBits currentKeys;
        static KeyBits prevKeys;
        static KeyBits pressedKeys;
        static KeyBits releasedKeys;
        static Uint32 pressedMouseButtons;
        static Uint32 releasedMouseButtons;
        static Uint32 currentMouseButtons;
        static Uint32 prevMouseButtons;
    };
}
