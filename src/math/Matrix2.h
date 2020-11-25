// Nicholas Baldwin
// 2011/07/17
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "Vector2.h"

namespace Improbability
{

class Matrix2
{
public:
  Matrix2(){};

  // Gets a row from the matrix
  Vector2 GetRow(int row);
      
  // Multiplication with a vector
  Vector2 operator*(Vector2 const &rhs);
      
  // Multiplication with a matrix
  Matrix2 operator*(Matrix2 const &rhs);
      
  // Addition with a matrix
  Matrix2 operator+(Matrix2 const &rhs);
      
  // Make the matrix the identity matrix 
  void Identity();
      
  // Scale the matrix 
  void Scale(Vector2 const &scale);
  void Scale(float x, float y);
  void Scale(float val);
      
  // Rotate the matrix (values in radians)
  void Rotate(float angle);

  // Transpose the matrix
  void Transpose();

  // Generic inverse of the matrix (slow)
  // (does nothing if matrix not invertible)
  void Inverse();

  // Scale the matrix 
  static Matrix2 CreateScaling(Vector2 const &scale);
  static Matrix2 CreateScaling(float x, float y);
  static Matrix2 CreateScaling(float val);
      
  // Rotate the matrix (values in radians)
  static Matrix2 CreateRotation(float angle);

  // Union and struct ensures no padding
  union
  {
    struct 
    {
      float m_00; float m_01;
      float m_10; float m_11;
    };
    float m_a[4];
  };
};

}