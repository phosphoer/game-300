// Nicholas Baldwin
// 2011/07/17
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "Vector2.h"

namespace Improbability
{

class Serializer;
class MetaComposition;

class Vector3
{
public:
  static void BindMeta(MetaComposition*);
  
  //Default constructor
  Vector3(){}

  // Constructor that takes all values
  Vector3(float x_, float y_, float z_):m_x(x_), m_y(y_), m_z(z_) {}
  Vector3(int x_, int y_, int z_):m_x(static_cast<float>(x_)), m_y(static_cast<float>(y_)), m_z(static_cast<float>(z_)) {}

  void Deserialize(Serializer &serializer);

  // Constructs a Vector3 as a point (with z being 1)
  static Vector3 Point(float x_, float y_){return Vector3(x_, y_, 1.0f);}
  static Vector3 Point(Vector2 const &original){return Vector3(original.m_x, original.m_y, 1.0f);}

  // Constructs a Vector3 as a vector (with z being 0)
  static Vector3 Vector(float x_, float y_){return Vector3(x_, y_, 0.0f);}
  static Vector3 Vector(Vector2 const &original){return Vector3(original.m_x, original.m_y, 0.0f);}

  // Calculate the cross product of two vectors
  // rhs - Vector to cross with
  // return - Cross product vector
  Vector3 Cross(Vector3 const &rhs) const;

  // Calculate the dot product of two vectors
  // rhs - Vector to dot with
  // return - Dot product of two vectors
  float Dot(Vector3 const &rhs) const;

  // Calculate the projection onto another vector
  Vector3 Project(Vector3 const &rhs) const;

  // Normalizes the vector
  void Normalize();

  // Return a normalized version of the vector
  // return - The normalized vector
  Vector3 Normalized() const;

  // Returns the magnitude of the vector
  // return - the length of the vector
  float Magnitude() const;

  // Returns the squared magnitude of the vector
  // return - The squared length of the vector
  float MagnitudeSquared() const;

  // Calculates the distance between two positions
  // rhs - The position to get the distance to
  // return - Distance between two positions 
  float Distance(Vector3 const &rhs) const;

  // Calculates the squared distance between two positions
  // rhs - The position to get the squared distance to
  // return - Squared Distance between two positions 
  float DistanceSquared(Vector3 const &rhs) const;

  // Affine versions of the above operations that do not take z into account
  void AffineNormalize();
  Vector3 AffineNormalized() const;
  float AffineMagnitude() const;
  float AffineMagnitudeSquared() const;
  float AffineDistance(Vector3 const &rhs) const;
  float AffineDistanceSquared(Vector3 const &rhs) const;

  // Add to vector
  // rhs - Vector to add
  void operator+=(Vector3 const &rhs);
  void operator-=(Vector3 const &rhs);

  // Adds two vectors
  // rhs - Vector to add with
  // return - Sum vector
  Vector3 operator+(Vector3 const &rhs) const;

  // Subtracts two vectors
  // rhs - Vector to subtract
  // return - Difference vector
  Vector3 operator-(Vector3 const &rhs) const;

  // Negates the vector
  // return - A vector with negated components
  Vector3 operator-() const;

  // Scales the vector by a scalar
  // scale - Amount to scale by
  // return - A scaled vector
  Vector3 operator*(float scale) const;
  void operator*=(float scale) {m_x *= scale; m_y *= scale; m_z *= scale;}

public:
  // This structure ensures that no padding is inserted between members
  union
  {
    struct {float m_x; float m_y; float m_z;};
    float m_a[3];
  };
};

}