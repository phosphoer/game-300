// Nicholas Baldwin
// 2011/07/15
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{

class Shape
{
public:
  enum ShapeType
  {
    ST_Plane,
    ST_Cube,
    //ST_Cone,
    //ST_Cylinder,
    ST_Sphere,
    //ST_Taurus,

    ST_Max
  };

  // MUST call this before attempting to draw anything, cannot be called twice 
  //   without calling Uninitialize first
  // slices - the number of slices to have in each shape
  // stacks - the number of stacks to have in each shape
  static void Initialize(unsigned slices = 16, unsigned stacks = 16);

  // draws the shape
  // shape_type - the shape to draw
  static void Draw(ShapeType shape_type);

  // uninitialize the shape library (either to deallocate the shapes or to 
  //   reinitialize the shape library with a different slice/stack count)
  static void Uninitialize();

private:
  // the count of slices and stacks per shape
  static unsigned m_slices;
  static unsigned m_stacks;

  // the vertex declaration and buffer handles
  static unsigned m_declaration;
  static unsigned m_shape_buffer[ST_Max];

  // the vertex layout
  struct Vertex
  {
    Vector2 m_tex_coord;
    Vector3 m_position, m_normal, m_tangent, m_bitangent;
    A8R8G8B8 m_color;
  };

  // the triangle layout, used to fill out index buffer/create tangent/bitangent
  struct Triangle
  {
    union
    {
      struct  
      {
        unsigned m_0, m_1, m_2;
      };
      unsigned m_a[3];
    };
  };
};

}