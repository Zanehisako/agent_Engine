#include "AgentEngine/Timer.hpp"
#include <chrono>
namespace ae {
    Timer::Timer(){
        startFrame= Clock::now();
        lastFrame = startFrame;
    }

    void Timer::update(){
        Clock::time_point currentTime = Clock::now();
        deltaTime = std::chrono::duration<float>(currentTime- lastFrame).count();
        lastFrame = currentTime;
        totalTime = std::chrono::duration<float>(currentTime - startFrame).count();
    }
    float Timer::GetDeltaTime() const {
        return deltaTime;
    }
    float Timer::GetTotalTime() const{
        return totalTime;
    }

}
