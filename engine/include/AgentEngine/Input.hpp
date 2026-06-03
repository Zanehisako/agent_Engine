#pragma once
namespace ae {
    class Input {
    public:
        static void Update();
        static bool IsKeyPressed(int key);
        static bool IsMouseButtonPressed(int button);
        // static std::pair<int, int> GetMousePosition();
    };
}
