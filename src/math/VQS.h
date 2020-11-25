// Nicholas Baldwin
// 2011/10/16
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "matrix4.h"
#include "quaternion.h"
#include "Vector3.h"
#include "vector4.h"

namespace Improbability
{

class VQS
{
public:
  // default constructor
  VQS(){}

  // construct from vector quaternion and uniform scale
  VQS(Vector3 v, Quaternion q, float s);
  VQS(Vector4 v, Quaternion q, float s);

  // construct from matrix, assumes scale is 1.0f
  VQS(Matrix4 const &m);

  // VQS addition
  VQS operator+(VQS const &rhs) const;

  // VQS addistion
  VQS& operator+=(VQS const &rhs);

  // transform Vector3 with VQS
  Vector3 operator*(Vector3 rhs) const;

  // transform Vector4 with VQS
  Vector4 operator*(Vector4 rhs) const;

  // concatenate VQS
  VQS operator*(VQS const &rhs) const;

  // scale VQS
  VQS VQS::operator*(float rhs) const;

  // create Identity VQS
  void Identity();

  // invert VQS
  void Inverse();

  // returns inverted VQS
  VQS Inversed();

  // returns matrix equivalent to VQS
  Matrix4 ToMatrix();

  // makes VQS from matrix, assumes scale is 1.0f
  void FromMatrix(Matrix4 const &m);

public:
  Vector3 m_vector;
  float m_scale;
  Quaternion m_quaternion;
};

}