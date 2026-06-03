#include "AgentEngine/Stopwatch.hpp"
#include <chrono>

namespace ae {
    void Stopwatch::Start(){
        startTime = std::chrono::steady_clock::now();
    }
    void Stopwatch::Stop(){
        endTime = std::chrono::steady_clock::now();
        elapsedTime= std::chrono::duration_cast<std::chrono::duration<float>>(endTime - startTime).count();
    }
    float Stopwatch::GetElapsedTime() const{
        return elapsedTime;
    }

}
