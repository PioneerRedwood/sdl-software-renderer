//------------------------------------------------------------------------------
// File: Math.hpp
// Author: Chris Redwood
// Created: 2024-10-30
// License: MIT License
//------------------------------------------------------------------------------

#pragma once

#include <vector>
#include <cstdlib>
#include <cmath>

namespace ssr {

struct Vector2 {
  float x;
  float y;

  Vector2();
  Vector2(float x, float y);

  Vector2 operator+(const Vector2& other) const;
  Vector2 operator*(float scalar) const;
  Vector2& operator=(const Vector2& other);
  Vector2 operator-(const Vector2& other) const;
  bool operator==(const Vector2& other) const;

  /// @brief 벡터의 크기 계산
  float magnitude() const;

  /// @brief 벡터 노멀라이즈 (정규화)
  Vector2 normalized() const;
};

struct Vector3 {
  float x;
  float y;
  float z;

  Vector3();
  Vector3(float x, float y, float z);

  Vector3(const Vector3& other);
  Vector3& operator=(const Vector3& other);

  Vector3 operator+(const Vector3& other) const;
  Vector3 operator*(float scalar) const;
  Vector3 operator-(const Vector3& other) const;
  Vector3 operator/(const Vector3& other) const;
  Vector3 operator/(float value) const;
  bool operator==(const Vector3& other) const;

  /// @brief 각 값을 정규화
  /// @return 정규화된 벡터 반환
  Vector3 normalize() const;
};

namespace math {

/// @brief 두 벡터의 뺄셈을 반환
/// @param v1 첫번째 벡터
/// @param v2 두번째 벡터
/// @return 두 벡터의 뺄셈
Vector3 subtract(const Vector3& v1, const Vector3& v2);

/// @brief 두 벡터의 내적을 반환. 스칼라 값 출력. v1 · v2 = |v1| |v2| cosθ
/// https://en.wikipedia.org/wiki/Dot_product 참고.
/// 두 벡터가 모두 영 벡터가 아닐때 내적이 0이면 두 벡터의 사이각은 반드시 90도.
/// 영 벡터가 아닌 두 벡터의 길이는 항상 양이므로 내적이 0보다 크면 사이각이 
/// 90도 보다 작고, 반대로 0보다 작으면 90도 보다 크다. 
/// @param v1 첫번째 벡터
/// @param v2 두번째 벡터
/// @return 두 벡터의 내적
float dotProduct(const Vector3& v1, const Vector3& v2);

/// @brief 두 벡터의 외적을 반환. 벡터 출력. v1 X v2 = |v1| |v2| sinθ
/// https://en.wikipedia.org/wiki/Cross_product 참고
/// @param v1 첫번째 벡터
/// @param v2 두번째 벡터
/// @return 두 벡터의 외적
Vector3 crossProduct(const Vector3& v1, const Vector3& v2);

}

struct Vector4 {
  float x;
  float y;
  float z;
  float w;

  Vector4();
  Vector4(float, float, float, float);
  void perspectiveDivide();
};

/*
   x   y   z   w -> axis
   v   v   v   v
+-----------------+ +-----------------+
| m11 m12 m13 m14 | | x.x y.x z.x w.x |
| m21 m22 m23 m24 | | x.y y.y z.y w.y |
| m31 m32 m33 m34 | | x.z y.z z.z w.z |
| m41 m42 m43 m44 | | x.w y.w z.w w.w |
+-----------------+ +-----------------+
*/
struct Matrix4x4 {
  float m11, m12, m13, m14;
  float m21, m22, m23, m24;
  float m31, m32, m33, m34;
  float m41, m42, m43, m44;

  Matrix4x4();
  Matrix4x4(float, float, float, float,
            float, float, float, float,
            float, float, float, float,
            float, float, float, float);
  /*
  OpenGL 오른손 좌표계
  +-----------------+   +-----------------+
  | m11 m12 m13 m14 |   | x.x y.x z.x w.x |
  | m21 m22 m23 m24 |   | x.y y.y z.y w.y |
  | m31 m32 m33 m34 |   | x.z y.z z.z w.z |
  | m41 m42 m43 m44 |   | x.w y.w z.w w.w |
  +-----------------+   +-----------------+
  - (m11, m21, m31): X축, 왼쪽 벡터 (left)
  - (m12, m22, m32): Y축, 위쪽 벡터 (up)
  - (m14, m24, m34): Z축, 앞쪽 벡터 (forward)
  */
  Matrix4x4(Vector4 x, Vector4 y, Vector4 z, Vector4 w);
  Matrix4x4(const Vector4&, const Vector4&, const Vector4&, const Vector4&);

  Matrix4x4(const Matrix4x4& other);
  Matrix4x4& operator=(const Matrix4x4& other);

  Matrix4x4 operator+(const Matrix4x4& other) const;
  Matrix4x4 operator*(float scalar) const;
  Matrix4x4 operator-(const Matrix4x4& other) const;
  Matrix4x4 operator/(const Matrix4x4& other) const;
  bool operator==(const Matrix4x4& other) const;
  
  // multiply
  Matrix4x4 operator*(const Matrix4x4& other) const;
  Vector4 operator*(const Vector4& other) const;

  static Matrix4x4 identity;

  /// @brief 주어진 좌표로 이동하도록 하는 행렬 수정
  void translate(float x, float y, float z);
  
  /// @brief 주어진 좌표로 변환한 벡터를 구함
  /// @param v 3차원 좌표
  /// @return 3차원 벡터
  Vector3 transform(const Vector3& v);
  
  /// @brief 주어진 좌표로 변환한 벡터를 구함
  /// @param v 4차원 좌표
  /// @return 4차원 벡터
  Vector4 transform4(const Vector3& v);

  /// @brief 주어진 각도로 행렬 회전
  void rotate(float x, float y, float z);

  void print() const;
};

namespace math {

/**
 * @brief 현재 카메라가 바라보고 있는 방향의 벡터를 반환
 * https://arienbv.org/blog/2017/07/30/breakdown-of-the-lookAt-function-in-OpenGL/
 * @return Matrix4x4 
 */
void lookAt(Matrix4x4& out, const Vector3& eye, const Vector3& at, const Vector3& up);

/**
 * @brief 원근 투영 프러스텀 매트릭스 반환
 * https://www.songho.ca/opengl/gl_projectionmatrix.html
 * @param camera 
 */
void perspectiveProject(Matrix4x4& out, float fovY, float aspect, float near, float far);

// https://www.songho.ca/opengl/gl_transform.html
// https://www.songho.ca/opengl/gl_viewport.html
// near, far는 기본값 각각 0, 1 사용
void viewport(Matrix4x4& out, float x, float y, float w, float h);

} // namespace math

} // namespace ssr
