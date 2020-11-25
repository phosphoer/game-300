// Nicholas Baldwin
// 2011/05/18
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "Vector4.h"
#include "Matrix4.h"

namespace Improbability
{

class Quaternion
{
public:
  // Default constructor
  Quaternion(){}

  // Constructs a rotating quaternion from an angle and an axis
  Quaternion(float angle, Vector3 axis);

  // Construct a rotatable quaternion from a vector
  Quaternion(Vector3 rhs);

  // Construct a quaternion from a matrix
  Quaternion(Matrix4 rhs);

  // Scale the quaternion
  Quaternion operator*(float scalar) const;

  // Quaternion multiplication
  Quaternion operator*(Quaternion rhs) const;

  // Component-wise addition
  Quaternion operator+(Quaternion rhs) const;

  // Component-wise addition
  Quaternion& operator+=(Quaternion rhs);

  // Dot product of a quaternion
  float Dot(Quaternion rhs) const;

  Vector3 Rotate(Vector3 rhs) const;

  // makes identity quaternion
  void Identity();

  // Sets values of quaternion from a matrix
  void FromMatrix(Matrix4 rhs);

  // Sets values of quaternion from an axis and angle
  void FromAxisAngle(Vector3 axis, float angle);

  // Calculate the magnitude of a quaternion (SLOW)
  float Magnitude() const;

  // Calculate the squared magnitude of a quaternion
  float MagnitudeSquared() const;

  // Normalize the quaternion
  void Normalize();

  // Return a normalized form of the quaternion
  Quaternion Normalized() const;

  // Inverse the quaternion
  void Inverse();

  // Return an inversed form of the quaternion
  Quaternion Inversed() const;

  // Convert the quaternion to a matrix
  Matrix4 ToMatrix() const;

  // Convert the pure quaternion to a vector
  Vector3 ToVector() const;

  // Get the axis that forms the rotation
  Vector3 GetAxis() const;

  // Get the angle that forms the rotation
  float GetAngle() const;

  union
  {
    struct {float m_x; float m_y; float m_z; float m_s;};
    float m_a[4];
  };
};

}