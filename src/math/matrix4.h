// David Evans
// 2011/05/18
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "Vector4.h"

namespace Improbability
{

class Matrix3;

class Matrix4
{
  public:
    // Constructor
    Matrix4(){};

    // Creates an affine matrix from a lower order one
    Matrix4(Matrix3 const &original);
      
    // Gets a row from the matrix
    Vector4 GetRow(int row);
      
    // Multiplication with a vector
    Vector4 operator*(Vector4 const &rhs);
      
    // Multiplication with a matrix
    Matrix4 operator*(Matrix4 const &rhs);
      
    // Addition with a matrix
    Matrix4 operator+(Matrix4 const &rhs);
      
    // Make the matrix the identity matrix 
    void Identity();
      
    // Translate the matrix
    void Translate(Vector4 const &pos);
    void Translate(float x, float y, float z);
      
    // Scale the matrix 
    void Scale(Vector4 const &scale);
    void Scale(float x, float y, float z);
    void Scale(float val);
      
    // Rotate the matrix (values in radians)
    void RotateX(float radians);
    void RotateY(float radians);
    void RotateZ(float radians);
    void Rotate(Vector4 const &axis, float angle);

    // Make the matrix orthogonal alignment
    void Orthogonal(Vector4 const &x_axis, Vector4 const &y_axis, Vector4 const &z_axis);

    // Make the matrix lookat
    void LookAt(Vector4 const &position, Vector4 const &target, Vector4 const &up);
    
    // Project the matrix
    void PerspectiveProject(float cotangent_field_of_view, float aspect_ratio, float near_distance, float far_distance);
    void OrthographicProject(float cotangent_field_of_view, float aspect_ratio, float near_distance, float far_distance, float scaling_factor);

    // Make the matrix into a viewport
    void Viewport(float width, float height);

    // Make the matrix into a tensor
    void Tensor(Vector4 const &vec1, Vector4 const &vec2);

    // Make the matrix into a cross product
    void CrossProduct(Vector4 const &vec);

    // Transpose the matrix
    void Transpose();

    // Generic inverse of the matrix (slow)
    // (does nothing if matrix not invertible)
    void Inverse();

    // Faster inverses of some of the above functions
    // (those without have obvious inverses)
    void InverseOrthogonal(Vector4 const &x_axis, Vector4 const &y_axis, Vector4 const &z_axis);
    void InverseLookAt(Vector4 const &position, Vector4 const &target, Vector4 const &up);

    void InversePerspectiveProject(float field_of_view, float aspect_ratio, float near_distance, float far_distance);
    void InverseOrthographicProject(float left_distance, float right_distance, float bottom_distance, float top_distance, float near_distance, float far_distance);

    void InverseViewport(float width, float height);

    // static functions to facilitate quick transforms
    // Creates a translation matrix
    static Matrix4 CreateTranslation(Vector4 const &pos);
    static Matrix4 CreateTranslation(float x, float y, float z);

    // Creates a scaling matrix
    static Matrix4 CreateScaling(Vector4 const &scale);
    static Matrix4 CreateScaling(float x, float y, float z);
    static Matrix4 CreateScaling(float scale);

    // Creates a rotation matrix
    static Matrix4 CreateRotationX(float angle);
    static Matrix4 CreateRotationY(float angle);
    static Matrix4 CreateRotationZ(float angle);
    static Matrix4 CreateRotation(Vector4 const &axis, float angle);

    // Create an Orthogonal alignment matrix
    static Matrix4 CreateOrthogonal(Vector4 const &x_axis, Vector4 const &y_axis, Vector4 const &z_axis);

    // Create a lookat matrix
    static Matrix4 CreateLookAt(Vector4 const &postion, Vector4 const &target, Vector4 const &up);
    
    // Create a projection matrix
    static Matrix4 CreatePerspectiveProjection(float cotangent_field_of_view, float aspect_ratio, float near_distance, float far_distance);
    static Matrix4 CreateOrthographicProjection(float cotangent_field_of_view, float aspect_ratio, float near_distance, float far_distance, float scaling_factor);

    // Create a viewport matrix
    static Matrix4 CreateViewport(float width, float height);

    // Create inverses of some of the above functions
    static Matrix4 CreateInverseOrthogonal(Vector4 const &x_axis, Vector4 const &y_axis, Vector4 const &z_axis);
    static Matrix4 CreateInverseLookAt(Vector4 const &position, Vector4 const &target, Vector4 const &up);
      
    static Matrix4 CreateInversePerspectiveProjection(float field_of_view, float aspect_ratio, float near_distance, float far_distance);
    static Matrix4 CreateInverseOrthographicProjection(float left_distance, float right_distance, float bottom_distance, float top_distance, float near_distance, float far_distance);

    static Matrix4 CreateInverseViewport(float width, float height);

  public:
    // Union and struct ensures no padding
    union
    {
      struct 
      {
        float m_00; float m_01; float m_02; float m_03;
        float m_10; float m_11; float m_12; float m_13;
        float m_20; float m_21; float m_22; float m_23;
        float m_30; float m_31; float m_32; float m_33;
      };
      float m_a[16];
    };
};
  
}
