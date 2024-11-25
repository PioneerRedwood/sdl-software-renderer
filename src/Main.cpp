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

#define Z_NEAR 0.1f
#define Z_FAR  10.0f
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define NUM_OF_STARS 300

////////////////////////////////////////////////////////////

// Global variables
ssr::Matrix4x4 g_viewMat = ssr::Matrix4x4::identity;
ssr::Matrix4x4 g_projectionMat = ssr::Matrix4x4::identity;
ssr::Matrix4x4 g_viewportMat = ssr::Matrix4x4::identity;

ssr::SDLProgram *g_program;
SDL_Texture* g_screenTexture;

ssr::Camera g_camera;
unsigned int* g_frameBuffer = nullptr;
ssr::Vector3 g_positionsOfStars[NUM_OF_STARS];

////////////////////////////////////////////////////////////

void initMatrices() {
  // 뷰 행렬
  ssr::math::lookAt(g_viewMat, g_camera.m_eye, g_camera.m_at, g_camera.m_up);

  // 프로젝션 행렬
  ssr::math::perspectiveProject(g_projectionMat, g_camera.m_fov, 
                                g_camera.m_aspect, Z_NEAR, Z_FAR);
  
  // 뷰포트 행렬
  ssr::math::viewport(g_viewportMat, 0, 0, (float)g_program->width(), (float)g_program->height());
}

#define LOG_MATRIX 0

void transformToScreen(ssr::Matrix4x4& mat, ssr::Vector4& point) {
#if LOG_MATRIX
  std::cout << "=====================================\n";
  std::cout << "basic mat \n";
  mat.print();

  // 모델 뷰 변환
  mat = g_viewMat * mat;
  std::cout << "after view applied\n";
  mat.print();

  // 프로젝션 
  mat = g_projectionMat * mat;
  std::cout << "after proj applied\n";
  mat.print();

  // 뷰포트 변환
  mat = g_viewportMat * mat;
  std::cout << "after viewport applied\n";
  mat.print();
  std::cout << "=====================================\n";
#else
  // Viewport * Projection * View * Model
  mat = g_viewMat * mat;
  mat = g_projectionMat * mat;
  mat = g_viewportMat * mat;
//  mat.print();
#endif

  point = mat * point;

  // 프로젝션 분할(projectionDevide) 클립좌표계 -> NDC로 변환
  point.perspectiveDivide();
}

void drawPoint(int x, int y, int color) {
  if(x > SCREEN_WIDTH || x < 0) return;
  if(y > SCREEN_HEIGHT || y < 0) return;

  g_frameBuffer[x + y * SCREEN_WIDTH] = color;
}

ssr::Vector3 getNewPos() {
  float range = 2.f;
  ssr::Vector3 pos;
  pos.x = (rand() % (int)(range * 2 * 10) * 0.1f) - range;
  pos.y = (rand() % (int)(range * 2 * 10) * 0.1f) - range;
  pos.z = (float)(rand() % (int)Z_FAR);

  return pos;
}

void renderStar() {
  for(int i = 0; i < NUM_OF_STARS; ++i) {
    ssr::Vector3& star = g_positionsOfStars[i];
    ssr::Vector4 pos = { star.x, star.y, star.z, 1.0f };
    ssr::Matrix4x4 mat = ssr::Matrix4x4::identity;
    transformToScreen(mat, pos);

    // draw point
    int color = 0xFFFFFFFF;
    drawPoint(pos.x, pos.y, color);

    // rest star position
    star.z -= 0.02f;
    if(star.z < 0) {
      star = getNewPos();
    }
  }
}

int main(int argc, char **argv)
{
  srand((unsigned)time(nullptr));

  g_program = ssr::SDLProgram::instance();
  if (g_program->init(400, 0, SCREEN_WIDTH, SCREEN_HEIGHT) == false) {
    return 1;
  }

  ssr::SDLRenderer &renderer = g_program->renderer();
//  SDL_RenderSetLogicalSize(renderer.native(), SCREEN_WIDTH, SCREEN_HEIGHT);
//  SDL_RenderSetIntegerScale(renderer.native(), SDL_TRUE);

  // 메모리에 상주하는 프레임버퍼 생성
  g_frameBuffer = new unsigned int[g_program->width() * g_program->height() * 4];
  g_screenTexture = SDL_CreateTexture(renderer.native(), SDL_PIXELFORMAT_RGBA8888, 
                                          SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
  if(g_screenTexture == nullptr) {
    std::cout << "Failed to create g_screenTexture \n";
    return 1;
  }
  
  // 카메라 설정
  g_camera.m_aspect = (float)g_program->width() / g_program->height();
  g_camera.m_fov = 45.0f;

  // 매트릭스 초기화
  initMatrices();

  // 스타 좌표 초기화
  for(int i = 0; i < NUM_OF_STARS; ++i) {
    g_positionsOfStars[i] = getNewPos();
  }

  // Main loop
  g_program->updateTime();
  while (g_program->neededQuit() == false)
  {
    // Update delta
    g_program->updateTime();

    // Handle input events
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0)
    {
      int move = 0;
      switch (event.type)
      {
      case SDL_QUIT:
      {
        g_program->quit();
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
    memset((char*)g_frameBuffer, 0, sizeof(int) * SCREEN_WIDTH * SCREEN_HEIGHT);
    renderStar();

    SDL_UpdateTexture(g_screenTexture, nullptr, g_frameBuffer, SCREEN_WIDTH * 4);
    SDL_RenderCopy(renderer.native(), g_screenTexture, nullptr, nullptr);
    renderer.present();

    SDL_Delay(1); // Almost no delayed
  }

  return 0;
}
