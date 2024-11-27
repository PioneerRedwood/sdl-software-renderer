//------------------------------------------------------------------------------
// File: Math.cpp
// Author: Chris Redwood
// Created: 2024-11-01
// License: MIT License
//------------------------------------------------------------------------------

#include "Math.hpp"

namespace ssr {

Vector2::Vector2() : x(0.0f), y(0.0f) {}

Vector2::Vector2(float x, float y) : x(x), y(y) {}

Vector2 Vector2::operator+(const Vector2& other) const {
  return { x + other.x, y + other.y };
}

Vector2 Vector2::operator*(float scalar) const {
  return { x * scalar, y * scalar };
}

Vector2& Vector2::operator=(const Vector2& other) {
  x = other.x;
  y = other.y;
  return *this;
}

Vector2 Vector2::operator-(const Vector2& other) const {
  return { x - other.x, y - other.y };
}

bool Vector2::operator==(const Vector2& other) const {
  return (x == other.x) && (y == other.y);
}

float Vector2::magnitude() const {
  return sqrtf(x * x + y * y);
}

Vector2 Vector2::normalized() const {
  float mag = magnitude();
  return { x / mag, y / mag };
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Vector3::Vector3() : x(0.0f), y(0.0f), z(0.0f) {}

Vector3::Vector3(float x, float y, float z)
: x(x), y(y), z(z) {}

Vector3::Vector3(const Vector3& other)
: x(other.x), y(other.y), z(other.z) {}

Vector3& Vector3::operator=(const Vector3& other) {
  this->x = other.x;
  this->y = other.y;
  this->z = other.z;
  return *this;
}

Vector3 Vector3::operator+(const Vector3& other) const {
  return {this->x + other.x, this->y + other.y, this->z + other.z};
}

Vector3 Vector3::operator*(float scalar) const {
  return { this->x * scalar, this->y * scalar, this->z * scalar };
}

Vector3 Vector3::operator-(const Vector3& other) const {
  return { this->x - other.x, this->y - other.y, this->z - other.z };
}

Vector3 Vector3::operator/(const Vector3& other) const {
  return { this->x / other.x, this->y / other.y, this->z / other.z };
}

Vector3 Vector3::operator/(float value) const {
  return { this->x / value, this->y / value, this->z / value };
}

bool Vector3::operator==(const Vector3& other) const {
  return (this->x == other.x && this->y == other.y && this->z == other.z);
}

Vector3 Vector3::normalize() const {
  const float mag = (float)std::sqrt(this->x * this->x + this->y * this->y +
                                     this->z * this->z);
  return (*this) / mag;
}

namespace math {

Vector3 subtract(const Vector3& v1, const Vector3& v2) {
  return v1 - v2;
}

float dotProduct(const Vector3& v1, const Vector3& v2) {
  return v1.x * v2.x + v1.y * v2.y + v1.z + v2.z;
}

Vector3 crossProduct(const Vector3& v1, const Vector3& v2) {
  return {
    v1.y * v2.z - v1.z * v2.y,
    v1.z * v2.x - v1.x * v2.z,
    v1.x * v2.y - v1.y * v2.z
  };
}

}

////////////////////////////////////////////////////////////////////////////////////////////////////

Vector4::Vector4()
: x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

Vector4::Vector4(float x, float y, float z, float w)
: x(x), y(y), z(z), w(w) {}

void Vector4::perspectiveDivide() {
  const float rhw = 1 / w;
  this->x = x * rhw;
  this->y = y * rhw;
  this->z = z * rhw;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Matrix4x4 Matrix4x4::identity = {
  1, 0, 0, 0,
  0, 1, 0, 0,
  0, 0, 1, 0,
  0, 0, 0, 1
};

Matrix4x4::Matrix4x4()
: m11(0.0f), m12(0.0f), m13(0.0f), m14(0.0f),
  m21(0.0f), m22(0.0f), m23(0.0f), m24(0.0f),
  m31(0.0f), m32(0.0f), m33(0.0f), m34(0.0f),
  m41(0.0f), m42(0.0f), m43(0.0f), m44(0.0f) {}

Matrix4x4::Matrix4x4(
  float m11, float m12, float m13, float m14,
  float m21, float m22, float m23, float m24,
  float m31, float m32, float m33, float m34,
  float m41, float m42, float m43, float m44 )
: m11(m11), m12(m12), m13(m13), m14(m14),
  m21(m21), m22(m22), m23(m23), m24(m24),
  m31(m31), m32(m32), m33(m33), m34(m34),
  m41(m41), m42(m42), m43(m43), m44(m44) {}

Matrix4x4::Matrix4x4(Vector4 x, Vector4 y, Vector4 z, Vector4 w)
: m11(x.x), m12(y.x), m13(z.x), m14(w.x),
  m21(x.y), m22(y.y), m23(z.y), m24(w.y),
  m31(x.z), m32(y.z), m33(z.z), m34(w.z),
  m41(x.w), m42(y.w), m43(z.w), m44(w.w) {}

Matrix4x4::Matrix4x4(const Vector4& x, const Vector4& y, 
                     const Vector4& z, const Vector4& w)
: m11(x.x), m12(y.x), m13(z.x), m14(w.x),
  m21(x.y), m22(y.y), m23(z.y), m24(w.y),
  m31(x.z), m32(y.z), m33(z.z), m34(w.z),
  m41(x.w), m42(y.w), m43(z.w), m44(w.w) {}

Matrix4x4::Matrix4x4(const Matrix4x4& other) {
  this->m11 = other.m11, this->m12 = other.m12, this->m13 = other.m13, this->m14 = other.m14;
  this->m21 = other.m21, this->m22 = other.m22, this->m23 = other.m23, this->m24 = other.m24;
  this->m31 = other.m31, this->m32 = other.m32, this->m33 = other.m33, this->m34 = other.m34;
  this->m41 = other.m41, this->m42 = other.m42, this->m43 = other.m43, this->m44 = other.m44;
}

Matrix4x4& Matrix4x4::operator=(const Matrix4x4& other) {
  this->m11 = other.m11, this->m12 = other.m12, this->m13 = other.m13, this->m14 = other.m14;
  this->m21 = other.m21, this->m22 = other.m22, this->m23 = other.m23, this->m24 = other.m24;
  this->m31 = other.m31, this->m32 = other.m32, this->m33 = other.m33, this->m34 = other.m34;
  this->m41 = other.m41, this->m42 = other.m42, this->m43 = other.m43, this->m44 = other.m44;
  return *this;
}

Matrix4x4 Matrix4x4::operator+(const Matrix4x4& other) const {
  return {
      this->m11 + other.m11, this->m12 + other.m12, this->m13 + other.m13, this->m14 + other.m14,
      this->m21 + other.m21, this->m22 + other.m22, this->m23 + other.m23, this->m24 + other.m24,
      this->m31 + other.m31, this->m32 + other.m32, this->m33 + other.m33, this->m34 + other.m34,
      this->m41 + other.m41, this->m42 + other.m42, this->m43 + other.m43, this->m44 + other.m44
  };
}

Matrix4x4 Matrix4x4::operator*(float scalar) const {
  return {
      this->m11 * scalar, this->m12 * scalar, this->m13 * scalar, this->m14 * scalar,
      this->m21 * scalar, this->m22 * scalar, this->m23 * scalar, this->m24 * scalar,
      this->m31 * scalar, this->m32 * scalar, this->m33 * scalar, this->m34 * scalar,
      this->m41 * scalar, this->m42 * scalar, this->m43 * scalar, this->m44 * scalar
  };
}

Vector4 Matrix4x4::operator*(const Vector4& other) const {
  return {
    this->m11 * other.x + this->m12 * other.y + this->m13 * other.z + this->m14 * other.w,
    this->m21 * other.x + this->m22 * other.y + this->m23 * other.z + this->m24 * other.w,
    this->m31 * other.x + this->m32 * other.y + this->m33 * other.z + this->m34 * other.w,
    this->m41 * other.x + this->m42 * other.y + this->m43 * other.z + this->m44 * other.w
  };
}

Matrix4x4 Matrix4x4::operator-(const Matrix4x4& other) const {
  return {
      this->m11 - other.m11, this->m12 - other.m12, this->m13 - other.m13, this->m14 - other.m14,
      this->m21 - other.m21, this->m22 - other.m22, this->m23 - other.m23, this->m24 - other.m24,
      this->m31 - other.m31, this->m32 - other.m32, this->m33 - other.m33, this->m34 - other.m34,
      this->m41 - other.m41, this->m42 - other.m42, this->m43 - other.m43, this->m44 - other.m44
  };
}

Matrix4x4 Matrix4x4::operator/(const Matrix4x4& other) const {
  return {
      this->m11 / other.m11, this->m12 / other.m12, this->m13 / other.m13, this->m14 / other.m14,
      this->m21 / other.m21, this->m22 / other.m22, this->m23 / other.m23, this->m24 / other.m24,
      this->m31 / other.m31, this->m32 / other.m32, this->m33 / other.m33, this->m34 / other.m34,
      this->m41 / other.m41, this->m42 / other.m42, this->m43 / other.m43, this->m44 / other.m44
  };
}

bool Matrix4x4::operator==(const Matrix4x4& other) const {
  return (
      this->m11 == other.m11 && this->m12 == other.m12 && this->m13 == other.m13 && this->m14 + other.m14 &&
      this->m21 == other.m21 && this->m22 == other.m22 && this->m23 == other.m23 && this->m24 + other.m24 &&
      this->m31 == other.m31 && this->m32 == other.m32 && this->m33 == other.m33 && this->m34 + other.m34 &&
      this->m41 == other.m41 && this->m42 == other.m42 && this->m43 == other.m43 && this->m44 + other.m44
  );
}

/*
  행렬 곱
  +-----------------+   +-----------------+   +-----------------+  
  | m11 m12 m13 m14 |   | n11 n12 n13 n14 |   | r11 r12 r13 r14 |  
  | m21 m22 m23 m24 | X | n21 n22 n23 n24 | = | r21 r22 r23 r24 |  
  | m31 m32 m33 m34 |   | n31 n32 n33 n34 |   | r31 r32 r33 r34 |  
  | m41 m42 m43 m44 |   | n41 n42 n43 n44 |   | r41 r42 r43 r44 |  
  +-----------------+   +-----------------+   +-----------------+  
  ...
*/
Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const {
  return {
    // 첫 번째 행
    this->m11 * other.m11 + this->m12 * other.m21 + this->m13 * other.m31 + this->m14 * other.m41,
    this->m11 * other.m12 + this->m12 * other.m22 + this->m13 * other.m32 + this->m14 * other.m42,
    this->m11 * other.m13 + this->m12 * other.m23 + this->m13 * other.m33 + this->m14 * other.m43,
    this->m11 * other.m14 + this->m12 * other.m24 + this->m13 * other.m34 + this->m14 * other.m44,

    // 두 번째 행
    this->m21 * other.m11 + this->m22 * other.m21 + this->m23 * other.m31 + this->m24 * other.m41,
    this->m21 * other.m12 + this->m22 * other.m22 + this->m23 * other.m32 + this->m24 * other.m42,
    this->m21 * other.m13 + this->m22 * other.m23 + this->m23 * other.m33 + this->m24 * other.m43,
    this->m21 * other.m14 + this->m22 * other.m24 + this->m23 * other.m34 + this->m24 * other.m44,

    // 세 번째 행
    this->m31 * other.m11 + this->m32 * other.m21 + this->m33 * other.m31 + this->m34 * other.m41,
    this->m31 * other.m12 + this->m32 * other.m22 + this->m33 * other.m32 + this->m34 * other.m42,
    this->m31 * other.m13 + this->m32 * other.m23 + this->m33 * other.m33 + this->m34 * other.m43,
    this->m31 * other.m14 + this->m32 * other.m24 + this->m33 * other.m34 + this->m34 * other.m44,

    // 네 번째 행
    this->m41 * other.m11 + this->m42 * other.m21 + this->m43 * other.m31 + this->m44 * other.m41,
    this->m41 * other.m12 + this->m42 * other.m22 + this->m43 * other.m32 + this->m44 * other.m42,
    this->m41 * other.m13 + this->m42 * other.m23 + this->m43 * other.m33 + this->m44 * other.m43,
    this->m41 * other.m14 + this->m42 * other.m24 + this->m43 * other.m34 + this->m44 * other.m44
  };
}

/*
  이동 
  +-----------------+
  | x.x y.x z.x w.x |
  | x.y y.y z.y w.y |
  | x.z y.z z.z w.z |
  | x.w y.w z.w w.w |
  +-----------------+
  m11(x.x) + x, m22(y.y) + y, m33(z.z) + z
*/
void Matrix4x4::translate(float x, float y, float z) {
  this->m11 += x;
  this->m22 += y;
  this->m33 += z;
}

/*
  변환
  +-----------------+   +---+
  | m11 m12 m13 m14 |   | x |
  | m21 m22 m23 m24 | X | y |
  | m31 m32 m33 m34 |   | z |
  | m41 m42 m43 m44 |   | 1 |
  +-----------------+   +---+
  = (m11 * x + m21 * y + m31 * z + m41 * 1, ... )
  = (m12 * x + m22 * y + m32 * z + m42 * 1, ... )
  = (m13 * x + m23 * y + m33 * z + m43 * 1, ... )
*/
Vector3 Matrix4x4::transform(const Vector3& v) {
  return {
    this->m11 * v.x + this->m12 * v.y + this->m13 * v.z + this->m41,
    this->m21 * v.x + this->m22 * v.y + this->m23 * v.z + this->m42,
    this->m31 * v.x + this->m32 * v.y + this->m33 * v.z + this->m43
  };
}

/*
  변환
  +-----------------+   +---+
  | m11 m12 m13 m14 |   | x |
  | m21 m22 m23 m24 | X | y | 
  | m31 m32 m33 m34 |   | z |
  | m41 m42 m43 m44 |   | 1 |
  +-----------------+   +---+
  = (m11 * x + m21 * y + m31 * z + m41 * 1, ... )
  = (m12 * x + m22 * y + m32 * z + m42 * 1, ... )
  = (m13 * x + m23 * y + m33 * z + m43 * 1, ... )
  = (m14 * x + m24 * y + m34 * z + m44 * 1, ... )
*/
Vector4 Matrix4x4::transform4(const Vector3& v) {
  return {
    this->m11 * v.x + this->m12 * v.y + this->m13 * v.z + this->m14,
    this->m21 * v.x + this->m22 * v.y + this->m23 * v.z + this->m24,
    this->m31 * v.x + this->m32 * v.y + this->m33 * v.z + this->m34,
    this->m41 * v.x + this->m42 * v.y + this->m43 * v.z + this->m44
  };
}

void Matrix4x4::rotate(float x, float y, float z) {
  const float DEG2RAD = acos(-1.0f) / 180;

}

void Matrix4x4::print() const {
  const char* log = R"(
    [ %6.1f %6.1f %6.1f %6.1f ]
    [ %6.1f %6.1f %6.1f %6.1f ]
    [ %6.1f %6.1f %6.1f %6.1f ]
    [ %6.1f %6.1f %6.1f %6.1f ]
  )";
  printf(log, m11, m12, m13, m14, 
              m21, m22, m23, m24, 
              m31, m32, m33, m34, 
              m41, m42, m43, m44);
}

namespace math {

void setupViewMatrix(Matrix4x4& model, const Vector3& eye, const Vector3& at, const Vector3& up) {
  // eye는 카메라의 위치, at은 카메라가 바라보고 있는 방향, up은 카메라 기준 위 방향
  
  // Z축을 가져오기 위해서 카메라가 바라보고 있는 방향에서 
  // 현재 카메라의 위치 벡터를 뺀 벡터의 유닛 벡터를 구함

  // 카메라가 바라보고 있는 방향을 가져옴
  // 하지만 이는 실제 바라보는 방향과 반대되는 방향임
  // 왼손 좌표계를 만들어놓고 나중에 오른손 좌표계로 하기 위해서는
  // z축을 기준으로 뒤집으면 됨 (이때는 왼손 좌표계)
  Vector3 zaxis = (at - eye).normalize();

  // 두 개의 다른 벡터 축 계산
  // 다음 두 벡터를 계산하기 위해서는 외적이 효과적
  
  //  1. 카메라가 바라보고 있는 방향과 카메라의 위 방향 
  // 벡터를 외적하여 x축을 구함. 이때 내적하고 난 뒤
  // 벡터 정규화를 거쳐야 함 up 벡터는 유닛 벡터가 아니기
  // 때문.
  Vector3 xaxis = math::crossProduct(zaxis, up).normalize();
  
  //  2. 구한 x축과 z축을 외적하여 y축 구하기
  Vector3 yaxis = math::crossProduct(xaxis, zaxis);

  /*
  +-----------------+
  | m11 m12 m13 m14 |
  | m21 m22 m23 m24 |
  | m31 m32 m33 m34 |
  | m41 m42 m43 m44 |
  +-----------------+
  */
  // 위 세 개의 축을 구한 뒤 주어진 모델 좌표계를
  // 카메라 좌표계로 이동하려면 어파인 변환을 거쳐야 함
  // 어파인 변환은 이동과 회전으로 나뉘며 이를 적용할 때
  // 이들을 분리하여 변환 행렬의 역행렬을 구하기 쉽도록 함
  // X, Y, Z, 순으로 채우고 // W 쪽 채우기
  model.m11 = xaxis.x;   model.m12 = yaxis.x;     model.m13 = zaxis.x;    model.m14 = -math::dotProduct(xaxis, eye);
  model.m21 = xaxis.y;   model.m22 = yaxis.y;     model.m23 = zaxis.y;    model.m24 = -math::dotProduct(yaxis, eye);
  model.m31 = xaxis.z;   model.m32 = yaxis.z;     model.m33 = zaxis.z;    model.m34 = -math::dotProduct(zaxis, eye);
  model.m41 = 0.0f;      model.m42 = 0.0f;        model.m43 = 0.0f;       model.m44 = 1.0f;
}

void setupPerspectiveProjectionMatrix(Matrix4x4& out, float fovY, float aspect, float near, float far) {
  // 원근 투영 
  // 카메라 -> 클립 영역
  // 가로 세로 비율이 대칭인 것을 전제로 함

  const float DEG2RAD = acos(-1.0f) / 180;

  // fovY 절반 탄젠트 값
  float tangent = tan(fovY/2 * DEG2RAD);

  // near 평면의 절반 높이 및 너비
  float top = near * tangent, right = top * aspect;
  
  /*
  +-----------------+ +-----------------+
  | m11  0   0   0  | | n/r  0   0   0  |
  |  0  m22  0   0  | |  0  n/t  0   0  |
  |  0   0  m33 m34 | |  0   0  m33 m34 |
  |  0   0  m43 m44 | |  0   0   -1  0  |
  +-----------------+ +-----------------+
  m11 = near / right
  m22 = near / top
  m33 = -(far + near) / (far - near)
  m34 = -(2 * far * near) / (far - near)
  */
  out.m11 = near / right;
  out.m22 = near / top;
  out.m33 = -(far + near) / (far - near);
  out.m34 = -(2 * far * near) / (far - near);
  out.m43 = -1.0f;
  out.m44 = 0.0f;
}

void setupViewportMatrix(Matrix4x4& out, float x, float y, float w, float h) {
  /*
  +-----------------+ +-----------------+
  | m11  0   0  m14 | | w/2  0   0   0  |
  |  0  m22  0  m24 | |  0  h/2  0   0  |
  |  0   0  m33 m34 | |  0   0  m33 m34 |
  |  0   0   0   1  | |  0   0  m43  1  |
  +-----------------+ +-----------------+
  */
  out.m11 = w / 2;
  out.m14 = x + w / 2;
  out.m22 = h / 2;
  out.m24 = y + h / 2;
  out.m33 = 0.5f;
  out.m34 = 0.5f;
  out.m44 = 1.0f;
}

}

}  // namespace ssr
