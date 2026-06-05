#pragma once
#include <sys/stat.h>
#include <vector>
namespace ae {
    class Input {
    public:
        static void BeginFrame();
        static void EndFrame();
        static void Update();
        static bool IsKeyDown(int key);
        static bool IsKeyPressed(int key);
        static bool IsKeyReleased(int key);
        static bool IsMouseButtonDown(SDL_MouseButtonFlags buttonMask);
        static bool IsMouseButtonClicked(SDL_MouseButtonFlags buttonMask);
        static bool IsMouseButtonReleased(SDL_MouseButtonFlags buttonMask);
        static std::pair<int, int> GetMousePosition();
    private:
        static std::vector<bool> currentKeys;
        static std::vector<bool> prevKeys;
        static Uint32 currentMouseButtons;
        static Uint32 prevMouseButtons;
    };
}
