
#include "AgentEngine/Window.hpp"
#include <string_view>

namespace ae {
    Window::Window(int x,int y,std::string_view title) {
        SDLWindow = SDL_CreateWindow(
          title.data(),
          x, y, 0
        );
      }

      Window::~Window() {
        if (SDLWindow && SDL_WasInit(SDL_INIT_VIDEO)) {
          SDL_DestroyWindow(SDLWindow);
        }
      }
}
