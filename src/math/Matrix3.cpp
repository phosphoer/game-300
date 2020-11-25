// Nicholas Baldwin
// 2011/07/17
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Matrix3.h"
#include "Matrix2.h"

namespace Improbability
{

Matrix3::Matrix3(Matrix2 const &original)
{
  m_00 = original.m_00; m_01 = original.m_01; m_02 = 0.0f;
  m_10 = original.m_10; m_11 = original.m_11; m_12 = 0.0f;
  m_20 = 0.0f;          m_21 = 0.0f;          m_22 = 1.0f;
}

Improbability::Vector3 Matrix3::GetRow(int row)
{
  return Vector3(m_a[0 + 3 * row], m_a[1 + 3 * row], m_a[2 + 3 * row]);
}

Improbability::Vector3 Matrix3::operator*(Vector3 const &rhs)
{
  Vector3 result;

  result.m_x = m_00 * rhs.m_x + m_01 * rhs.m_y + m_02 * rhs.m_z;
  result.m_y = m_10 * rhs.m_x + m_11 * rhs.m_y + m_12 * rhs.m_z;
  result.m_z = m_20 * rhs.m_x + m_21 * rhs.m_y + m_22 * rhs.m_z;

  return result;
}

Improbability::Matrix3 Matrix3::operator*(Matrix3 const &rhs)
{
  Matrix3 result;

  result.m_00 = m_00 * rhs.m_00 + m_01 * rhs.m_10 + m_02 * rhs.m_20;
  result.m_01 = m_00 * rhs.m_01 + m_01 * rhs.m_11 + m_02 * rhs.m_21;
  result.m_02 = m_00 * rhs.m_02 + m_01 * rhs.m_12 + m_02 * rhs.m_22;

  result.m_10 = m_10 * rhs.m_00 + m_11 * rhs.m_10 + m_12 * rhs.m_20;
  result.m_11 = m_10 * rhs.m_01 + m_11 * rhs.m_11 + m_12 * rhs.m_21;
  result.m_12 = m_10 * rhs.m_02 + m_11 * rhs.m_12 + m_12 * rhs.m_22;

  result.m_20 = m_20 * rhs.m_00 + m_21 * rhs.m_10 + m_22 * rhs.m_20;
  result.m_21 = m_20 * rhs.m_01 + m_21 * rhs.m_11 + m_22 * rhs.m_21;
  result.m_22 = m_20 * rhs.m_02 + m_21 * rhs.m_12 + m_22 * rhs.m_22;

  return result;
}

Improbability::Matrix3 Matrix3::operator+(Matrix3 const &rhs)
{
  Matrix3 result;
  for (int i = 0; i < 9; ++i)
    result.m_a[i] = m_a[i] + rhs.m_a[i];
    
  return result;
}

void Matrix3::Identity()
{
  m_00 = 1.0f; m_01 = 0.0f; m_02 = 0.0f;
  m_10 = 0.0f; m_11 = 1.0f; m_12 = 0.0f;
  m_20 = 0.0f; m_21 = 0.0f; m_22 = 1.0f;
}

void Matrix3::Scale(Vector3 const &scale)
{
  Scale(scale.m_x, scale.m_y, scale.m_z);
}

void Matrix3::Scale(float x, float y, float z)
{
  m_00 = x;
  m_11 = y;
  m_22 = z;
}

void Matrix3::Scale(float val)
{
  Scale(val, val, val);
}

void Matrix3::RotateX(float radians)
{
  float cosine = std::cos(radians);
  float sine = std::sin(radians);

  m_11 = cosine; m_12  = -sine;
  m_21 = sine; m_22 =  cosine;
}

void Matrix3::RotateY(float radians)
{
  float cosine = std::cos(radians);
  float sine = std::sin(radians);

  m_00 = cosine; m_02  = sine;
  m_20 = -sine;  m_22 =  cosine;
}

void Matrix3::RotateZ(float radians)
{
  float cosine = std::cos(radians);
  float sine = std::sin(radians);

  m_00 = cosine; m_01  = -sine;
  m_10 = sine;   m_11 =  cosine;
}

void Matrix3::Rotate(Vector3 const &axis, float angle)
{
  Matrix3 a, tensor, b, cross_mat, c;

  float cosine = std::cos(angle);
  float sine = std::sin(angle);

  a.Identity();
  a.Scale(cosine);

  tensor.Identity();
  tensor.Tensor(axis, axis);

  b.Identity();
  b.Scale(1.0f - cosine);

  cross_mat.Identity();
  cross_mat.CrossProduct(axis);

  c.Identity();
  c.Scale(sine);

  *this = (a) + (b * tensor) + (c * cross_mat);
}

void Matrix3::AffineTranslate(Vector3 const &pos)
{
  AffineTranslate(pos.m_x, pos.m_y);
}

void Matrix3::AffineTranslate(float x, float y)
{
  m_02 = x;
  m_12 = y;
}

void Matrix3::AffineScale(Vector3 const &scale)
{
  AffineScale(scale.m_x, scale.m_y);
}

void Matrix3::AffineScale(float x, float y)
{
  m_00 = x;
  m_11 = y;
}

void Matrix3::AffineScale(float val)
{
  AffineScale(val, val);
}

void Matrix3::AffineRotate(float radians)
{
  RotateZ(radians);
}

void Matrix3::Tensor(Vector3 const &vec1, Vector3 const &vec2)
{
  m_00 = vec1.m_x * vec2.m_x; m_01 = vec1.m_x * vec2.m_y; m_02 = vec1.m_x * vec2.m_z;
  m_10 = vec1.m_y * vec2.m_x; m_11 = vec1.m_y * vec2.m_y; m_12 = vec1.m_y * vec2.m_z;
  m_20 = vec1.m_z * vec2.m_x; m_21 = vec1.m_z * vec2.m_y; m_22 = vec1.m_z * vec2.m_z;
}

void Matrix3::CrossProduct(Vector3 const &vec)
{
  m_00 = 0.0f;     m_01 = -vec.m_z; m_02 = vec.m_y;
  m_10 = vec.m_z;  m_11 = 0.0f;     m_12 = -vec.m_x;
  m_20 = -vec.m_y; m_21 = vec.m_x;  m_22 = 0.0f;
}

void Matrix3::Transpose()
{
  float temp;
  for (int i = 1; i < 3; ++i)
  {
    for (int j = i; j < 3; ++j)
    {
      temp = m_a[j + i * 3];
      m_a[j + i * 3] = m_a[i + j * 3];
      m_a[i + j * 3] = temp;
    }
  }
}

void Matrix3::Inverse()
{
  double det;
  Matrix3 ret;

  ret.m_00 = m_11 * m_22 - m_12 * m_21;
  ret.m_10 = m_12 * m_20 - m_10 * m_22;
  ret.m_20 = m_10 * m_21 - m_11 * m_20;

  ret.m_01 = m_02 * m_21 - m_01 * m_22;
  ret.m_11 = m_00 * m_22 - m_02 * m_20;
  ret.m_21 = m_01 * m_20 - m_00 * m_21;

  ret.m_02 = m_01 * m_12 - m_02 * m_11;
  ret.m_12 = m_02 * m_10 - m_00 * m_12;
  ret.m_22 = m_00 * m_11 - m_01 * m_10;

  det = m_00 * ret.m_00 + m_01 * ret.m_10 + m_02 * ret.m_20;
  if (det == 0)
    return;

  det = 1.0 / det;

  for (int i = 0; i < 9; i++)
    m_a[i] = float(ret.m_a[i] * det);
}

Improbability::Matrix3 Matrix3::CreateScaling(Vector3 const &scale)
{
  Matrix3 new_matrix;
  new_matrix.Identity();

  new_matrix.Scale(scale);

  return new_matrix;
}

Improbability::Matrix3 Matrix3::CreateScaling(float x, float y, float z)
{
  Matrix3 new_matrix;
  new_matrix.Identity();

  new_matrix.Scale(x, y, z);

  return new_matrix;
}

Improbability::Matrix3 Matrix3::CreateScaling(float val)
{
  Matrix3 new_matrix;
  new_matrix.Identity();

  new_matrix.Scale(val);

  return new_matrix;
}

Improbability::Matrix3 Matrix3::CreateRotationX(float radians)
{
  Matrix3 new_matrix;
  new_matrix.Identity();

  new_matrix.RotateX(radians);

  return new_matrix;
}

Improbability::Matrix3 Matrix3::CreateRotationY(float radians)
{
  Matrix3 new_matrix;
  new_matrix.Identity();

  new_matrix.RotateY(radians);

  return new_matrix;
}

Improbability::Matrix3 Matrix3::CreateRotationZ(float radians)
{
  Matrix3 new_matrix;
  new_matrix.Identity();

  new_matrix.RotateZ(radians);

  return new_matrix;
}

Improbability::Matrix3 Matrix3::CreateRotation(Vector3 const &axis, float angle)
{
  Matrix3 new_matrix;
  new_matrix.Identity();

  new_matrix.Rotate(axis, angle);

  return new_matrix;
}

Improbability::Matrix3 Matrix3::CreateAffineTranslation(Vector3 const &pos)
{
  Matrix3 new_matrix;
  new_matrix.Identity();

  new_matrix.AffineTranslate(pos);

  return new_matrix;
}

Improbability::Matrix3 Matrix3::CreateAffineTranslation(float x, float y)
{
  Matrix3 new_matrix;
  new_matrix.Identity();

  new_matrix.AffineTranslate(x, y);

  return new_matrix;
}

Improbability::Matrix3 Matrix3::CreateAffineScaling(Vector3 const &scale)
{
  Matrix3 new_matrix;
  new_matrix.Identity();

  new_matrix.AffineScale(scale);

  return new_matrix;
}

Improbability::Matrix3 Matrix3::CreateAffineScaling(float x, float y)
{
  Matrix3 new_matrix;
  new_matrix.Identity();

  new_matrix.AffineScale(x, y);

  return new_matrix;
}

Improbability::Matrix3 Matrix3::CreateAffineScaling(float val)
{
  Matrix3 new_matrix;
  new_matrix.Identity();

  new_matrix.AffineScale(val);

  return new_matrix;
}

Improbability::Matrix3 Matrix3::CreateAffineRotation(float radians)
{
  Matrix3 new_matrix;
  new_matrix.Identity();

  new_matrix.AffineRotate(radians);

  return new_matrix;
}

}