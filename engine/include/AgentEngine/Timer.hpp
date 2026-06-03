#pragma once
#include <chrono>
namespace ae {
    class Timer{

        public:
            Timer();
            void update();
            float GetDeltaTime() const;
            float GetTotalTime() const;
        private:
            using Clock = std::chrono::steady_clock;
            Clock::time_point startFrame ;
            Clock::time_point lastFrame ;

            float deltaTime = 0.0f;
            float totalTime = 0.0f;
    };
}
