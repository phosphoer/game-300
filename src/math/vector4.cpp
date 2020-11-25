// David Evans
// 2011/05/18
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Vector4.h"
#include "../FileIO/Serializer.h"

namespace Improbability
{

MakeMeta(Vector4);

void Vector4::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(zero);
  comp->MetaAddComponent(MetaFieldable);
  comp->MetaGetComponent(MetaFieldable)->MetaRegisterField(float, m_x);
  comp->MetaGetComponent(MetaFieldable)->MetaRegisterField(float, m_y);
  comp->MetaGetComponent(MetaFieldable)->MetaRegisterField(float, m_z);
  comp->MetaGetComponent(MetaFieldable)->MetaRegisterField(float, m_w);
}

void Vector4::Deserialize(Serializer &serializer)
{
  Read(serializer, m_x, "X");
  Read(serializer, m_y, "Y");
  Read(serializer, m_z, "Z");
  Read(serializer, m_w, "W");
}

Vector4 Vector4::Cross(Vector4 const &rhs) const
{
  return Vector4(m_y * rhs.m_z - m_z * rhs.m_y, m_z * rhs.m_x - m_x * rhs.m_z, m_x * rhs.m_y - m_y * rhs.m_x, 0.0f);
}

float Vector4::Dot(Vector4 const &rhs) const
{
  return m_x * rhs.m_x + m_y * rhs.m_y + m_z * rhs.m_z + m_w * rhs.m_w;
}

void Vector4::Normalize()
{
  float length = Magnitude();
  m_x /= length;
  m_y /= length;
  m_z /= length;
}

Vector4 Vector4::Normalized() const
{
  Vector4 vector(*this);
  vector.Normalize();
  return vector;
}

float Vector4::Magnitude() const
{
  return std::sqrt(MagnitudeSquared());
}

float Vector4::MagnitudeSquared() const
{
  return m_x * m_x + m_y * m_y + m_z * m_z;
}

float Vector4::Distance(Vector4 const &rhs) const
{
  return std::sqrt(DistanceSquared(rhs));
}

float Vector4::DistanceSquared(Vector4 const &rhs) const
{
  float dx = m_x - rhs.m_x, dy = m_y - rhs.m_y, dz = m_z - rhs.m_z;

  return dx * dx + dy * dy + dz * dz;
}

void Vector4::operator+=(Vector4 const &rhs)
{
  m_x += rhs.m_x;
  m_y += rhs.m_y;
  m_z += rhs.m_z;
  m_w += rhs.m_w;
}

void Vector4::operator+=(Vector3 const &rhs)
{
  m_x += rhs.m_x;
  m_y += rhs.m_y;
  m_z += rhs.m_z;
}

Vector4 Vector4::operator+(Vector4 const &rhs) const
{
  return Vector4(m_x + rhs.m_x, m_y + rhs.m_y, m_z + rhs.m_z, m_w + rhs.m_w);
}

Vector4 Vector4::operator+(Vector3 const &rhs) const
{
  return Vector4(m_x + rhs.m_x, m_y + rhs.m_y, m_z + rhs.m_z, m_w);
}

Vector4 Vector4::operator-(Vector4 const &rhs) const
{
  return Vector4(m_x - rhs.m_x, m_y - rhs.m_y, m_z - rhs.m_z, m_w - rhs.m_w);
}

Vector4 Vector4::operator-() const
{
  return Vector4(-m_x, -m_y, -m_z, -m_w);
}

Vector4 Vector4::operator*(float scale) const
{
  return Vector4(m_x * scale, m_y * scale, m_z * scale, m_w * scale);
}

void Vector4::zero(void)
{
  m_x = m_y = m_z = m_w = 0.0f;
}

}