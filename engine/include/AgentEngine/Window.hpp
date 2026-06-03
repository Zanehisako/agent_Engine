#pragma once
#include <SDL3/SDL.h>
#include <string_view>

namespace ae {
    class Window {
    public:
      Window(int x,int y,std::string_view title);
      Window(const Window&) = delete;
      Window& operator=(const Window&) = delete;

      ~Window();

    private:
      SDL_Window* SDLWindow{nullptr};
    };
}
