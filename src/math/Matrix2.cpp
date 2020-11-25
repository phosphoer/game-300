// Nicholas Baldwin
// 2011/07/17
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Matrix2.h"

namespace Improbability
{

Vector2 Matrix2::GetRow(int row)
{
  return Vector2(m_a[0 + 2 * row], m_a[1 + 2 * row]);
}

Vector2 Matrix2::operator*(Vector2 const &rhs)
{
  Vector2 result;

  result.m_x = m_00 * rhs.m_x + m_01 * rhs.m_y;
  result.m_y = m_10 * rhs.m_x + m_11 * rhs.m_y;

  return result;
}

Matrix2 Matrix2::operator*(Matrix2 const &rhs)
{
  Matrix2 result;

  result.m_00 = m_00 * rhs.m_00 + m_01 * rhs.m_10;
  result.m_01 = m_00 * rhs.m_01 + m_01 * rhs.m_11;

  result.m_10 = m_10 * rhs.m_00 + m_11 * rhs.m_10;
  result.m_11 = m_10 * rhs.m_01 + m_11 * rhs.m_11;

  return result;
}

Matrix2 Matrix2::operator+(Matrix2 const &rhs)
{
  Matrix2 result;
  for (int i = 0; i < 9; ++i)
    result.m_a[i] = m_a[i] + rhs.m_a[i];
    
  return result;
}

void Matrix2::Identity()
{
  m_00 = 1.0f; m_01 = 0.0f;
  m_10 = 0.0f; m_11 = 1.0f;
}

void Matrix2::Scale(Vector2 const &scale)
{
  Scale(scale.m_x, scale.m_y);
}

void Matrix2::Scale(float x, float y)
{
  m_00 = x;
  m_11 = y;
}

void Matrix2::Scale(float val)
{
  Scale(val, val);
}

void Matrix2::Rotate(float angle)
{
  float cosine = std::cos(angle);
  float sine = std::sin(angle);

  m_00 = cosine; m_01  = -sine;
  m_10 = sine;   m_11 =  cosine;
}

void Matrix2::Transpose()
{
  float temp = m_01;
  m_01 = m_10;
  m_10 = temp;
}

void Matrix2::Inverse()
{
  double det = m_00 * m_11 - m_10 * m_01;

  if (det == 0)
    return;

  det = 1.0 / det;

  float temp = m_00;
  m_00 = float(m_11 * det);
  m_11 = float(temp * det);
  m_10 = float(-m_10 * det);
  m_01 = float(-m_01 * det);
}

Matrix2 Matrix2::CreateScaling(Vector2 const &scale)
{
  Matrix2 new_matrix;
  new_matrix.Identity();

  new_matrix.Scale(scale);

  return new_matrix;
}

Matrix2 Matrix2::CreateScaling(float x, float y)
{
  Matrix2 new_matrix;
  new_matrix.Identity();

  new_matrix.Scale(x, y);

  return new_matrix;
}

Matrix2 Matrix2::CreateScaling(float val)
{
  Matrix2 new_matrix;
  new_matrix.Identity();

  new_matrix.Scale(val);

  return new_matrix;
}

Matrix2 Matrix2::CreateRotation(float angle)
{
  Matrix2 new_matrix;
  new_matrix.Identity();

  new_matrix.Rotate(angle);

  return new_matrix;
}



}

