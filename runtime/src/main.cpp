#include "AgentEngine/Engine.hpp"

int main()
{
    ae::Engine engine;

    engine.Initialize();
    engine.Shutdown();

    return 0;
}
