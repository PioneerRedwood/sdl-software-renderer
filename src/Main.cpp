//------------------------------------------------------------------------------
// File: main.cpp
// Author: Chris Redwood
// Created: 2024-10-21
// License: MIT License
//------------------------------------------------------------------------------

#include <SDL.h>

#include <cstdlib>
#include <iostream>
#include <memory>

#include "SDLProgram.hpp"
#include "Math.hpp"

int main(int argc, char **argv)
{
  std::srand((unsigned)time(nullptr));

  shmup::SDLProgram *program = shmup::SDLProgram::instance();

  if (program->init(400, 0, 1024, 768) == false)
  {
    return 1;
  }

  auto &renderer = program->renderer();
  auto *nativeRenderer = program->nativeRenderer();

  // Main loop
  program->updateTime();
  while (program->neededQuit() == false)
  {
    // Update delta
    program->updateTime();

    // Handle input events
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0)
    {
      int move = 0;
      switch (event.type)
      {
      case SDL_QUIT:
      {
        program->quit();
        return 0;
      }
      case SDL_KEYDOWN:
      {
        switch (event.key.keysym.sym)
        {
        case SDLK_RIGHT:
        {
          move = 1;
          break;
        }
        case SDLK_LEFT:
        {
          move = -1;
          break;
        }
        default:
        {
          break;
        }
        }
        break;
      }
      default:
      {
        break;
      }
      }
    }
    SDL_SetRenderDrawColor(renderer.native(), 12, 10, 40, 255);
    renderer.clear();
    renderer.present();

    SDL_Delay(1); // Almost no delayed
  }

  return 0;
}
