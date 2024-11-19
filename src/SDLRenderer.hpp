
//------------------------------------------------------------------------------
// File: SDLRenderer.hpp
// Author: Chris Redwood
// Created: 2024-10-21
// License: MIT License
//------------------------------------------------------------------------------

#pragma once

#include <SDL.h>
#include "Blend.hpp"

namespace ssr {

class SDLRenderer {
 public:
  SDLRenderer();

  ~SDLRenderer();

  SDLRenderer(const SDLRenderer&) = delete;
  SDLRenderer& operator=(const SDLRenderer&) = delete;

  SDLRenderer(SDLRenderer&&) = delete;
  SDLRenderer& operator=(SDLRenderer&&) = delete;

  bool init(SDL_Window* window, int x, int y);

  SDL_Renderer* native();

  void clear();

  void present();

  void flush();

 private:
  SDL_Renderer* m_renderer = nullptr;

};

}  // namespace ssr
