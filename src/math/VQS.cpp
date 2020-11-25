// Nicholas Baldwin
// 2011/10/16
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "VQS.h"

namespace Improbability
{

VQS::VQS(Vector3 v, Quaternion q, float s) : m_vector(v), m_scale(s), m_quaternion(q)
{}

VQS::VQS(Vector4 v, Quaternion q, float s) : m_vector(v.ToVector3()), m_scale(s), m_quaternion(q)
{}

VQS::VQS(Matrix4 const &m)
{
  FromMatrix(m);
}

VQS VQS::operator+(VQS const &rhs) const
{
  VQS vqs(*this);
  vqs += rhs;
  return vqs;
}

VQS& VQS::operator+=(VQS const &rhs)
{
  m_vector += rhs.m_vector;
  m_quaternion += rhs.m_quaternion;
  m_scale += rhs.m_scale;
  return *this;
}

Vector3 VQS::operator*(Vector3 rhs) const
{
  return m_quaternion.Rotate(rhs * m_scale) + m_vector;
}

Vector4 VQS::operator*(Vector4 rhs) const 
{
  Vector3 v(*this * rhs.ToVector3());
  return Vector4(v.m_x, v.m_y, v.m_z, rhs.m_w);
}

VQS VQS::operator*(float rhs) const
{
  VQS vqs;
  vqs.m_vector = vqs.m_vector * rhs;
  vqs.m_quaternion = vqs.m_quaternion * rhs;
  vqs.m_scale *= rhs;
  return vqs;
}

VQS VQS::operator*(VQS const &rhs) const
{
  VQS vqs;
  vqs.m_vector = *this * rhs.m_vector;
  vqs.m_quaternion = m_quaternion * rhs.m_quaternion;
  vqs.m_scale = m_scale * rhs.m_scale;
  return vqs;
}

void VQS::Identity()
{
  m_vector.m_x = 0.0f;
  m_vector.m_y = 0.0f;
  m_vector.m_z = 0.0f;

  m_quaternion.Identity();

  m_scale = 1.0f;
}

void VQS::Inverse()
{
  m_scale = 1.0f / m_scale;
  m_quaternion.Inverse();
  m_vector = m_quaternion.Rotate(-m_vector * m_scale);
}

VQS VQS::Inversed()
{
  VQS vqs(*this);
  vqs.Inverse();
  return vqs;
}

Matrix4 VQS::ToMatrix()
{
  Matrix4 m;
  m = m_quaternion.ToMatrix();
  m = m * Matrix4::CreateScaling(m_scale);
  m = m + Matrix4::CreateTranslation(Vector4::Vector(m_vector));
  return m;
}

void VQS::FromMatrix(Matrix4 const &m)
{
  m_quaternion.FromMatrix(m);
  m_vector.m_x = m.m_03;
  m_vector.m_y = m.m_13;
  m_vector.m_z = m.m_23;
  m_scale = 1.0f;
}

}