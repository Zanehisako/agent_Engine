#include "AgentEngine/Renderer.hpp"
namespace ae{
    Renderer::Renderer(){

    }
    Renderer::Renderer(SDL_Renderer* renderer){
        m_renderer = renderer;
    }
    Renderer::~Renderer(){
        m_renderer = nullptr;
    }

    void Renderer::BeginDraw(){

        //  Draw a bright red rectangle onto our backbuffer
        SDL_FRect rect = { 250.0f, 175.0f, 300.0f, 250.0f };
        SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(m_renderer, &rect);

    }
    void Renderer::EndDraw(){
        // Present the completed backbuffer to the user
        SDL_RenderPresent(m_renderer);

    }
    void Renderer::Clear(){
        // 1. Clear the screen to a solid dark grey color
        SDL_SetRenderDrawColor(m_renderer, 40, 40, 40, 255);
        SDL_RenderClear(m_renderer);
    }

}
