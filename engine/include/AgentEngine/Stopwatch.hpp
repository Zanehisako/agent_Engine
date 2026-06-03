#pragma once
#include <chrono>
namespace ae {
    class Stopwatch{
        public:
            void Start();
            void Stop();
            float GetElapsedTime() const;

        private:
            std::chrono::steady_clock::time_point startTime;
            std::chrono::steady_clock::time_point endTime;
            float elapsedTime = 0.0f;

    };
}
