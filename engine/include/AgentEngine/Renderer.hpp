#pragma once
namespace ae {
    class Renderer{
        public:
            Renderer();
            Renderer(SDL_Renderer* renderer);
            ~Renderer();
            void BeginDraw();
            void EndDraw();
            void Clear();
        private:
            SDL_Renderer* m_renderer;
    };
}
