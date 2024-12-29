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
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
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

ssr::Vertex g_triVerts[3];
float g_triangleRotationRadian = 0.0f;

////////////////////////////////////////////////////////////

void initMatrices() {
	// 뷰 행렬
	ssr::math::setupViewMatrix(g_viewMat, g_camera.m_eye, g_camera.m_at, g_camera.m_up);

	// 프로젝션 행렬
	ssr::math::setupPerspectiveProjectionMatrix(g_projectionMat, g_camera.m_fov,
		g_camera.m_aspect, Z_NEAR, Z_FAR);

	// 뷰포트 행렬
	ssr::math::setupViewportMatrix(g_viewportMat, 0, 0, 
    (float)g_program->width(), (float)g_program->height());
}

#define LOG_MATRIX 0

void transformToScreen(ssr::Vector4& point) {
#if LOG_MATRIX
  ssr::Matrix4x4 mat = ssr::Matrix4x4::identity;
  std::cout << "=====================================\n";

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

  point = mat * point;
#else
  // Viewport * Projection * View * Model
  point = (g_viewportMat * (g_projectionMat * g_viewMat)) * point;
//  mat.print();
#endif

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
    transformToScreen(pos);

    // draw point
    int color = 0xFFFFFFFF;
    drawPoint(pos.x, pos.y, color);

    // rest star position
    star.z -= 0.02f;
    if(star.z <= 1.0f) {
      star = getNewPos();
    }
  }
}

void drawFlatTop(const ssr::Vector3& v0, const ssr::Vector3& v1, const ssr::Vector3& v2) {
  const float m0 = (v2.x - v0.x) / (v2.y - v0.y);
  const float m1 = (v2.x - v1.x) / (v2.y - v1.y);

  const int yStart = (int)ceil(v0.y - 0.5f);
  const int yEnd = (int)ceil(v2.y - 0.5f);

  for (int y = yStart; y < yEnd; ++y) {
    const float px0 = m0 * (float(y) + 0.5f - v0.y) + v0.x;
    const float px1 = m1 * (float(y) + 0.5f - v1.y) + v1.x;

    const int xStart = (int)(px0 - 0.5f);
    const int xEnd = (int)(px1 - 0.5f);

    for (int x = xStart; x < xEnd; ++x) {
      drawPoint(x, y, 0XFFFFFFFF);
    }
  }
}

void drawFlatBottom(const ssr::Vector3& v0, const ssr::Vector3& v1, const ssr::Vector3& v2) {
  const float m0 = (v1.x - v0.x) / (v1.y - v0.y);
  const float m1 = (v2.x - v0.x) / (v2.y - v0.y);

  const int yStart = (int)ceil(v0.y - 0.5f);
  const int yEnd = (int)ceil(v2.y - 0.5f);

  for (int y = yStart; y < yEnd; ++y) {
    const float px0 = m0 * (float(y) + 0.5f - v0.y) + v0.x;
    const float px1 = m1 * (float(y) + 0.5f - v0.y) + v0.x;

    const int xStart = (int)(px0 - 0.5f);
    const int xEnd = (int)(px1 - 0.5f);

    for (int x = xStart; x < xEnd; ++x) {
      drawPoint(x, y, 0xFFFFFFFF);
    }
  }
}

void drawTriangle(const ssr::Vector3& v0, const ssr::Vector3& v1, const ssr::Vector3& v2) {
  using namespace ssr;

  // 오름차순 정렬을 위해서 임시 포인터 변수 선언 및 초기화
  const Vector3* pv0 = &v0;
  const Vector3* pv1 = &v1;
  const Vector3* pv2 = &v2;

  // 먼저 Y 좌표 기준으로 정점 포인터 변수 정렬
  if (pv1->y < pv0->y) std::swap(pv0, pv1);
  if (pv2->y < pv1->y) std::swap(pv1, pv2);
  if (pv1->y < pv0->y) std::swap(pv0, pv1);

  // 삼각형 위 두 정점 평평한 경우
  if (pv0->y == pv1->y) {
    // 위 두 정점을 X 좌표 기준으로 정렬
    if (pv1->x < pv0->x) std::swap(pv0, pv1);
    drawFlatTop(*pv0, *pv1, *pv2);
  }
  // 삼각형 아래 두 정점 평평한 경우
  else if (pv1->y == pv2->y) {
    // 아래 두 정점을 X 좌표 기준으로 정렬
    if (pv2->x < pv1->x) std::swap(pv1, pv2);
    drawFlatBottom(*pv0, *pv1, *pv2);
  }
  else {
    // 정점을 쪼개는 탐색 과정
    const float alphaSplit = (pv1->y - pv0->y) / (pv2->y - pv0->y);
    const Vector3 vi = *pv0 + (*pv2 - *pv0) * alphaSplit;

    if (pv1->x < vi.x) {
      drawFlatBottom(*pv0, *pv1, vi);
      drawFlatTop(*pv1, vi, *pv2);
    }
    else {
      drawFlatBottom(*pv0, vi, *pv1);
      drawFlatTop(vi, *pv1, *pv2);
    }
  }
}

void initTriangle() {
  //g_triVerts[0].pos = { -1.0f, +1.0f, +0.0f };
  //g_triVerts[1].pos = { +1.0f, +1.0f, +0.0f };
  //g_triVerts[2].pos = { +0.0f, -1.0f, +0.0f };
  g_triVerts[0].pos = { -0.2f, +0.2f, +0.0f };
  g_triVerts[1].pos = { +0.2f, +0.2f, +0.0f };
  g_triVerts[2].pos = { +0.0f, -0.2f, +0.0f };
}

void renderTriangle() {
  using namespace ssr;
  using namespace ssr::math;

  // TODO: Matrix Y축 기준 회전!
  Matrix4x4 rotationMat = Matrix4x4::identity;
  g_triangleRotationRadian += 0.3f;
  rotationMat.rotateY(g_triangleRotationRadian * DEG2RAD);

  Vector3 tri[3];
  Matrix4x4 mat = g_viewportMat * (g_projectionMat * (g_viewMat));

  for (int i = 0; i < 3; ++i) {
    tri[i] = rotationMat * g_triVerts[i].pos;
    Vector4 temp = { tri[i].x, tri[i].y, tri[i].z, 1.0f };
    temp = mat * temp;
    temp.perspectiveDivide();
    tri[i].x = temp.x, tri[i].y = temp.y, tri[i].z = temp.z;
  }

  // 삼각형 채워서 렌더링
  drawTriangle(tri[0], tri[1], tri[2]);

  // 삼각형 꼭짓점 렌더링
  //int color = 0x00FF00FF;
  //drawPoint((int)tri[0].x, (int)tri[0].y, color);
  //drawPoint((int)tri[1].x, (int)tri[1].y, color);
  //drawPoint((int)tri[2].x, (int)tri[2].y, color);
}

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

  // 스타 좌표 초기화
  //for(int i = 0; i < NUM_OF_STARS; ++i) {
  //  g_positionsOfStars[i] = getNewPos();
  //}

  initTriangle();

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
        case SDLK_UP: {
          g_camera.m_fov++;
          g_projectionMat = ssr::Matrix4x4::identity;
          ssr::math::setupPerspectiveProjectionMatrix(
            g_projectionMat, g_camera.m_fov, g_camera.m_aspect, Z_NEAR, Z_FAR);
          printf("FOV %.2f \n", g_camera.m_fov);
          break;
        }
        case SDLK_DOWN: {
          g_camera.m_fov--;
          g_projectionMat = ssr::Matrix4x4::identity;
          ssr::math::setupPerspectiveProjectionMatrix(
            g_projectionMat, g_camera.m_fov, g_camera.m_aspect, Z_NEAR, Z_FAR);
          printf("FOV %.2f \n", g_camera.m_fov);
          break;
        }
        case SDLK_RIGHT: {
          g_camera.m_eye.x += 0.01f;
          g_viewMat = ssr::Matrix4x4::identity;
          ssr::math::setupViewMatrix(
            g_viewMat, g_camera.m_eye, g_camera.m_at, g_camera.m_up);
          printf("EYE X %.2f \n", g_camera.m_eye.x);
          break;
        }
        case SDLK_LEFT: {
          g_camera.m_eye.x -= 0.01f;
          g_viewMat = ssr::Matrix4x4::identity;
          ssr::math::setupViewMatrix(
            g_viewMat, g_camera.m_eye, g_camera.m_at, g_camera.m_up);
          printf("EYE X %.2f \n", g_camera.m_eye.x);
          break;
        }
        case SDLK_r: {
          g_camera.m_fov = 45.0f;
          g_projectionMat = ssr::Matrix4x4::identity;
          ssr::math::setupPerspectiveProjectionMatrix(
            g_projectionMat, g_camera.m_fov, g_camera.m_aspect, Z_NEAR, Z_FAR);
          
          g_camera.m_eye.z = 0.1f; // default?
          g_camera.m_eye.x = 0.0f;
          g_viewMat = ssr::Matrix4x4::identity;
          ssr::math::setupViewMatrix(
            g_viewMat, g_camera.m_eye, g_camera.m_at, g_camera.m_up);

          printf("Reset Camera Properties \n");
          break;
        }
        case SDLK_q: {
          g_camera.m_eye.z += 0.01f;
          g_projectionMat = ssr::Matrix4x4::identity;
          ssr::math::setupPerspectiveProjectionMatrix(
            g_projectionMat, g_camera.m_fov, g_camera.m_aspect, Z_NEAR, Z_FAR);
          printf("EYE Z %.2f \n", g_camera.m_eye.z);
          break;
        }
        case SDLK_e: {
          g_camera.m_eye.z -= 0.01f;
          g_projectionMat = ssr::Matrix4x4::identity;
          ssr::math::setupPerspectiveProjectionMatrix(
            g_projectionMat, g_camera.m_fov, g_camera.m_aspect, Z_NEAR, Z_FAR);
          printf("EYE Z %.2f \n", g_camera.m_eye.z);
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
    //renderStar();
    renderTriangle();

    SDL_UpdateTexture(g_screenTexture, nullptr, g_frameBuffer, SCREEN_WIDTH * 4);
    SDL_RenderCopy(renderer.native(), g_screenTexture, nullptr, nullptr);
    renderer.present();

    SDL_Delay(1); // Almost no delayed
  }

  return 0;
}
