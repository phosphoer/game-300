// David Evans
// 2011/05/18
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "Vector3.h"
#include "Vector2.h"

namespace Improbability
{

class Serializer;
class MetaComposition;

class Vector4
{
  public:
    static void BindMeta(MetaComposition*);
    
    // Default constructor
    Vector4() {}

    // Constructor that takes all values
    Vector4(float x_, float y_, float z_, float w_):m_x(x_), m_y(y_), m_z(z_), m_w(w_) {}
    Vector4(Vector3 pos, float w_):m_x(pos.m_x), m_y(pos.m_y), m_z(pos.m_z), m_w(w_) {}

    // Read from file
    void Deserialize(Serializer &serializer);

    // Constructs a Vector4 as a point (with w being 1)
    static Vector4 Point(float x_, float y_, float z_){return Vector4(x_, y_, z_, 1.0f);}
    static Vector4 Point(Vector3 const &original){return Vector4(original.m_x, original.m_y, original.m_z, 1.0f);}

    // Constructs a Vector4 as a vector (with w being 0)
    static Vector4 Vector(float x_, float y_, float z_){return Vector4(x_, y_, z_, 0.0f);}
    static Vector4 Vector(Vector3 const &original){return Vector4(original.m_x, original.m_y, original.m_z, 0.0f);}

    // Get a Vector3 with components x, y, z
    Vector3 ToVector3() {return Vector3(m_x, m_y, m_z);}
    Vector2 ToVector2() {return Vector2(m_x, m_y);}

    // Calculate the cross product of two vectors
    // rhs - Vector to cross with
    // return - Cross product vector
    Vector4 Cross(Vector4 const &rhs) const;

    // Calculate the dot product of two vectors
    // rhs - Vector to dot with
    // return - Dot product of two vectors
    float Dot(Vector4 const &rhs) const;

    // Normalizes the vector (mutator)
    void Normalize();

    // Return a normalized version of the vector (non-mutating)
    // return - The normalized vector
    Vector4 Normalized() const;

    // Returns the magnitude of the vector (SLOW)
    // return - The length of the vector
    float Magnitude() const;

    // Returns the squared magnitude of the vector
    // return - The squared length of the vector
    float MagnitudeSquared() const;

    // Calculates the distance between two positions
    // rhs - The position to get the distance to
    // return - Distance between two positions
    float Distance(Vector4 const &rhs) const;

    // Calculates the squared distance between two positions
    // rhs - The position to get the squared distance to
    // return - Squared Distance between two positions 
    float DistanceSquared(Vector4 const &rhs) const;

    // Add to vector
    void operator+=(Vector4 const &rhs);
    void operator+=(Vector3 const &rhs);

    // Adds two vectors
    // rhs - Vector to add with
    // return - Sum vector
    Vector4 operator+(Vector4 const &rhs) const;
    Vector4 operator+(Vector3 const &rhs) const;

    // Subtracts two vectors
    // rhs - Vector to subtract
    // return - Difference vector
    Vector4 operator-(Vector4 const &rhs) const;

    // Negates the vector
    // return - A vector with negated components (excluding w)
    Vector4 operator-() const;

    // Scales the vector by a scalar
    // scale - Amount to scale by
    // return - A scaled vector (w is not scaled)
    Vector4 operator*(float scale) const;

    void zero(void);

    // This structure ensures that no padding is inserted between members
    union
    {
      struct {float m_x; float m_y; float m_z; float m_w;};
      float m_a[4];
    };
};

}