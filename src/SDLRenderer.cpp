//------------------------------------------------------------------------------
// File: SDLRenderer.cpp
// Author: Chris Redwood
// Created: 2024-10-21
// License: MIT License
//------------------------------------------------------------------------------

#include "SDLRenderer.hpp"

#include <algorithm>
#include <iostream>  // ste::cout long

#include "SDLProgram.hpp"

namespace shmup {

SDLRenderer::SDLRenderer() {}

SDLRenderer::~SDLRenderer() { SDL_DestroyRenderer(m_renderer); }

SDL_Renderer* SDLRenderer::native() { return m_renderer; }

bool SDLRenderer::init(SDL_Window* window, int w, int h) {
  m_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (m_renderer == nullptr) {
    std::cout << "SDL_CreateRenderer failed error: " << SDL_GetError() << std::endl;
    return false;
  }
}

void SDLRenderer::clear() { SDL_RenderClear(m_renderer); }

void SDLRenderer::present() { SDL_RenderPresent(m_renderer); }

void SDLRenderer::flush() { SDL_RenderFlush(m_renderer); }

}  // namespace shmup

