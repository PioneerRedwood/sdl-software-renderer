//------------------------------------------------------------------------------
// File: Camera.hpp
// Author: Chris Redwood
// Created: 2024-11-20
// License: MIT License
//------------------------------------------------------------------------------

#pragma once

#include "Math.hpp"

namespace ssr {

class Camera {
public:
  float m_aspect = 0.0f;
  float m_fov = 0.0f;

  Vector3 m_eye = { 0.0f, 0.0f, 0.0f };
  Vector3 m_at = { 0.0f, 0.0f, 1.0f };
  Vector3 m_up = { 0.0f, 1.0f, 0.0f };

  /**
   * @brief 현재 카메라가 바라보고 있는 방향의 벡터를 반환
   * https://arienbv.org/blog/2017/07/30/breakdown-of-the-lookAt-function-in-OpenGL/
   * @return Matrix4x4 
   */
  void lookAt(Matrix4x4& model) const {
    // 카메라가 바라보고 있는 방향을 가져옴
    // 하지만 이는 실제 바라보는 방향과 반대되는 방향임
    // 왼손 좌표계를 만들어놓고 나중에 오른손 좌표계로 하기 위해서는
    // z축을 기준으로 뒤집으면 됨 (이때는 왼손 좌표계)
    Vector3 zaxis = (m_at - m_eye).normalize();

    // 두 개의 다른 벡터 축 계산
    // 다음 두 벡터를 계산하기 위해서는 외적이 효과적
    
    //  1. 카메라가 바라보고 있는 방향과 카메라의 위 방향 
    // 벡터를 외적하여 x축을 구함. 이때 내적하고 난 뒤
    // 벡터 정규화를 거쳐야 함 up 벡터는 유닛 벡터가 아니기
    // 때문.
    Vector3 xaxis = math::crossProduct(zaxis, m_up).normalize();
    
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
    model.m11 = xaxis.x, model.m21 = xaxis.y, model.m31 = xaxis.z, model.m41 = -math::dotProduct(xaxis, m_eye);
    model.m12 = xaxis.x, model.m22 = xaxis.y, model.m32 = xaxis.z, model.m42 = -math::dotProduct(yaxis, m_eye);
    model.m13 = xaxis.x, model.m23 = xaxis.y, model.m33 = xaxis.z, model.m43 = -math::dotProduct(zaxis, m_eye);
    model.m41 = 0.0f, model.m21 = 0.0f, model.m31 = 0.0f, model.m41 = 1.0f;
  }
};


}