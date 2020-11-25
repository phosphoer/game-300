// Nicholas Baldwin
// 2011/07/17
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Vector3.h"
#include "../FileIO/Serializer.h"

namespace Improbability
{

MakeMeta(Vector3);

void Vector3::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaFieldable);
  comp->MetaGetComponent(MetaFieldable)->MetaRegisterField(float, m_x);
  comp->MetaGetComponent(MetaFieldable)->MetaRegisterField(float, m_y);
  comp->MetaGetComponent(MetaFieldable)->MetaRegisterField(float, m_z);
}

void Vector3::Deserialize(Serializer &serializer)
{
  Read(serializer, m_x, "X");
  Read(serializer, m_y, "Y");
  Read(serializer, m_z, "Z");
}

Vector3 Vector3::Cross(Vector3 const &rhs) const
{
  Vector3 result;

  result.m_x = m_y * rhs.m_z - m_z * rhs.m_y;
  result.m_y = m_z * rhs.m_x - m_x * rhs.m_z;
  result.m_z = m_x * rhs.m_y - m_y * rhs.m_x;

  return result;
}

float Vector3::Dot(Vector3 const &rhs) const
{
  return m_x * rhs.m_x + m_y * rhs.m_y + m_z * rhs.m_z;
}

Vector3 Vector3::Project(Vector3 const &rhs) const
{
  Vector3 b = rhs.Normalized();
  return b * Dot(b);
}

void Vector3::Normalize()
{
  float mag = Magnitude();

  m_x /= mag;
  m_y /= mag;
  m_z /= mag;
}

Vector3 Vector3::Normalized() const
{
  float mag = Magnitude();

  return Vector3(m_x / mag, m_y / mag, m_z / mag);
}

float Vector3::Magnitude() const
{
  return std::sqrt(MagnitudeSquared());
}

float Vector3::MagnitudeSquared() const
{
  return m_x * m_x + m_y * m_y + m_z * m_z;
}

float Vector3::Distance(Vector3 const &rhs) const
{
  return std::sqrt(DistanceSquared(rhs));
}

float Vector3::DistanceSquared(Vector3 const &rhs) const
{
  float dx = m_x - rhs.m_x, dy = m_y - rhs.m_y, dz = m_z - rhs.m_z;

  return dx * dx + dy * dy + dz * dz;
}

void Vector3::AffineNormalize()
{
  float mag = AffineMagnitude();

  m_x /= mag;
  m_y /= mag;
}

Vector3 Vector3::AffineNormalized() const
{
  float mag = AffineMagnitude();

  return Vector3(m_x / mag, m_y / mag, m_z);
}

float Vector3::AffineMagnitude() const
{
  return std::sqrt(AffineMagnitudeSquared());
}

float Vector3::AffineMagnitudeSquared() const
{
  return m_x * m_x + m_y * m_y;
}

float Vector3::AffineDistance(Vector3 const &rhs) const
{
  return std::sqrt(AffineDistanceSquared(rhs));
}

float Vector3::AffineDistanceSquared(Vector3 const &rhs) const
{
  float dx = m_x - rhs.m_x, dy = m_y - rhs.m_y;

  return dx * dx + dy * dy;
}

void Vector3::operator+=(Vector3 const &rhs)
{
  m_x += rhs.m_x;
  m_y += rhs.m_y;
  m_z += rhs.m_z;
}

void Vector3::operator-=(Vector3 const &rhs)
{
  m_x -= rhs.m_x;
  m_y -= rhs.m_y;
  m_z -= rhs.m_z;
}

Vector3 Vector3::operator+(Vector3 const &rhs) const
{
  return Vector3(m_x + rhs.m_x, m_y + rhs.m_y, m_z + rhs.m_z);
}

Vector3 Vector3::operator-(Vector3 const &rhs) const
{
  return Vector3(m_x - rhs.m_x, m_y - rhs.m_y, m_z - rhs.m_z);
}

Vector3 Vector3::operator-() const
{
  return Vector3(-m_x, -m_y, -m_z);
}

Vector3 Vector3::operator*(float scale) const
{
  return Vector3(m_x * scale, m_y * scale, m_z * scale);
}

}