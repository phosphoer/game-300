// David Evans
// 2011/05/18
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Matrix4.h"
#include "Matrix3.h"

namespace Improbability
{

Matrix4::Matrix4(Matrix3 const &original)
{
  m_00 = original.m_00; m_01 = original.m_01; m_02 = original.m_02; m_03 = 0.0f;
  m_10 = original.m_10; m_11 = original.m_11; m_12 = original.m_12; m_13 = 0.0f;
  m_20 = original.m_20; m_21 = original.m_21; m_22 = original.m_22; m_23 = 0.0f;
  m_30 = 0.0f;          m_31 = 0.0f;          m_32 = 0.0f;          m_33 = 1.0f;
}

Vector4 Matrix4::GetRow(int row)
{
  return Vector4(m_a[0 + 4 * row], m_a[1 + 4 * row], m_a[2 + 4 * row], m_a[3 + 4 * row]);
}

Vector4 Matrix4::operator*(Vector4 const &rhs)
{
  // Create a new vector from the original
  Vector4 result;
  
  // Perform the multiplcation
  result.m_x = m_a[0]  * rhs.m_x + m_a[1]  * rhs.m_y + m_a[2]  * rhs.m_z + m_a[3]  * rhs.m_w;
  result.m_y = m_a[4]  * rhs.m_x + m_a[5]  * rhs.m_y + m_a[6]  * rhs.m_z + m_a[7]  * rhs.m_w;
  result.m_z = m_a[8]  * rhs.m_x + m_a[9]  * rhs.m_y + m_a[10] * rhs.m_z + m_a[11] * rhs.m_w;
  result.m_w = m_a[12] * rhs.m_x + m_a[13] * rhs.m_y + m_a[14] * rhs.m_z + m_a[15] * rhs.m_w;
  
  return result;
}

Matrix4 Matrix4::operator*(Matrix4 const &rhs)
{
  // Resultant matrix
  Matrix4 result;
  
  // Perform the multiplication
  result.m_a[0] = m_a[0] * rhs.m_a[0] + m_a[1] * rhs.m_a[4] + m_a[2] * rhs.m_a[8]  + m_a[3] * rhs.m_a[12];
  result.m_a[1] = m_a[0] * rhs.m_a[1] + m_a[1] * rhs.m_a[5] + m_a[2] * rhs.m_a[9]  + m_a[3] * rhs.m_a[13]; 
  result.m_a[2] = m_a[0] * rhs.m_a[2] + m_a[1] * rhs.m_a[6] + m_a[2] * rhs.m_a[10] + m_a[3] * rhs.m_a[14];
  result.m_a[3] = m_a[0] * rhs.m_a[3] + m_a[1] * rhs.m_a[7] + m_a[2] * rhs.m_a[11] + m_a[3] * rhs.m_a[15];
  
  result.m_a[4] = m_a[4] * rhs.m_a[0] + m_a[5] * rhs.m_a[4] + m_a[6] * rhs.m_a[8]  + m_a[7] * rhs.m_a[12];
  result.m_a[5] = m_a[4] * rhs.m_a[1] + m_a[5] * rhs.m_a[5] + m_a[6] * rhs.m_a[9]  + m_a[7] * rhs.m_a[13]; 
  result.m_a[6] = m_a[4] * rhs.m_a[2] + m_a[5] * rhs.m_a[6] + m_a[6] * rhs.m_a[10] + m_a[7] * rhs.m_a[14];
  result.m_a[7] = m_a[4] * rhs.m_a[3] + m_a[5] * rhs.m_a[7] + m_a[6] * rhs.m_a[11] + m_a[7] * rhs.m_a[15];

  result.m_a[8]  = m_a[8] * rhs.m_a[0] + m_a[9] * rhs.m_a[4] + m_a[10] * rhs.m_a[8] + m_a[11] * rhs.m_a[12];
  result.m_a[9]  = m_a[8] * rhs.m_a[1] + m_a[9] * rhs.m_a[5] + m_a[10] * rhs.m_a[9] + m_a[11] * rhs.m_a[13];
  result.m_a[10] = m_a[8] * rhs.m_a[2] + m_a[9] * rhs.m_a[6] + m_a[10] * rhs.m_a[10] + m_a[11] * rhs.m_a[14];
  result.m_a[11] = m_a[8] * rhs.m_a[3] + m_a[9] * rhs.m_a[7] + m_a[10] * rhs.m_a[11] + m_a[11] * rhs.m_a[15];
  
  result.m_a[12] = m_a[12] * rhs.m_a[0] + m_a[13] * rhs.m_a[4] + m_a[14] * rhs.m_a[8]  + m_a[15] * rhs.m_a[12];
  result.m_a[13] = m_a[12] * rhs.m_a[1] + m_a[13] * rhs.m_a[5] + m_a[14] * rhs.m_a[9]  + m_a[15] * rhs.m_a[13];
  result.m_a[14] = m_a[12] * rhs.m_a[2] + m_a[13] * rhs.m_a[6] + m_a[14] * rhs.m_a[10] + m_a[15] * rhs.m_a[14];
  result.m_a[15] = m_a[12] * rhs.m_a[3] + m_a[13] * rhs.m_a[7] + m_a[14] * rhs.m_a[11] + m_a[15] * rhs.m_a[15];
  
  return result;
}

Matrix4 Matrix4::operator+(Matrix4 const &rhs)
{
  Matrix4 result;
  for (int i = 0; i < 16; ++i)
    result.m_a[i] = m_a[i] + rhs.m_a[i];
    
  return result;
}

void Matrix4::Identity()
{
  m_a[0]  = 1.0f; m_a[1]  = 0.0f; m_a[2]  = 0.0f; m_a[3]  = 0.0f;
  m_a[4]  = 0.0f; m_a[5]  = 1.0f; m_a[6]  = 0.0f; m_a[7]  = 0.0f;
  m_a[8]  = 0.0f; m_a[9]  = 0.0f; m_a[10] = 1.0f; m_a[11] = 0.0f;
  m_a[12] = 0.0f; m_a[13] = 0.0f; m_a[14] = 0.0f; m_a[15] = 1.0f;
}

void Matrix4::Translate(Vector4 const &pos)
{
  Translate(pos.m_x, pos.m_y, pos.m_z);
}

void Matrix4::Translate(float x, float y, float z)
{
  m_a[3]  = x;
  m_a[7]  = y;
  m_a[11] = z;
}

void Matrix4::Scale(Vector4 const &scale)
{
  Scale(scale.m_x, scale.m_y, scale.m_z);
}

void Matrix4::Scale(float x, float y, float z)
{
  m_a[0] = x;
  m_a[5] = y;
  m_a[10] = z;
}

void Matrix4::Scale(float val)
{
  m_a[0] = val;
  m_a[5] = val;
  m_a[10] = val;  
}

void Matrix4::RotateX(float radians)
{
  float cosine = std::cos(radians);
  float sine = std::sin(radians);

  m_a[5] = cosine; m_a[6]  = -sine;
  m_a[9] = sine;   m_a[10] =  cosine;
}

void Matrix4::RotateY(float radians)
{
  float cosine = std::cos(radians);
  float sine = std::sin(radians);

  m_a[0] = cosine; m_a[2]  = sine;
  m_a[8] = -sine;  m_a[10] = cosine;
}

void Matrix4::RotateZ(float radians)
{
  float cosine = std::cos(radians);
  float sine = std::sin(radians);

  m_a[0] = cosine; m_a[1] = -sine;
  m_a[4] = sine;   m_a[5] =  cosine;
}

void Matrix4::Rotate(Vector4 const &axis, float angle)
{
  Matrix4 a, tensor, b, cross_mat, c;

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

  m_30 = 0; m_31 = 0; m_32 = 0; m_33 = 1;
}

void Matrix4::Orthogonal(Vector4 const &x_axis, Vector4 const &y_axis, Vector4 const &z_axis)
{
  Vector4 x_norm(x_axis), y_norm(y_axis), z_norm(z_axis);
  x_norm.Normalize();
  y_norm.Normalize();
  z_norm.Normalize();

  m_00 = x_norm.m_x; m_01 = y_norm.m_x; m_02 = z_norm.m_x;
  m_10 = x_norm.m_y; m_11 = y_norm.m_y; m_12 = z_norm.m_y;
  m_20 = x_norm.m_z; m_21 = y_norm.m_z; m_22 = z_norm.m_z;
}

void Matrix4::LookAt(Vector4 const &position, Vector4 const &target, Vector4 const &up)
{
  Vector4 view, right, norm_up(up);
  view = Vector4(target.m_x - position.m_x, target.m_y - position.m_y, target.m_z - position.m_z, 0.0f);
  view.Normalize();
  norm_up.Normalize();

  right = view.Cross(norm_up);
  right.Normalize();
  norm_up = right.Cross(view);
  norm_up.Normalize();
  
  Matrix4 inverse_orthogonal, translation;

  inverse_orthogonal.Identity();

  inverse_orthogonal.m_00 = right.m_x;   inverse_orthogonal.m_01 = right.m_y;   inverse_orthogonal.m_02 = right.m_z;
  inverse_orthogonal.m_10 = norm_up.m_x; inverse_orthogonal.m_11 = norm_up.m_y; inverse_orthogonal.m_12 = norm_up.m_z;
  inverse_orthogonal.m_20 = -view.m_x;   inverse_orthogonal.m_21 = -view.m_y;   inverse_orthogonal.m_22 = -view.m_z;

  translation.Identity();
  translation.Translate(-position);

  *this = inverse_orthogonal * translation;
}

void Matrix4::PerspectiveProject(float cotangent_field_of_view, float aspect_ratio, float near_distance, float far_distance)
{
  m_00 = cotangent_field_of_view * aspect_ratio;
  m_11 = cotangent_field_of_view;
  m_22 = (near_distance + far_distance) / (near_distance - far_distance);
  m_23 = (2.0f * near_distance * far_distance) / (near_distance - far_distance);
  m_32 = -1.0f;
  m_33 = 0.0f;
}

void Matrix4::OrthographicProject(float cotangent_field_of_view, float aspect_ratio, float near_distance, float far_distance, float scaling_factor)
{
  // screw trying the correct orthographic projection, I'm going to make z equal to
  // the far plane, then do a perspective project from there

  m_22 = 0.0f;
  m_23 = scaling_factor * -far_distance + (1.0f - scaling_factor) * -near_distance;

  *this = CreatePerspectiveProjection(cotangent_field_of_view, aspect_ratio, near_distance, far_distance) * *this;

//   // this is the correct orthographic projection in case I ever need it again
//   m_00 = (2.0f/(width));
//   m_11 = (2.0f/(height));
//   m_22 = /*(scaling_factor) * */(-1.0f/(near_distance - far_distance));
//   m_23 = /*(scaling_factor) * */((near_distance)/(near_distance - far_distance));
//   m_33 = 1.0f;
}

void Matrix4::Viewport(float width, float height)
{
  m_00 = width/2.0f;
  m_03 = width/2.0f;
  m_11 = -height/2.0f;
  m_13 = height/2.0f;
}

void Matrix4::Tensor(Vector4 const &vec1, Vector4 const &vec2)
{
  m_00 = vec1.m_x * vec2.m_x; m_01 = vec1.m_x * vec2.m_y; m_02 = vec1.m_x * vec2.m_z;
  m_10 = vec1.m_y * vec2.m_x; m_11 = vec1.m_y * vec2.m_y; m_12 = vec1.m_y * vec2.m_z;
  m_20 = vec1.m_z * vec2.m_x; m_21 = vec1.m_z * vec2.m_y; m_22 = vec1.m_z * vec2.m_z;
  m_33 = 0.0f;
}

void Matrix4::CrossProduct(Vector4 const &vec)
{
  m_00 = 0.0f;     m_01 = -vec.m_z; m_02 = vec.m_y;
  m_10 = vec.m_z;  m_11 = 0.0f;     m_12 = -vec.m_x;
  m_20 = -vec.m_y; m_21 = vec.m_x;  m_22 = 0.0f;
  m_33 = 0.0f;
}

void Matrix4::Transpose()
{
  float temp;
  for (int i = 0; i < 4; ++i)
  {
    for (int j = i; j < 4; ++j)
    {
      temp = m_a[j + i * 4];
      m_a[j + i * 4] = m_a[i + j * 4];
      m_a[i + j * 4] = temp;
    }
  }
}

void Matrix4::Inverse()
{
  double det;
  Matrix4 ret;

  ret.m_a[0] = m_a[5]*m_a[10]*m_a[15] - m_a[5]*m_a[11]*m_a[14] - m_a[9]*m_a[6]*m_a[15]
             + m_a[9]*m_a[7]*m_a[14] + m_a[13]*m_a[6]*m_a[11] - m_a[13]*m_a[7]*m_a[10];

  ret.m_a[4] = -m_a[4]*m_a[10]*m_a[15] + m_a[4]*m_a[11]*m_a[14] + m_a[8]*m_a[6]*m_a[15]
             - m_a[8]*m_a[7]*m_a[14] - m_a[12]*m_a[6]*m_a[11] + m_a[12]*m_a[7]*m_a[10];

  ret.m_a[8] = m_a[4]*m_a[9]*m_a[15] - m_a[4]*m_a[11]*m_a[13] - m_a[8]*m_a[5]*m_a[15]
             + m_a[8]*m_a[7]*m_a[13] + m_a[12]*m_a[5]*m_a[11] - m_a[12]*m_a[7]*m_a[9];

  ret.m_a[12] = -m_a[4]*m_a[9]*m_a[14] + m_a[4]*m_a[10]*m_a[13] + m_a[8]*m_a[5]*m_a[14]
              - m_a[8]*m_a[6]*m_a[13] - m_a[12]*m_a[5]*m_a[10] + m_a[12]*m_a[6]*m_a[9];

  ret.m_a[1] = -m_a[1]*m_a[10]*m_a[15] + m_a[1]*m_a[11]*m_a[14] + m_a[9]*m_a[2]*m_a[15]
             - m_a[9]*m_a[3]*m_a[14] - m_a[13]*m_a[2]*m_a[11] + m_a[13]*m_a[3]*m_a[10];

  ret.m_a[5] = m_a[0]*m_a[10]*m_a[15] - m_a[0]*m_a[11]*m_a[14] - m_a[8]*m_a[2]*m_a[15]
             + m_a[8]*m_a[3]*m_a[14] + m_a[12]*m_a[2]*m_a[11] - m_a[12]*m_a[3]*m_a[10];

  ret.m_a[9] = -m_a[0]*m_a[9]*m_a[15] + m_a[0]*m_a[11]*m_a[13] + m_a[8]*m_a[1]*m_a[15]
             - m_a[8]*m_a[3]*m_a[13] - m_a[12]*m_a[1]*m_a[11] + m_a[12]*m_a[3]*m_a[9];

  ret.m_a[13] = m_a[0]*m_a[9]*m_a[14] - m_a[0]*m_a[10]*m_a[13] - m_a[8]*m_a[1]*m_a[14]
              + m_a[8]*m_a[2]*m_a[13] + m_a[12]*m_a[1]*m_a[10] - m_a[12]*m_a[2]*m_a[9];

  ret.m_a[2] = m_a[1]*m_a[6]*m_a[15] - m_a[1]*m_a[7]*m_a[14] - m_a[5]*m_a[2]*m_a[15]
             + m_a[5]*m_a[3]*m_a[14] + m_a[13]*m_a[2]*m_a[7] - m_a[13]*m_a[3]*m_a[6];

  ret.m_a[6] = -m_a[0]*m_a[6]*m_a[15] + m_a[0]*m_a[7]*m_a[14] + m_a[4]*m_a[2]*m_a[15]
             - m_a[4]*m_a[3]*m_a[14] - m_a[12]*m_a[2]*m_a[7] + m_a[12]*m_a[3]*m_a[6];

  ret.m_a[10] = m_a[0]*m_a[5]*m_a[15] - m_a[0]*m_a[7]*m_a[13] - m_a[4]*m_a[1]*m_a[15]
              + m_a[4]*m_a[3]*m_a[13] + m_a[12]*m_a[1]*m_a[7] - m_a[12]*m_a[3]*m_a[5];

  ret.m_a[14] = -m_a[0]*m_a[5]*m_a[14] + m_a[0]*m_a[6]*m_a[13] + m_a[4]*m_a[1]*m_a[14]
              - m_a[4]*m_a[2]*m_a[13] - m_a[12]*m_a[1]*m_a[6] + m_a[12]*m_a[2]*m_a[5];

  ret.m_a[3] = -m_a[1]*m_a[6]*m_a[11] + m_a[1]*m_a[7]*m_a[10] + m_a[5]*m_a[2]*m_a[11]
             - m_a[5]*m_a[3]*m_a[10] - m_a[9]*m_a[2]*m_a[7] + m_a[9]*m_a[3]*m_a[6];

  ret.m_a[7] = m_a[0]*m_a[6]*m_a[11] - m_a[0]*m_a[7]*m_a[10] - m_a[4]*m_a[2]*m_a[11]
             + m_a[4]*m_a[3]*m_a[10] + m_a[8]*m_a[2]*m_a[7] - m_a[8]*m_a[3]*m_a[6];

  ret.m_a[11] = -m_a[0]*m_a[5]*m_a[11] + m_a[0]*m_a[7]*m_a[9] + m_a[4]*m_a[1]*m_a[11]
              - m_a[4]*m_a[3]*m_a[9] - m_a[8]*m_a[1]*m_a[7] + m_a[8]*m_a[3]*m_a[5];

  ret.m_a[15] = m_a[0]*m_a[5]*m_a[10] - m_a[0]*m_a[6]*m_a[9] - m_a[4]*m_a[1]*m_a[10]
              + m_a[4]*m_a[2]*m_a[9] + m_a[8]*m_a[1]*m_a[6] - m_a[8]*m_a[2]*m_a[5];

  det = m_a[0]*ret.m_a[0] + m_a[1]*ret.m_a[4] + m_a[2]*ret.m_a[8] + m_a[3]*ret.m_a[12];
  if (det == 0)
    return;

  det = 1.0 / det;

  for (int i = 0; i < 16; i++)
    m_a[i] = float(ret.m_a[i] * det);
}

void Matrix4::InverseOrthogonal(Vector4 const &x_axis, Vector4 const &y_axis, Vector4 const &z_axis)
{
  Vector4 x_norm(x_axis), y_norm(y_axis), z_norm(z_axis);
  x_norm.Normalize();
  y_norm.Normalize();
  z_norm.Normalize();

  m_00 = x_norm.m_x; m_01 = x_norm.m_y; m_02 = x_norm.m_z;
  m_10 = y_norm.m_x; m_11 = y_norm.m_y; m_12 = y_norm.m_z;
  m_20 = z_norm.m_x; m_21 = z_norm.m_y; m_22 = z_norm.m_z;
}

void Matrix4::InverseLookAt(Vector4 const &position, Vector4 const &target, Vector4 const &up)
{
  Vector4 view, right, norm_up(up);
  view = target + Vector4(-position.m_x, -position.m_y, -position.m_z, -position.m_w);
  view.Normalize();
  norm_up.Normalize();

  right = view.Cross(up);
  
  Matrix4 inverse_orthogonal, translation;

  inverse_orthogonal.Identity();

  inverse_orthogonal.m_00 = right.m_x; inverse_orthogonal.m_01 = norm_up.m_x; inverse_orthogonal.m_02 = -view.m_x;
  inverse_orthogonal.m_10 = right.m_y; inverse_orthogonal.m_11 = norm_up.m_y; inverse_orthogonal.m_12 = -view.m_y;
  inverse_orthogonal.m_20 = right.m_z; inverse_orthogonal.m_21 = norm_up.m_z; inverse_orthogonal.m_22 = -view.m_z;

  translation.Identity();
  translation.Translate(position);

  *this = translation * inverse_orthogonal;
}

void Matrix4::InversePerspectiveProject(float field_of_view, float aspect_ratio, float near_distance, float far_distance)
{
  m_00 = aspect_ratio/field_of_view;
  m_11 = 1.0f/field_of_view;
  m_22 = 0.0f;
  m_23 = -1.0f;
  m_32 = (near_distance - far_distance) / (2.0f * near_distance * far_distance);
  m_33 = (near_distance + far_distance) / (2.0f * near_distance * far_distance);
}

void Matrix4::InverseOrthographicProject(float left_distance, float right_distance, float bottom_distance, float top_distance, float near_distance, float far_distance)
{
  m_00 = ((right_distance - left_distance)/2.0f);
  m_03 = ((right_distance + left_distance)/2.0f);
  m_11 = ((top_distance - bottom_distance)/2.0f);
  m_13 = ((top_distance + bottom_distance)/2.0f);
  m_22 = ((near_distance - far_distance)/2.0f);
  m_23 = (-(far_distance + near_distance)/2.0f);
}

void Matrix4::InverseViewport(float width, float height)
{
  m_00 =  2.0f/width;
  m_03 = -1.0f;
  m_11 = -2.0f/height;
  m_13 =  1.0f;
}

Improbability::Matrix4 Matrix4::CreateTranslation(Vector4 const &pos)
{
  Matrix4 new_matrix;
  new_matrix.Identity();

  new_matrix.Translate(pos);

  return new_matrix;
}

Improbability::Matrix4 Matrix4::CreateTranslation(float x, float y, float z)
{
  Matrix4 new_matrix;
  new_matrix.Identity();

  new_matrix.Translate(x, y, z);

  return new_matrix;
}

Improbability::Matrix4 Matrix4::CreateScaling(Vector4 const &scale)
{
  Matrix4 new_matrix;
  new_matrix.Identity();

  new_matrix.Scale(scale);

  return new_matrix;
}

Improbability::Matrix4 Matrix4::CreateScaling(float x, float y, float z)
{
  Matrix4 new_matrix;
  new_matrix.Identity();

  new_matrix.Scale(x, y, z);

  return new_matrix;
}

Improbability::Matrix4 Matrix4::CreateScaling(float scale)
{
  Matrix4 new_matrix;
  new_matrix.Identity();

  new_matrix.Scale(scale);

  return new_matrix;
}

Improbability::Matrix4 Matrix4::CreateRotationX(float angle)
{
  Matrix4 new_matrix;
  new_matrix.Identity();

  new_matrix.RotateX(angle);

  return new_matrix;
}

Improbability::Matrix4 Matrix4::CreateRotationY(float angle)
{
  Matrix4 new_matrix;
  new_matrix.Identity();

  new_matrix.RotateY(angle);

  return new_matrix;
}

Improbability::Matrix4 Matrix4::CreateRotationZ(float angle)
{
  Matrix4 new_matrix;
  new_matrix.Identity();

  new_matrix.RotateZ(angle);

  return new_matrix;
}

Improbability::Matrix4 Matrix4::CreateRotation(Vector4 const &axis, float angle)
{
  Matrix4 new_matrix;
  new_matrix.Identity();

  new_matrix.Rotate(axis, angle);

  return new_matrix;
}

Improbability::Matrix4 Matrix4::CreateOrthogonal(Vector4 const &x_axis, Vector4 const &y_axis, Vector4 const &z_axis)
{
  Matrix4 new_matrix;
  new_matrix.Identity();

  new_matrix.Orthogonal(x_axis, y_axis, z_axis);

  return new_matrix;
}

Improbability::Matrix4 Matrix4::CreateLookAt(Vector4 const &postion, Vector4 const &target, Vector4 const &up)
{
  Matrix4 new_matrix;
  new_matrix.Identity();

  new_matrix.LookAt(postion, target, up);

  return new_matrix;
}

Improbability::Matrix4 Matrix4::CreatePerspectiveProjection(float cotangent_field_of_view, float aspect_ratio, float near_distance, float far_distance)
{
  Matrix4 new_matrix;
  new_matrix.Identity();

  new_matrix.PerspectiveProject(cotangent_field_of_view, aspect_ratio, near_distance, far_distance);

  return new_matrix;
}

Improbability::Matrix4 Matrix4::CreateOrthographicProjection(float cotangent_field_of_view, float aspect_ratio, float near_distance, float far_distance, float scaling_factor)
{
  Matrix4 new_matrix;
  new_matrix.Identity();

  new_matrix.OrthographicProject(cotangent_field_of_view, aspect_ratio, near_distance, far_distance, scaling_factor);

  return new_matrix;
}

Improbability::Matrix4 Matrix4::CreateViewport(float width, float height)
{
  Matrix4 new_matrix;
  new_matrix.Identity();

  new_matrix.Viewport(width, height);

  return new_matrix;
}

Improbability::Matrix4 Matrix4::CreateInverseOrthogonal(Vector4 const &x_axis, Vector4 const &y_axis, Vector4 const &z_axis)
{
  Matrix4 new_matrix;
  new_matrix.Identity();

  new_matrix.InverseOrthogonal(x_axis, y_axis, z_axis);

  return new_matrix;
}

Improbability::Matrix4 Matrix4::CreateInverseLookAt(Vector4 const &position, Vector4 const &target, Vector4 const &up)
{
  Matrix4 new_matrix;
  new_matrix.Identity();

  new_matrix.InverseLookAt(position, target, up);

  return new_matrix;
}

Improbability::Matrix4 Matrix4::CreateInversePerspectiveProjection(float field_of_view, float aspect_ratio, float near_distance, float far_distance)
{
  Matrix4 new_matrix;
  new_matrix.Identity();

  new_matrix.InversePerspectiveProject(field_of_view, aspect_ratio, near_distance, far_distance);

  return new_matrix;
}

Improbability::Matrix4 Matrix4::CreateInverseOrthographicProjection(float left_distance, float right_distance, float bottom_distance, float top_distance, float near_distance, float far_distance)
{
  Matrix4 new_matrix;
  new_matrix.Identity();

  new_matrix.InverseOrthographicProject(left_distance, right_distance, bottom_distance, top_distance, near_distance, far_distance);

  return new_matrix;
}

Improbability::Matrix4 Matrix4::CreateInverseViewport(float width, float height)
{
  Matrix4 new_matrix;
  new_matrix.Identity();

  new_matrix.InverseViewport(width, height);

  return new_matrix;
}

}
