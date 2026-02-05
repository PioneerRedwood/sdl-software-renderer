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
#include <cstring>
#include <vector>
#include <cstdint>

#include "SDLProgram.hpp"
#include "Math.hpp"
#include "Camera.hpp"

#define Z_NEAR 0.1f
#define Z_FAR  10.0f
#define SCREEN_WIDTH 360
#define SCREEN_HEIGHT 640

#pragma mark Global Variables

// Global variables
ssr::Matrix4x4 g_cameraMat = ssr::Matrix4x4::identity;
ssr::Matrix4x4 g_projectionMat = ssr::Matrix4x4::identity;
ssr::Matrix4x4 g_viewportMat = ssr::Matrix4x4::identity;

ssr::SDLProgram *g_program;
SDL_Texture* g_screenTexture;

ssr::Camera g_camera;
unsigned int* g_frameBuffer = nullptr;
bool g_logThisFrame = false;

bool isSimTestEnabled() {
  const char* env = std::getenv("SSR_SIM_TEST");
  return env != nullptr &&
         (strcmp(env, "1") == 0 || strcmp(env, "true") == 0 || strcmp(env, "TRUE") == 0);
}

void initMatrices(float width, float height) {
	// 뷰 행렬
	ssr::math::setupCameraMatrix(g_cameraMat, g_camera.m_eye, g_camera.m_at, g_camera.m_up);

	// 프로젝션 행렬
	ssr::math::setupPerspectiveProjectionMatrix(g_projectionMat, g_camera.m_fov,
		g_camera.m_aspect, Z_NEAR, Z_FAR);

	// 뷰포트 행렬
	ssr::math::setupViewportMatrix(g_viewportMat, 0, 0,
    width, height, Z_NEAR, Z_FAR);
}

void transformToScreen(ssr::Vector4& point) {
  // View * Projection
  point = (g_projectionMat * (g_cameraMat * point));

  // 프로젝션 분할(projectionDevide) 클립좌표계 -> NDC로 변환
  point.perspectiveDivide();

  // Viewport 변환
  point = g_viewportMat * point;
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
    g_camera.m_eye.x += 0.1f;
    g_cameraMat = ssr::Matrix4x4::identity;
    ssr::math::setupCameraMatrix(
      g_cameraMat, g_camera.m_eye, g_camera.m_at, g_camera.m_up);
    printf("Key Input: SDLK_RIGHT => Camera position changed %s\n", g_camera.m_eye.toString().c_str());
    break;
  }
  case SDLK_LEFT: {
    g_camera.m_eye.x -= 0.1f;
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
    g_camera.m_eye.z = -5.0f;
    g_camera.m_at.x = 0.0f;
    g_camera.m_at.y = 0.0f;
    g_camera.m_at.z = 1.0f;
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

#pragma mark Game Logic

struct SimpleMesh {
  std::vector<ssr::Vector3> vertices;
  std::vector<uint32_t> indices;
};

SimpleMesh g_mesh;
std::vector<ssr::Vector3> g_transformedVerts;
float g_meshRotationDeg = 0.0f;
const float g_meshRotationSpeedDegPerSec = 25.0f;

void simulateInputForFrame(int frame) {
  SDL_Event event{};
  event.type = SDL_KEYDOWN;

  switch (frame) {
  case 1:
    event.key.keysym.sym = SDLK_RIGHT;
    handleKeyInput(event);
    break;
  case 2:
    event.key.keysym.sym = SDLK_LEFT;
    handleKeyInput(event);
    break;
  case 3:
    event.key.keysym.sym = SDLK_UP;
    handleKeyInput(event);
    break;
  case 4:
    event.key.keysym.sym = SDLK_DOWN;
    handleKeyInput(event);
    break;
  default:
    break;
  }
}

void logFrameState(int frame) {
  printf("[SIM] frame=%d eye=%s at=%s fov=%.2f\n",
         frame, g_camera.m_eye.toString().c_str(), g_camera.m_at.toString().c_str(), g_camera.m_fov);

  size_t verticesToLog = g_mesh.vertices.size();
  if (verticesToLog > 4) {
    verticesToLog = 4;
  }
  for (size_t i = 0; i < verticesToLog; ++i) {
    ssr::Vector4 v = { g_mesh.vertices[i].x, g_mesh.vertices[i].y, g_mesh.vertices[i].z, 1.0f };
    v = (g_projectionMat * (g_cameraMat * v));
    v.perspectiveDivide();
    v = g_viewportMat * v;
    printf("[SIM] v%zu world=%s screen=(%.2f, %.2f, %.2f)\n",
           i, g_mesh.vertices[i].toString().c_str(), v.x, v.y, v.z);
  }
}

SimpleMesh createTetrahedronMesh() {
  SimpleMesh mesh;
  mesh.vertices = {
    { 0.0f,  1.0f,  0.0f },
    { -1.0f, -1.0f,  1.0f },
    { 1.0f, -1.0f,  1.0f },
    { 0.0f, -1.0f, -1.5f },
  };

  mesh.indices = {
    0, 1, 2,
    0, 2, 3,
    0, 3, 1,
    1, 3, 2
  };
  return mesh;
}

void initMesh() {
  g_mesh = createTetrahedronMesh();
  g_transformedVerts.resize(g_mesh.vertices.size());
}

void drawPoint(int x, int y, int color) {
  if(x > SCREEN_WIDTH || x < 0) return;
  if(y > SCREEN_HEIGHT || y < 0) return;

  g_frameBuffer[x + y * SCREEN_WIDTH] = color;
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
void renderMeshWireframe(double deltaMs) {
  if (g_mesh.vertices.empty() || g_mesh.indices.empty()) {
    g_logThisFrame = false;
    return;
  }

  if (g_transformedVerts.size() != g_mesh.vertices.size()) {
    g_transformedVerts.resize(g_mesh.vertices.size());
  }

  const float deltaSeconds = static_cast<float>(deltaMs) * 0.001f;
  if (deltaSeconds > 0.0f) {
    g_meshRotationDeg += g_meshRotationSpeedDegPerSec * deltaSeconds;
    if (g_meshRotationDeg >= 360.0f) {
      g_meshRotationDeg -= 360.0f;
    }
  }

  ssr::Matrix4x4 modelMat = ssr::Matrix4x4::identity;
  modelMat.rotateY(g_meshRotationDeg);

  for (size_t i = 0; i < g_mesh.vertices.size(); ++i) {
    ssr::Vector4 v = { g_mesh.vertices[i].x, g_mesh.vertices[i].y, g_mesh.vertices[i].z, 1.0f };
    v = modelMat * v;
    v = (g_projectionMat * (g_cameraMat * v));
    v.perspectiveDivide();
    v = g_viewportMat * v;
    g_transformedVerts[i].x = v.x;
    g_transformedVerts[i].y = v.y;
    g_transformedVerts[i].z = v.z;

    if (g_logThisFrame && i < 4) {
      printf("screen v%zu => %s\n", i, g_transformedVerts[i].toString().c_str());
    }
  }

  static const int whiteColor = 0xFFFFFFFF;
  for (size_t idx = 0; idx + 2 < g_mesh.indices.size(); idx += 3) {
    const ssr::Vector3& v0 = g_transformedVerts[g_mesh.indices[idx]];
    const ssr::Vector3& v1 = g_transformedVerts[g_mesh.indices[idx + 1]];
    const ssr::Vector3& v2 = g_transformedVerts[g_mesh.indices[idx + 2]];

    drawLineWithBresenhamAlgorithm({ v0.x, v0.y }, { v1.x, v1.y }, whiteColor);
    drawLineWithBresenhamAlgorithm({ v1.x, v1.y }, { v2.x, v2.y }, whiteColor);
    drawLineWithBresenhamAlgorithm({ v0.x, v0.y }, { v2.x, v2.y }, whiteColor);
  }

  g_logThisFrame = false;
}

#pragma mark Main func

int main(int argc, char **argv)
{
  srand((unsigned)time(nullptr));

  const bool sim_test = isSimTestEnabled();
  const int sim_total_frames = 5;

  if (sim_test) {
    g_camera.m_aspect = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
    g_camera.m_fov = 45.0f;
    g_camera.m_eye = { 0.0f, 0.0f, -5.0f };
    g_camera.m_at = { 0.0f, 0.0f, 1.0f };
    initMatrices((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
    initMesh();

    for (int frame = 0; frame < sim_total_frames; ++frame) {
      simulateInputForFrame(frame);
      logFrameState(frame);
    }
    return 0;
  }

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
  initMatrices((float)g_program->width(), (float)g_program->height());

  initMesh();

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

    renderMeshWireframe(g_program->delta());

    SDL_UpdateTexture(g_screenTexture, nullptr, g_frameBuffer, SCREEN_WIDTH * 4);
    SDL_RenderCopy(renderer.native(), g_screenTexture, nullptr, nullptr);
    renderer.present();

    SDL_Delay(1);
  }

  return 0;
}
