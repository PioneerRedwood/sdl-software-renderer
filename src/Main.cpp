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

  ssr::SDLProgram *program = ssr::SDLProgram::instance();

  if (program->init(400, 0, 1024, 768) == false)
  {
    return 1;
  }

  ssr::SDLRenderer &renderer = program->renderer();
  SDL_Renderer *nativeRenderer = program->nativeRenderer();

  // TODO: 프로그램에 필요한 리소스 생성

  // 각 행렬 초기화
  // 모델 행렬
  // 뷰 행렬
  // 프로젝션 행렬

  // 메모리에 상주하는 프레임버퍼

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
