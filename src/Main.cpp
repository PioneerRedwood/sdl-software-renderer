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
#include "Camera.hpp"

// Global variables
ssr::Matrix4x4 g_viewMat = ssr::Matrix4x4::identity;
ssr::Matrix4x4 g_projectionMat = ssr::Matrix4x4::identity;
ssr::Matrix4x4 g_viewportMat = ssr::Matrix4x4::identity;

unsigned int* g_frameBuffer = nullptr;
ssr::Vector3 g_tempPosition = { 1.0f, 1.0f, 1.0f };

#define DEGREE (3.141592f / 180.0f)
#define Z_NEAR 0.1f
#define Z_FAR  10.0f

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
  ssr::Camera camera;
  camera.m_aspect = (float)program->width() / program->height();
  camera.m_fov = 45.0f * DEGREE;

  // 각 행렬 초기화
  // 뷰 행렬
  ssr::math::lookAt(g_viewMat, camera.m_eye, camera.m_at, camera.m_up);

  // 프로젝션 행렬
  ssr::math::perspectiveProject(g_projectionMat, camera.m_fov, 
                                camera.m_aspect, Z_NEAR, Z_FAR);
  
  // 뷰포트 행렬
  ssr::math::viewport(g_viewportMat, 0, 0, (float)program->width(), (float)program->height());

  // 메모리에 상주하는 프레임버퍼 생성
  g_frameBuffer = new unsigned int[program->width() * program->height() * 4];

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
    
    // Update rendering objects
    // 행렬 적용 순서: 모델 뷰 변환 -> 프로젝션(원근 투영) -> 뷰포트 변환 -> 원근 분할
    ssr::Matrix4x4 mat;
    mat = g_viewportMat * (g_projectionMat * (g_viewMat * mat));
    ssr::Vector4 pos = mat.transform4(g_tempPosition);
    pos.perspectiveDivide();

    SDL_SetRenderDrawColor(renderer.native(), 12, 10, 40, 255);
    renderer.clear();

    SDL_SetRenderDrawColor(nativeRenderer, 255, 255, 255, 255);
    SDL_RenderDrawPoint(nativeRenderer, pos.x, pos.y);
    renderer.present();

    SDL_Delay(1); // Almost no delayed
  }

  return 0;
}
