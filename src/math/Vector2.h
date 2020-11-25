// Nicholas Baldwin
// 2011/07/17
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{

class Serializer;
class MetaComposition;

class Vector2
{
public:
  static void BindMeta(MetaComposition*);
  
  //Default constructor
  Vector2(){}

  // Constructor that takes all values
  Vector2(float x_, float y_):m_x(x_), m_y(y_) {}
  Vector2(int x_, int y_):m_x(static_cast<float>(x_)), m_y(static_cast<float>(y_)) {}

  // Read from file
  void Deserialize(Serializer &serializer);

  // Calculate the dot product of two vectors
  // rhs - Vector to dot with
  // return - Dot product of two vectors
  float Dot(Vector2 const &rhs) const;

  // Calculate the projection onto another vector
  Vector2 Project(Vector2 const &rhs) const;

  // Normalizes the vector
  void Normalize();

  // Return a normalized version of the vector
  // return - The normalized vector
  Vector2 Normalized() const;

  // Returns the magnitude of the vector
  // return - the length of the vector
  float Magnitude() const;

  // Returns the squared magnitude of the vector
  // return - The squared length of the vector
  float MagnitudeSquared() const;

  // Calculates the distance between two positions
  // rhs - The position to get the distance to
  // return - Distance between two positions 
  float Distance(Vector2 const &rhs) const;

  // Calculates the squared distance between two positions
  // rhs - The position to get the squared distance to
  // return - Squared Distance between two positions 
  float DistanceSquared(Vector2 const &rhs) const;

  // Adds two vectors
  // rhs - Vector to add with
  // return - Sum vector
  Vector2 operator+(Vector2 const &rhs) const;

  // Subtracts two vectors
  // rhs - Vector to subtract
  // return - Difference vector
  Vector2 operator-(Vector2 const &rhs) const;

  // Negates the vector
  // return - A vector with negated components
  Vector2 operator-() const;

  // Scales the vector by a scalar
  // scale - Amount to scale by
  // return - A scaled vector
  Vector2 operator*(float scale) const;

public:
  // This structure ensures that no padding is inserted between members
  union
  {
    struct {float m_x; float m_y;};
    float m_a[2];
  };
};

}