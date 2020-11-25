// Nicholas Baldwin
// 2011/07/17
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "Vector3.h"

namespace Improbability
{

class Matrix2;

class Matrix3
{
public:
  // Constructor
  Matrix3(){}; 

  // Creates an affine matrix from a lower order one
  Matrix3(Matrix2 const &original);

  // Gets a row from the matrix
  Vector3 GetRow(int row);
      
  // Multiplication with a vector
  Vector3 operator*(Vector3 const &rhs);
      
  // Multiplication with a matrix
  Matrix3 operator*(Matrix3 const &rhs);
      
  // Addition with a matrix
  Matrix3 operator+(Matrix3 const &rhs);
      
  // Make the matrix the identity matrix 
  void Identity();
      
  // Scale the matrix 
  void Scale(Vector3 const &scale);
  void Scale(float x, float y, float z);
  void Scale(float val);
      
  // Rotate the matrix (values in radians)
  void RotateX(float radians);
  void RotateY(float radians);
  void RotateZ(float radians);
  void Rotate(Vector3 const &axis, float angle);

  // Affine functions for dealing with converted Vector2s
  // Translate the matrix
  void AffineTranslate(Vector3 const &pos);
  void AffineTranslate(float x, float y);
      
  // Scale the matrix 
  void AffineScale(Vector3 const &scale);
  void AffineScale(float x, float y);
  void AffineScale(float val);
      
  // Rotate the matrix (values in radians)
  void AffineRotate(float radians);

  // Make the matrix into a tensor
  void Tensor(Vector3 const &vec1, Vector3 const &vec2);

  // Make the matrix into a cross product
  void CrossProduct(Vector3 const &vec);

  // Transpose the matrix
  void Transpose();

  // Generic inverse of the matrix (slow)
  // (does nothing if matrix not invertible)
  void Inverse();

  // Scale the matrix 
  static Matrix3 CreateScaling(Vector3 const &scale);
  static Matrix3 CreateScaling(float x, float y, float z);
  static Matrix3 CreateScaling(float val);
      
  // Rotate the matrix (values in radians)
  static Matrix3 CreateRotationX(float radians);
  static Matrix3 CreateRotationY(float radians);
  static Matrix3 CreateRotationZ(float radians);
  static Matrix3 CreateRotation(Vector3 const &axis, float angle);

  // Affine functions for dealing with converted Vector2s
  // Translate the matrix
  static Matrix3 CreateAffineTranslation(Vector3 const &pos);
  static Matrix3 CreateAffineTranslation(float x, float y);
      
  // Scale the matrix 
  static Matrix3 CreateAffineScaling(Vector3 const &scale);
  static Matrix3 CreateAffineScaling(float x, float y);
  static Matrix3 CreateAffineScaling(float val);
      
  // Rotate the matrix (values in radians)
  static Matrix3 CreateAffineRotation(float radians);

public:
  // Union and struct ensures no padding
  union
  {
    struct 
    {
      float m_00; float m_01; float m_02;
      float m_10; float m_11; float m_12;
      float m_20; float m_21; float m_22;
    };
    float m_a[9];
  };
};

}