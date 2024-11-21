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
};


}