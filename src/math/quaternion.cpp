// Nicholas Baldwin
// 2011/05/18
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Quaternion.h"

namespace Improbability
{

Quaternion::Quaternion(float angle, Vector3 axis):
  m_s(std::cos(angle * 0.5f)),
  m_x(axis.m_x * std::sin(angle * 0.5f)),
  m_y(axis.m_y * std::sin(angle * 0.5f)),
  m_z(axis.m_z * std::sin(angle * 0.5f))
{
}

Quaternion::Quaternion(Vector3 rhs):
  m_s(0),
  m_x(rhs.m_x),
  m_y(rhs.m_y),
  m_z(rhs.m_z)
{
}

Quaternion::Quaternion(Matrix4 rhs)
{
  FromMatrix(rhs);
}

Quaternion Quaternion::operator*(float scalar) const
{
  Quaternion q;
  q.m_s = m_s * scalar;
  q.m_x = m_x * scalar;
  q.m_y = m_y * scalar;
  q.m_z = m_z * scalar;
  return q;
}

Quaternion Quaternion::operator*(Quaternion rhs) const
{
  Quaternion q;
  q.m_s = m_s * rhs.m_s - m_x * rhs.m_x - m_y * rhs.m_y - m_z * rhs.m_z;
  q.m_x = m_s * rhs.m_x + m_x * rhs.m_s + m_y * rhs.m_z - m_z * rhs.m_y;
  q.m_y = m_s * rhs.m_y - m_x * rhs.m_z + m_y * rhs.m_s + m_z * rhs.m_x;
  q.m_z = m_s * rhs.m_z + m_x * rhs.m_y - m_y * rhs.m_x + m_z * rhs.m_s;
  return q;
}

Quaternion Quaternion::operator+(Quaternion rhs) const
{
  Quaternion q(*this);
  q += rhs;
  return q;
}

Quaternion& Quaternion::operator+=(Quaternion rhs)
{
  m_s += rhs.m_s;
  m_x += rhs.m_x;
  m_y += rhs.m_y;
  m_z += rhs.m_z;
  return *this;
}

float Quaternion::Dot(Quaternion rhs) const
{
  return m_s * rhs.m_s + m_x * rhs.m_x + m_y * rhs.m_y + m_z * rhs.m_z;
}

Vector3 Quaternion::Rotate(Vector3 rhs) const
{
  return (*this * Quaternion(rhs) * this->Inversed()).ToVector();
}

void Quaternion::Identity()
{
  m_s = 1.0f;
  m_x = 0.0f;
  m_y = 0.0f;
  m_z = 0.0f;
}

void Quaternion::FromMatrix(Matrix4 rhs)
{
  float trace = 1.0f + rhs.m_00 + rhs.m_11 + rhs.m_22;
  if (trace > 0.00001f)
  {
    float w = std::sqrt(trace) * 2.0f;
    m_s = w * 0.25f;
    m_x = (rhs.m_21 - rhs.m_12) / w;
    m_y = (rhs.m_02 - rhs.m_20) / w;
    m_z = (rhs.m_10 - rhs.m_01) / w;
  }
  else if (rhs.m_00 > rhs.m_11 && rhs.m_00 > rhs.m_22)
  {
    float w = std::sqrt(1.0f + rhs.m_00 - rhs.m_11 - rhs.m_22) * 2.0f;
    m_s = (rhs.m_21 - rhs.m_12) / w;
    m_x = w * 0.25f;
    m_y = (rhs.m_10 + rhs.m_01) / w;
    m_z = (rhs.m_02 + rhs.m_20) / w;
  }
  else if (rhs.m_11 > rhs.m_22)
  {
    float w = std::sqrt(1.0f + rhs.m_11 - rhs.m_00 - rhs.m_22) * 2.0f;
    m_s = (rhs.m_02 - rhs.m_20) / w;
    m_x = (rhs.m_10 + rhs.m_01) / w;
    m_y = w * 0.25f;
    m_z = (rhs.m_21 + rhs.m_12) / w;
  }
  else
  {
    float w = std::sqrt(1.0f + rhs.m_22 - rhs.m_00 - rhs.m_11) * 2.0f;
    m_s = (rhs.m_10 - rhs.m_01) / w;
    m_x = (rhs.m_02 + rhs.m_20) / w;
    m_y = (rhs.m_21 + rhs.m_12) / w;
    m_z = w * 0.25f;
  }
}

void Quaternion::FromAxisAngle(Vector3 axis, float angle)
{
  m_s = std::cos(angle * 0.5f);
  m_x = axis.m_x * std::sin(angle * 0.5f);
  m_y = axis.m_y * std::sin(angle * 0.5f);
  m_z = axis.m_z * std::sin(angle * 0.5f);
}

float Quaternion::Magnitude() const
{
  return std::sqrt(MagnitudeSquared());
}

float Quaternion::MagnitudeSquared() const
{
  return m_s * m_s + m_x * m_x + m_y * m_y + m_z * m_z;
}

void Quaternion::Normalize()
{
  float m = Magnitude();
  // Assert if m < kEpsilon
  ErrorIf(m < k_epsilon, "m cannot be less than epsilon.");

  m_s /= m;
  m_x /= m;
  m_y /= m;
  m_z /= m;
}

Quaternion Quaternion::Normalized() const
{
  Quaternion q(*this);
  q.Normalize();
  return q;
}

void Quaternion::Inverse()
{
  float mag_squared = MagnitudeSquared();

  m_s = m_s / mag_squared;
  m_x = -m_x / mag_squared;
  m_y = -m_y / mag_squared;
  m_z = -m_z / mag_squared;
}

Quaternion Quaternion::Inversed() const
{
  Quaternion q(*this);
  q.Inverse();
  return q;
}

Matrix4 Quaternion::ToMatrix() const
{
  Matrix4 m;
  m.Identity();

  m.m_00 = 1.0f - 2.0f * m_y * m_y - 2.0f * m_z * m_z;
  m.m_01 =        2.0f * m_x * m_y - 2.0f * m_z * m_s;
  m.m_02 =        2.0f * m_x * m_z + 2.0f * m_y * m_s;

  m.m_10 =        2.0f * m_x * m_y + 2.0f * m_z * m_s;
  m.m_11 = 1.0f - 2.0f * m_x * m_x - 2.0f * m_z * m_z;
  m.m_12 =        2.0f * m_y * m_z - 2.0f * m_x * m_s;

  m.m_20 =        2.0f * m_x * m_z - 2.0f * m_y * m_s;
  m.m_21 =        2.0f * m_y * m_z + 2.0f * m_x * m_s;
  m.m_22 = 1.0f - 2.0f * m_x * m_x - 2.0f * m_y * m_y;

  return m;
}

Vector3 Quaternion::ToVector() const
{
  return Vector3(m_x, m_y, m_z);
}

Vector3 Quaternion::GetAxis() const
{
  if (m_s > 1 - k_epsilon) 
  {
    // near 0 rotations will break the conversions
    return Vector3(m_x, m_y, m_z) * (1.0f / std::sqrt(1 - m_s * m_s));
  }
  else
  {
    // since its near 0 the angle doesn't matter much
    return Vector3(m_x, m_y, m_z);
  }
}

float Quaternion::GetAngle() const
{
  return 2.0f * std::acos(m_s);
}

}