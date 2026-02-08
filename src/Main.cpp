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
#include <algorithm>
#include <cmath>

#include "SDLProgram.hpp"
#include "Math.hpp"
#include "Camera.hpp"

#define Z_NEAR 0.1f
#define Z_FAR  10.0f
#define SCREEN_WIDTH 720
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
std::vector<float> g_depthBuffer;
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

// 계산 순서:
// 1. 정점 좌표와 카메라 좌표 행렬 곱 -> 카메라 기준으로 좌표계 이동
// 2. 프로젝션 행렬 곱 -> 2차원 좌표를 3차원에 투영하는 과정
// 3. 차원 감소 -> 3차원에서 2차원으로 동차좌표계 (NDC 노말 디바이스 좌표계로 변환)
// 4. 뷰포트 적용
void transformToScreen(ssr::Vector4& point) {
  // Note: Matrix * Vector operator implements row-vector math (v * M).
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

const int TEX_W = 256, TEX_H = 256;
struct SimpleMesh {
  std::vector<ssr::Vector3> vertices;
  std::vector<uint32_t> indices;
  std::vector<ssr::Vector2> uvs;
  std::vector<uint32_t> texture;
};

SimpleMesh g_mesh;
std::vector<ssr::Vector3> g_transformedVerts;
float g_meshRotationDeg = 0.0f;
const float g_meshRotationSpeedDegPerSec = 25.0f;

void drawPoint(int x, int y, int color) {
  if (x > SCREEN_WIDTH || x < 0) return;
  if (y > SCREEN_HEIGHT || y < 0) return;

  g_frameBuffer[x + y * SCREEN_WIDTH] = color;
}

std::vector<uint32_t> createProceduralTexture() {
  std::vector<uint32_t> texture(TEX_W * TEX_H);
  const uint32_t bottom = 0xFF4A2F1F;
  const uint32_t top = 0xFFFAD89B;
  for (int y = 0; y < TEX_H; ++y) {
    float v = (float)y / (TEX_H - 1);
    uint32_t color = ssr::math::lerpColor(bottom, top, v);
    for (int x = 0; x < TEX_W; ++x) {
      texture[x + y * TEX_W] = color;
    }
  }
  return texture;
}

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

// 3차원 백페이스 컬링 시 사용 가능
// 두 벡터로 이루어진 면과 정점 사이의 관계를 계산
// result == 0: 면에 정점이 존재하는 것
// result > 0: 면보다 앞에 있는 것
// result < 0: 면보다 뒤에 있는 것
// 
// "한 점이 삼각형의 각 변 기준으로 어느 쪽에 있는지" 판단하는 부호값
// edge > 0: 점P는 선분 a->b 왼쪽
// edge < 0: 점P는 선분 a->b 오른쪽
// edge = 0: 점P는 선분 위
static float edgeFunction(const ssr::Vector2& a, const ssr::Vector2& b, float x, float y) {
  return (x - a.x) * (b.y - a.y) - (y - a.y) * (b.x - a.x);
}

static uint32_t sampleTexture(const std::vector<uint32_t>& texture, float u, float v) {
  if (u < 0.0f) u = 0.0f;
  if (u > 1.0f) u = 1.0f;
  if (v < 0.0f) v = 0.0f;
  if (v > 1.0f) v = 1.0f;
  int tx = (int)(u * (TEX_W - 1));
  int ty = (int)(v * (TEX_H - 1));
  return texture[tx + ty * TEX_W];
}

// 바리센트릭 가중치를 사용해서 점 p0~p2를 각 uv0~uv2에 맞는 색상 값을 구해서 점 그리기
// 바리센트릭 가중치를 구하기 위해서 우선 세가지 정점으로 구성된 삼각형의
// 내부와 그 정점마다 삼각형으로부터 얼마나 가까운지 각 uv에 어떤 가중치를 줄지 계산
static void drawTexturedTriangle(const ssr::Vector3& p0, const ssr::Vector3& p1, const ssr::Vector3& p2,
                                 const ssr::Vector2& uv0, const ssr::Vector2& uv1, const ssr::Vector2& uv2,
                                 const std::vector<uint32_t>& texture,
                                 std::vector<float>& depthBuffer) {
  ssr::Vector2 a{ p0.x, p0.y };
  ssr::Vector2 b{ p1.x, p1.y };
  ssr::Vector2 c{ p2.x, p2.y };

  float minX = std::min(a.x, std::min(b.x, c.x));
  float maxX = std::max(a.x, std::max(b.x, c.x));
  float minY = std::min(a.y, std::min(b.y, c.y));
  float maxY = std::max(a.y, std::max(b.y, c.y));

  int x0 = std::max(0, (int)std::floor(minX));
  int x1 = std::min(SCREEN_WIDTH - 1, (int)std::ceil(maxX));
  int y0 = std::max(0, (int)std::floor(minY));
  int y1 = std::min(SCREEN_HEIGHT - 1, (int)std::ceil(maxY));

  // 만약 이 삼각형의 영역이 0이라면 조기 리턴
  float area = edgeFunction(a, b, c.x, c.y);
  if (area == 0.0f) {
    return;
  }

  // 삼각형을 그려야 하는 범위 (사각영역)
  for (int y = y0; y <= y1; ++y) {
    for (int x = x0; x <= x1; ++x) {
      float px = x + 0.5f;
      float py = y + 0.5f;

      // 각 정점이 이루는 선분으로부터 해당 점에 대한 가중치값 계산
      float w0 = edgeFunction(b, c, px, py);
      float w1 = edgeFunction(c, a, px, py);
      float w2 = edgeFunction(a, b, px, py);

      // 삼각형의 모든 변에 대해 같은 방향으로 있어야 
      // 내부로 판정됨
      if ((area > 0.0f && (w0 < 0.0f || w1 < 0.0f || w2 < 0.0f)) ||
          (area < 0.0f && (w0 > 0.0f || w1 > 0.0f || w2 > 0.0f))) {
        continue;
      }

      float invArea = 1.0f / area;
      w0 *= invArea;
      w1 *= invArea;
      w2 *= invArea;

      float u = uv0.x * w0 + uv1.x * w1 + uv2.x * w2;
      float v = uv0.y * w0 + uv1.y * w1 + uv2.y * w2;
      float z = p0.z * w0 + p1.z * w1 + p2.z * w2;

      // 만약 z값이 깊이 버퍼에 있는 값보다 큰 경우 보이지 않음
      int depthIndex = x + y * SCREEN_WIDTH;
      if (z >= depthBuffer[depthIndex]) {
        continue;
      }
      uint32_t color = sampleTexture(texture, u, v);
      
      // 알파값이 만약 0이라면 그리지 않고 건너뜀
      if ((color >> 24) == 0) {
        continue;
      }

      // 깊이 버퍼 값 업데이트
      depthBuffer[depthIndex] = z;
      drawPoint(x, y, color);
    }
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

  float minX = mesh.vertices[0].x;
  float maxX = mesh.vertices[0].x;
  float minY = mesh.vertices[0].y;
  float maxY = mesh.vertices[0].y;
  for (const auto& v : mesh.vertices) {
    minX = std::min(minX, v.x);
    maxX = std::max(maxX, v.x);
    minY = std::min(minY, v.y);
    maxY = std::max(maxY, v.y);
  }
  const float invRangeX = (maxX - minX) != 0.0f ? 1.0f / (maxX - minX) : 0.0f;
  const float invRangeY = (maxY - minY) != 0.0f ? 1.0f / (maxY - minY) : 0.0f;

  mesh.uvs.reserve(mesh.vertices.size());
  for (const auto& v : mesh.vertices) {
    float u = (v.x - minX) * invRangeX;
    float t = (v.y - minY) * invRangeY;
    mesh.uvs.push_back({ u, t });
  }

  mesh.texture = createProceduralTexture();
  return mesh;
}

SimpleMesh createCubeMesh() {
	SimpleMesh mesh;
  mesh.vertices = {
    // Front face (z = -1)
    { -1.0f, -1.0f, -1.0f },
    { -1.0f,  1.0f, -1.0f },
    {  1.0f,  1.0f, -1.0f },
		{  1.0f, -1.0f, -1.0f },

    // Back face (z = +1)
    {  1.0f, -1.0f,  1.0f },
    {  1.0f,  1.0f,  1.0f },
    { -1.0f,  1.0f,  1.0f },
		{ -1.0f, -1.0f,  1.0f },

    // Right face (x = +1)
    {  1.0f, -1.0f, -1.0f },
    {  1.0f,  1.0f, -1.0f },
    {  1.0f,  1.0f,  1.0f },
		{  1.0f, -1.0f,  1.0f },

    // Left face (x = -1)
    { -1.0f, -1.0f,  1.0f },
    { -1.0f,  1.0f,  1.0f },
    { -1.0f,  1.0f, -1.0f },
		{ -1.0f, -1.0f, -1.0f },

    // Top face (y = +1)
    { -1.0f,  1.0f, -1.0f },
    { -1.0f,  1.0f,  1.0f },
    {  1.0f,  1.0f,  1.0f },
		{  1.0f,  1.0f, -1.0f },

    // Bottom face (y = -1)
    { -1.0f, -1.0f,  1.0f },
    { -1.0f, -1.0f, -1.0f },
    {  1.0f, -1.0f, -1.0f },
		{  1.0f, -1.0f,  1.0f },
	};

  mesh.indices = {
		0, 1, 2, 0, 2, 3,       // Front
		4, 5, 6, 4, 6, 7,       // Back
		8, 9,10, 8,10,11,       // Right
		12,13,14, 12,14,15,     // Left
		16,17,18, 16,18,19,     // Top
		20,21,22, 20,22,23,     // Bottom
	};

  mesh.uvs = {
    // Front
    { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f },
    // Back
    { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f },
    // Right
    { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f },
    // Left
    { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f },
    // Top
    { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f },
    // Bottom
    { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f },
  };

  mesh.texture = createProceduralTexture();
  return mesh;
}

void initMesh() {
  //g_mesh = createTetrahedronMesh();
  g_mesh = createCubeMesh();
  g_transformedVerts.resize(g_mesh.vertices.size());
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
void renderMeshTextured(double deltaMs) {
  if (g_mesh.vertices.empty() || g_mesh.indices.empty()) {
    g_logThisFrame = false;
    return;
  }

  if (g_transformedVerts.size() != g_mesh.vertices.size()) {
    g_transformedVerts.resize(g_mesh.vertices.size());
  }
  if (g_mesh.uvs.size() != g_mesh.vertices.size()) {
    g_logThisFrame = false;
    return;
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

  for (size_t idx = 0; idx + 2 < g_mesh.indices.size(); idx += 3) {
    uint32_t i0 = g_mesh.indices[idx];
    uint32_t i1 = g_mesh.indices[idx + 1];
    uint32_t i2 = g_mesh.indices[idx + 2];
    const ssr::Vector3& v0 = g_transformedVerts[i0];
    const ssr::Vector3& v1 = g_transformedVerts[i1];
    const ssr::Vector3& v2 = g_transformedVerts[i2];
    const ssr::Vector2& uv0 = g_mesh.uvs[i0];
    const ssr::Vector2& uv1 = g_mesh.uvs[i1];
    const ssr::Vector2& uv2 = g_mesh.uvs[i2];

    drawTexturedTriangle(v0, v1, v2, uv0, uv1, uv2, g_mesh.texture, g_depthBuffer);
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
    const size_t depthSize = SCREEN_WIDTH * SCREEN_HEIGHT;
    if (g_depthBuffer.size() != depthSize) {
      g_depthBuffer.assign(depthSize, 1.0f);
    } else {
      std::fill(g_depthBuffer.begin(), g_depthBuffer.end(), 1.0f);
    }

    renderMeshTextured(g_program->delta());

    SDL_UpdateTexture(g_screenTexture, nullptr, g_frameBuffer, SCREEN_WIDTH * 4);
    SDL_RenderCopy(renderer.native(), g_screenTexture, nullptr, nullptr);
    renderer.present();

    SDL_Delay(1);
  }

  return 0;
}
