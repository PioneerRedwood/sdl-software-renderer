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
#define SCREEN_WIDTH 360
#define SCREEN_HEIGHT 640

////////////////////////////////////////////////////////////

// Global variables
ssr::Matrix4x4 g_cameraMat = ssr::Matrix4x4::identity;
ssr::Matrix4x4 g_projectionMat = ssr::Matrix4x4::identity;
ssr::Matrix4x4 g_viewportMat = ssr::Matrix4x4::identity;

ssr::SDLProgram *g_program;
SDL_Texture* g_screenTexture;

ssr::Camera g_camera;
unsigned int* g_frameBuffer = nullptr;
bool g_logThisFrame = false;

////////////////////////////////////////////////////////////

void initMatrices() {
	// 뷰 행렬
	ssr::math::setupCameraMatrix(g_cameraMat, g_camera.m_eye, g_camera.m_at, g_camera.m_up);

	// 프로젝션 행렬
	ssr::math::setupPerspectiveProjectionMatrix(g_projectionMat, g_camera.m_fov,
		g_camera.m_aspect, Z_NEAR, Z_FAR);

	// 뷰포트 행렬
	ssr::math::setupViewportMatrix(g_viewportMat, 0, 0, 
    (float)g_program->width(), (float)g_program->height());
}

void transformToScreen(ssr::Vector4& point) {
  // Viewport * Projection * View * Model
  point = (g_viewportMat * (g_projectionMat * g_cameraMat)) * point;

  // 프로젝션 분할(projectionDevide) 클립좌표계 -> NDC로 변환
  point.perspectiveDivide();
}

void drawPoint(int x, int y, int color) {
  if(x > SCREEN_WIDTH || x < 0) return;
  if(y > SCREEN_HEIGHT || y < 0) return;

  g_frameBuffer[x + y * SCREEN_WIDTH] = color;
}

void handleKeyInput(SDL_Event event)
{
  switch (event.key.keysym.sym)
  {
  case SDLK_UP: {
    g_camera.m_fov++;
    g_projectionMat = ssr::Matrix4x4::identity;
    ssr::math::setupPerspectiveProjectionMatrix(
      g_projectionMat, g_camera.m_fov, g_camera.m_aspect, Z_NEAR, Z_FAR);
    printf("Key Input: SDLK_UP => Camera FOV changed %.1f\n", g_camera.m_fov);
    break;
  }
  case SDLK_DOWN: {
    g_camera.m_fov--;
    g_projectionMat = ssr::Matrix4x4::identity;
    ssr::math::setupPerspectiveProjectionMatrix(
      g_projectionMat, g_camera.m_fov, g_camera.m_aspect, Z_NEAR, Z_FAR);
    printf("Key Input: SDLK_DOWN => Camera FOV changed %.1f\n", g_camera.m_fov);
    break;
  }
  case SDLK_RIGHT: {
    //g_camera.m_eye.x += 0.01f;
    //g_camera.m_eye.y += 0.01f;
    g_camera.m_eye.z += 0.01f;
    g_cameraMat = ssr::Matrix4x4::identity;
    ssr::math::setupCameraMatrix(
      g_cameraMat, g_camera.m_eye, g_camera.m_at, g_camera.m_up);
    printf("Key Input: SDLK_RIGHT => Camera position changed %s\n", g_camera.m_eye.toString().c_str());
    break;
  }
  case SDLK_LEFT: {
    //g_camera.m_eye.x -= 0.01f;
    //g_camera.m_eye.y -= 0.01f;
    g_camera.m_eye.z -= 0.01f;
    g_cameraMat = ssr::Matrix4x4::identity;
    ssr::math::setupCameraMatrix(
      g_cameraMat, g_camera.m_eye, g_camera.m_at, g_camera.m_up);
    printf("Key Input: SDLK_LEFT => Camera position changed %s\n", g_camera.m_eye.toString().c_str());
    break;
  }
  case SDLK_r: {
    g_camera.m_fov = 45.0f;
    g_projectionMat = ssr::Matrix4x4::identity;
    ssr::math::setupPerspectiveProjectionMatrix(
      g_projectionMat, g_camera.m_fov, g_camera.m_aspect, Z_NEAR, Z_FAR);

    g_camera.m_eye.x = 0.0f;
    g_camera.m_eye.y = 0.0f;
    g_camera.m_eye.z = 0.0f;
    g_cameraMat = ssr::Matrix4x4::identity;
    ssr::math::setupCameraMatrix(
      g_cameraMat, g_camera.m_eye, g_camera.m_at, g_camera.m_up);

    printf("Key Input: SDLK_r => Camera settings set to default\n");
    break;
  }
  default: break;
  }
  g_logThisFrame = true;
}

////////////////////////////////////////////////////////////

// 삼각형 정점
ssr::Vector3 g_vertices[3];
ssr::Matrix4x4 g_vertsMat = ssr::Matrix4x4::identity;
float g_triangleRotationRadian = 0.0f;

void initVertices() {
  g_vertices[0] = { -0.2f, -0.2f, +0.0f };
  g_vertices[1] = { +0.2f, -0.2f, +0.0f };
  g_vertices[2] = { +0.0f, +0.2f, +0.0f };
}

// #1 Bresenham's line algorithm
// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
void drawLineWithBresenhamAlgorithm(const ssr::Vector2& startPos, const ssr::Vector2& endPos, int color) { 
  auto drawLow = [](int x0, int y0, int x1, int y1, int color) {
    int dx = x1 - x0, dy = y1 - y0;
    int yi = 1;
    if (dy < 0) {
      yi = -1;
      dy = -dy;
    }
    int d = (2 * dy) - dx;
    int y = y0;

    for (int x = x0; x < x1; ++x) {
      drawPoint(x, y, color);
      if (d > 0) {
        y = y + yi;
        d = d + (2 * (dy - dx));
      }
      else {
        d = d + 2 * dy;
      }
    }
  };

  auto drawHigh = [](int x0, int y0, int x1, int y1, int color) {
    int dx = x1 - x0, dy = y1 - y0;
    int xi = 1;
    if (dx < 0) {
      xi = -1;
      dx = -dx;
    }
    int d = (2 * dx) - dy;
    int x = x0;
    
    for (int y = y0; y < y1; ++y) {
      drawPoint(x, y, color);
      if (d > 0) {
        x = x + xi;
        d = d + (2 * (dx - dy));
      }
      else {
        d = d + 2 * dx;
      }
    }
  };

  if (abs(endPos.y - startPos.y) < abs(endPos.x - startPos.x)) {
    if (startPos.x > endPos.x) {
      drawLow(endPos.x, endPos.y, startPos.x, startPos.y, color);
    }
    else {
      drawLow(startPos.x, startPos.y, endPos.x, endPos.y, color);
    }
  }
  else {
    if (startPos.y > endPos.y) {
      drawHigh(endPos.x, endPos.y, startPos.x, startPos.y, color);
    }
    else {
      drawHigh(startPos.x, startPos.y, endPos.x, endPos.y, color);
    }
  }
}

// 주어진 세 개의 3D 정점으로 이루어진 삼각형을 그리기
void renderTriangleLines() {
  // 회전 행렬 적용
  ssr::Matrix4x4 rotationMat = ssr::Matrix4x4::identity;
  //g_triangleRotationRadian += 0.7f;
  //rotationMat.rotateY(g_triangleRotationRadian);

  ssr::Vector3 tri[3];
  ssr::Matrix4x4 transformMat = (g_viewportMat * (g_projectionMat * g_cameraMat));
  if (g_logThisFrame) {
    g_cameraMat.print();
    g_projectionMat.print();
    g_viewportMat.print();
  }
  
  // 화면 좌표계로 변환
  for (int i = 0; i < 3; ++i) {
    tri[i] = rotationMat * g_vertices[i];
    ssr::Vector4 temp = { tri[i].x, tri[i].y, tri[i].z, 1.0f };
    temp = transformMat * temp;
    temp.perspectiveDivide();
    tri[i].x = temp.x, tri[i].y = temp.y, tri[i].z = temp.z;
    if (g_logThisFrame) {
      printf("%d => %s\n", i, tri[i].toString().c_str());
    }
  }

  // 주어진 세 개의 정점으로 픽셀에 선분 그리기 시도 여기서부턴 2D 선분 그리기와 동일
  static const int whiteColor = 0xFFFFFFFF;
  drawLineWithBresenhamAlgorithm({ tri[0].x, tri[0].y }, { tri[1].x, tri[1].y }, whiteColor);
  drawLineWithBresenhamAlgorithm({ tri[1].x, tri[1].y }, { tri[2].x, tri[2].y }, whiteColor);
  drawLineWithBresenhamAlgorithm({ tri[0].x, tri[0].y }, { tri[2].x, tri[2].y }, whiteColor);

  g_logThisFrame = false;
}

////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
  srand((unsigned)time(nullptr));

  g_program = ssr::SDLProgram::instance();
  if (g_program->init(400, 0, SCREEN_WIDTH, SCREEN_HEIGHT) == false) {
    return 1;
  }

  ssr::SDLRenderer &renderer = g_program->renderer();

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

  initVertices();

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
      switch (event.type)
      {
      case SDL_QUIT:
      {
        g_program->quit();
        return 0;
      }
      case SDL_KEYDOWN:
      {
        handleKeyInput(event);
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

    renderTriangleLines();

    SDL_UpdateTexture(g_screenTexture, nullptr, g_frameBuffer, SCREEN_WIDTH * 4);
    SDL_RenderCopy(renderer.native(), g_screenTexture, nullptr, nullptr);
    renderer.present();

    SDL_Delay(1);
  }

  return 0;
}
