// Nicholas Baldwin
// 2011/07/17
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Vector2.h"
#include "../FileIO/Serializer.h"

namespace Improbability
{

MakeMeta(Vector2);

void Vector2::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaFieldable);
  comp->MetaGetComponent(MetaFieldable)->MetaRegisterField(float, m_x);
  comp->MetaGetComponent(MetaFieldable)->MetaRegisterField(float, m_y);
}

void Vector2::Deserialize(Serializer &serializer)
{
  Read(serializer, m_x, "X");
  Read(serializer, m_y, "Y");
}

float Vector2::Dot(Vector2 const &rhs) const
{
  return m_x * rhs.m_x + m_y * rhs.m_y;
}

Vector2 Vector2::Project(Vector2 const &rhs) const
{
  Vector2 b = rhs.Normalized();
  return b * Dot(b);
}

void Vector2::Normalize()
{
  float mag = Magnitude();

  m_x /= mag;
  m_y /= mag;
}

Vector2 Vector2::Normalized() const
{
  float mag = Magnitude();

  return Vector2(m_x / mag, m_y / mag);
}

float Vector2::Magnitude() const
{
  return std::sqrt(MagnitudeSquared());
}

float Vector2::MagnitudeSquared() const
{
  return m_x * m_x + m_y * m_y;
}

float Vector2::Distance(Vector2 const &rhs) const
{
  return std::sqrt(DistanceSquared(rhs));
}

float Vector2::DistanceSquared(Vector2 const &rhs) const
{
  float dx = m_x - rhs.m_x, dy = m_y - rhs.m_y;

  return dx * dx + dy * dy;
}

Vector2 Vector2::operator+(Vector2 const &rhs) const
{
  return Vector2(m_x + rhs.m_x, m_y + rhs.m_y);
}

Improbability::Vector2 Vector2::operator-(Vector2 const &rhs) const
{
  return Vector2(m_x - rhs.m_x, m_y - rhs.m_y);
}

Improbability::Vector2 Vector2::operator-() const
{
  return Vector2(-m_x, -m_y);
}

Improbability::Vector2 Vector2::operator*(float scale) const
{
  return Vector2(m_x * scale, m_y * scale);
}

}
