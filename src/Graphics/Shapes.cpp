// Nicholas Baldwin
// 2011/07/17
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Shapes.h"
#include "GraphicsVertexTypes.h"
#include "GraphicsDevice.h"

namespace Improbability
{
unsigned Shape::m_slices = 0;
unsigned Shape::m_stacks = 0;
unsigned Shape::m_declaration = -1;
unsigned Shape::m_shape_buffer[ST_Max];

void Shape::Initialize(unsigned slices, unsigned stacks)
{
  ErrorIf(m_slices != 0, "Don't initialize the shape library twice!");

  // just set the slices and stacks
  m_slices = slices; 
  m_stacks = stacks;

  // Create the vertex declaration
  GraphicsVertexTypes::VertexElement elements[6];

  elements[0].m_stream = 0;
  elements[0].m_type = GraphicsVertexTypes::VM_Vector2;
  elements[0].m_usage = GraphicsVertexTypes::MU_TexCoord;

  elements[1].m_stream = 0;
  elements[1].m_type = GraphicsVertexTypes::VM_Vector3;
  elements[1].m_usage = GraphicsVertexTypes::MU_Position;

  elements[2].m_stream = 0;
  elements[2].m_type = GraphicsVertexTypes::VM_Vector3;
  elements[2].m_usage = GraphicsVertexTypes::MU_Normal;

  elements[3].m_stream = 0;
  elements[3].m_type = GraphicsVertexTypes::VM_Vector3;
  elements[3].m_usage = GraphicsVertexTypes::MU_Tangent;

  elements[4].m_stream = 0;
  elements[4].m_type = GraphicsVertexTypes::VM_Vector3;
  elements[4].m_usage = GraphicsVertexTypes::MU_Bitangent;

  elements[5].m_stream = 0;
  elements[5].m_type = GraphicsVertexTypes::VM_Color;
  elements[5].m_usage = GraphicsVertexTypes::MU_Color;

  m_declaration = GraphicsDevice::CreateVertexDeclaration(elements, 6);

  // Generate the Plane
  m_shape_buffer[ST_Plane] = GraphicsDevice::CreateVertexBuffer(2 * (stacks) * (slices + 1), 
                                                                0, false, m_declaration);

  Vertex *plane_buffer = reinterpret_cast<Vertex*>(GraphicsDevice::LockVertexBuffer(m_shape_buffer[ST_Plane]));

  float delta_x = 1.0f/slices, delta_y = 1.0f/stacks;
  int index = 0;
  float x, y = 0.5f;

  for (unsigned j = 0; j < stacks; ++j)
  {
    x = -0.5f;
    for (unsigned i = 0; i <= slices; ++i)
    {
      plane_buffer[index].m_tex_coord = Vector2(x + 0.5f, -y + 0.5f);
      plane_buffer[index].m_color     = Color(0.0f, 0.0f, 0.0f, 1.0f);
      plane_buffer[index].m_normal    = Vector3(0.0f, 0.0f, 1.0f);
      plane_buffer[index].m_position  = Vector3(x, y, 0.0f);

      ++index;

      plane_buffer[index].m_tex_coord = Vector2(x + 0.5f, -y - delta_y + 0.5f);
      plane_buffer[index].m_color     = Color(0.0f, 0.0f, 0.0f, 1.0f);
      plane_buffer[index].m_normal    = Vector3(0.0f, 0.0f, 1.0f);
      plane_buffer[index].m_position  = Vector3(x, y - delta_y, 0.0f);

      ++index;
      x += delta_x;
    }
    y -= delta_y;
  }

  // Generate the Cube
  m_shape_buffer[ST_Cube] = GraphicsDevice::CreateVertexBuffer(6 * 2 * (stacks) * (slices + 1),
                                                               0, false, m_declaration);

  Vertex *cube_buffer = reinterpret_cast<Vertex*>(GraphicsDevice::LockVertexBuffer(m_shape_buffer[ST_Cube]));

  index = 0;
  y = 0.5f;

  for (unsigned j = 0; j < stacks; ++j)
  {
    x = -0.5f;
    for (unsigned i = 0; i <= slices; ++i)
    {
      int index_side[6];
      for (int k = 0; k < 6; ++k)
      {
        index_side[k] = index + (k * 2 * stacks * (slices + 1));
        cube_buffer[index_side[k]].m_tex_coord = Vector2(x + 0.5f, -y + 0.5f);
        cube_buffer[index_side[k]].m_color     = Color(0.0f, 0.0f, 0.0f, 1.0f);
      }

      cube_buffer[index_side[0]].m_normal = Vector3( 0.0f,  0.0f,  1.0f);
      cube_buffer[index_side[1]].m_normal = Vector3( 0.0f,  0.0f, -1.0f);
      cube_buffer[index_side[2]].m_normal = Vector3( 0.0f,  1.0f,  0.0f);
      cube_buffer[index_side[3]].m_normal = Vector3( 0.0f, -1.0f,  0.0f);
      cube_buffer[index_side[4]].m_normal = Vector3( 1.0f,  0.0f,  0.0f);
      cube_buffer[index_side[5]].m_normal = Vector3(-1.0f,  0.0f,  0.0f);

      cube_buffer[index_side[0]].m_position = Vector3(    x,     y, -0.5f);
      cube_buffer[index_side[1]].m_position = Vector3(   -x,     y,  0.5f);
      cube_buffer[index_side[2]].m_position = Vector3(   -x,  0.5f,     y);
      cube_buffer[index_side[3]].m_position = Vector3(    x, -0.5f,     y);
      cube_buffer[index_side[4]].m_position = Vector3( 0.5f,     y,     x);
      cube_buffer[index_side[5]].m_position = Vector3(-0.5f,     y,    -x);
      
      ++index;
      for (int k = 0; k < 6; ++k)
      {
        index_side[k] = index + (k * 2 * stacks * (slices + 1));
        cube_buffer[index_side[k]].m_tex_coord = Vector2(x + 0.5f, -y + delta_y + 0.5f);
        cube_buffer[index_side[k]].m_color     = Color(0.0f, 0.0f, 0.0f, 1.0f);
      }

      cube_buffer[index_side[0]].m_normal = Vector3( 0.0f,  0.0f,  1.0f);
      cube_buffer[index_side[1]].m_normal = Vector3( 0.0f,  0.0f, -1.0f);
      cube_buffer[index_side[2]].m_normal = Vector3( 0.0f,  1.0f,  0.0f);
      cube_buffer[index_side[3]].m_normal = Vector3( 0.0f, -1.0f,  0.0f);
      cube_buffer[index_side[4]].m_normal = Vector3( 1.0f,  0.0f,  0.0f);
      cube_buffer[index_side[5]].m_normal = Vector3(-1.0f,  0.0f,  0.0f);

      cube_buffer[index_side[0]].m_position = Vector3(    x, y - delta_y,       -0.5f);
      cube_buffer[index_side[1]].m_position = Vector3(   -x, y - delta_y,        0.5f);
      cube_buffer[index_side[2]].m_position = Vector3(   -x,        0.5f, y - delta_y);
      cube_buffer[index_side[3]].m_position = Vector3(    x,       -0.5f, y - delta_y);
      cube_buffer[index_side[4]].m_position = Vector3( 0.5f, y - delta_y,           x);
      cube_buffer[index_side[5]].m_position = Vector3(-0.5f, y - delta_y,          -x);

      ++index;
      x += delta_x;
    }
    y -= delta_y;
  }

  // Generate the Cone
  float z = 0.0f;

  // Generate the Cylinder

  // Generate the Sphere
  m_shape_buffer[ST_Sphere] = GraphicsDevice::CreateVertexBuffer( 2 * (stacks) * (slices + 1), 
                                                                  0, false, m_declaration);

  Vertex* sphere_buffer = reinterpret_cast<Vertex*>(GraphicsDevice::LockVertexBuffer(m_shape_buffer[ST_Sphere]));

  index = 0;

  for (unsigned j = 0; j < stacks; ++j)
  {
    float a_1 = (static_cast<float>(j) / static_cast<float>(stacks)) * k_pi;
    float a_2 = ((static_cast<float>(j) + 1.0f) / static_cast<float>(stacks)) * k_pi;

    for (unsigned i = 0; i <= slices; ++i)
    {
      float b = (static_cast<float>(i) / static_cast<float>(slices)) * 2 * k_pi;
      
      x = sin(a_1) * cos(b);
      y = cos(a_1);
      z = sin(a_1) * sin(b);

      sphere_buffer[index].m_tex_coord = Vector2(static_cast<float>(i) / static_cast<float>(slices), 
                                                 static_cast<float>(j) / static_cast<float>(stacks));
      sphere_buffer[index].m_color = Color(0.0f, 0.0f, 0.0f, 1.0f);
      sphere_buffer[index].m_normal = Vector3(x, y, z);
      sphere_buffer[index].m_position = Vector3(x / 2.0f, y / 2.0f, z / 2.0f);

      ++index;

      x = sin(a_2) * cos(b);
      y = cos(a_2);
      z = sin(a_2) * sin(b);

      sphere_buffer[index].m_tex_coord = Vector2(static_cast<float>(i) / static_cast<float>(slices), 
                                                (static_cast<float>(j) + 1.0f) / static_cast<float>(stacks));
      sphere_buffer[index].m_color = Color(0.0f, 0.0f, 0.0f, 1.0f);
      sphere_buffer[index].m_normal = Vector3(x, y, z);
      sphere_buffer[index].m_position = Vector3(x / 2.0f, y / 2.0f, z / 2.0f);

      ++index;
    }
  }

  // Generate the Taurus


  // Generate tangents and bitangents


  // Unlock all of the vertex buffers
  for (int i = 0; i < ST_Max; ++i)
  {
    GraphicsDevice::UnlockVertexBuffer(m_shape_buffer[i]);
  }
}

void Shape::Draw(ShapeType shape_type)
{

  // the -2's in each draw call is because it wants the number of triangles to 
  // draw, not the number of verts, and because they are triangle strips there
  // are num_verts - 2 triangles.
  switch (shape_type)
  {
  case ST_Plane:
    GraphicsDevice::SetVertexBuffer(m_shape_buffer[ST_Plane]);
    for (unsigned i = 0; i < m_stacks; ++i)
      GraphicsDevice::DrawTriStripBuffer(2 * (m_slices + 1) - 2, i * 2 * (m_slices + 1));
    break;
  case ST_Cube:
    GraphicsDevice::SetVertexBuffer(m_shape_buffer[ST_Cube]);
    for (unsigned i = 0; i < m_stacks * 6; ++i)
      GraphicsDevice::DrawTriStripBuffer(2 * (m_slices + 1) - 2, i * 2 * (m_slices + 1));
    break;
  case ST_Sphere:
    GraphicsDevice::SetVertexBuffer(m_shape_buffer[ST_Sphere]);
    for (unsigned i = 0; i < m_stacks; ++i)
      GraphicsDevice::DrawTriStripBuffer(2 * (m_slices + 1) - 2, i * 2 * (m_slices + 1));
    break;
  default:
    ErrorIf(true, "Did not pass a valid shape type to draw");
  }
}

void Shape::Uninitialize()
{
  ErrorIf(m_slices == 0, "Can't uninitialize when the shape library is not initialized");

  m_slices = 0;
  m_stacks = 0;
  
  for (unsigned i = 0; i < ST_Max; ++i)
  {
    GraphicsDevice::DeleteVertexBuffer(m_shape_buffer[i]);
  }

  GraphicsDevice::DeleteVertexDeclaration(m_declaration);
}

}