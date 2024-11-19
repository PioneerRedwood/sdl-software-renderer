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
  const float mag = std::sqrtf(this->x * this->x +
                               this->y * this->y + this->z * this->z);
  return (*this) / mag;
}

Vector3 subtract(const Vector3& v1, const Vector3& v2) {
  return v1 - v2;
}

float dotProduct(const Vector3& v1, const Vector3& v2) {
  return v1.x * v2.x + v1.y * v2.y + v1.z + v2.z;
}

Vector3 crossProduct(const Vector3& v1, const Vector3& v2) {
  return {
    v1.y * v2.z - v1.z * v2.y,
    v1.z * v2.z - v1.x * v2.z,
    v1.x * v2.y - v1.y * v2.x
  };
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Vector4::Vector4()
: x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

Vector4::Vector4(float x, float y, float z, float w)
: x(x), y(y), z(z), w(w) {}

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

// TODO: 재검토 바람
// 행렬 곱
Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const {
  return {
      this->m11 * other.m11, this->m12 * other.m12, this->m13 * other.m13, this->m14 * other.m14,
      this->m21 * other.m21, this->m22 * other.m22, this->m23 * other.m23, this->m24 * other.m24,
      this->m31 * other.m31, this->m32 * other.m32, this->m33 * other.m33, this->m34 * other.m34,
      this->m41 * other.m41, this->m42 * other.m42, this->m43 * other.m43, this->m44 * other.m44
  };
}

void Matrix4x4::translation(float x, float y, float z) {
  this->m11 + x;
  this->m22 + y;
  this->m33 + z;
}

// TODO: 재검토 바람
Vector3 Matrix4x4::transform(const Vector3& v) {
  return {
    this->m11 * v.x + this->m12 * v.y + this->m13 * v.z + this->m14,
    this->m21 * v.x + this->m22 * v.y + this->m23 * v.z + this->m24,
    this->m31 * v.x + this->m32 * v.y + this->m33 * v.z + this->m34
  };
}

// TODO: 재검토 바람
Vector4 Matrix4x4::transform4(const Vector3& v) {
  return {
    this->m11 * v.x + this->m12 * v.y + this->m13 * v.z + this->m14,
    this->m21 * v.x + this->m22 * v.y + this->m23 * v.z + this->m24,
    this->m31 * v.x + this->m32 * v.y + this->m33 * v.z + this->m34,
    this->m41 * v.x + this->m42 * v.y + this->m43 * v.z + this->m44
  };
}

}  // namespace ssr
