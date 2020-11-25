// Nicholas Baldwin
// 2011/08/05
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "DebugDraw.h"
#include "GraphicsVertexTypes.h"
#include "GraphicsDevice.h"
#include "VertexShader.h"
#include "PixelShader.h"

namespace Improbability
{

BufferHandle DebugDraw::m_debug_shapes[DS_Max];
DeclarationHandle DebugDraw::m_declaration;
std::vector<DebugDraw::DebugDrawObject*> DebugDraw::m_static_queue[DS_Max];
HashMap<String, DebugDraw::DebugDrawObject*> DebugDraw::m_registered_queue[DS_Max];
bool DebugDraw::m_is_initialized = false;
bool DebugDraw::m_draw_enabled = false;
static const unsigned num_segments = 30;

DebugDraw::DebugDrawObject* DebugDraw::AddArrow(Vector3 position, Vector3 alignment_vector, float scale)
{
  if (!m_is_initialized || !m_draw_enabled)
    return 0;

  alignment_vector.Normalize();
  Vector3 B(alignment_vector.m_y, alignment_vector.m_x, 0.0f);
  Vector3 C = alignment_vector.Cross(B);

  // create a rotation quaternion that takes the arrow to the alignment vector
  Matrix4 rotation = Matrix4::CreateOrthogonal(Vector4::Vector(alignment_vector), Vector4::Vector(B), -Vector4::Vector(C));

  DebugDrawObject* new_member = new DebugDrawObject;
  new_member->m_position = new Vector3(position);
  new_member->m_orientation = new Matrix4(rotation);
  new_member->m_scale = new Vector3(scale, scale, scale);
  new_member->m_color = Color(0.0f, 0.0f, 0.0f, 1.0f);
  new_member->m_lifetime = 0.0001f;

  m_static_queue[DS_Arrow].push_back(new_member);

  return new_member;
}

DebugDraw::DebugDrawObject* DebugDraw::AddCoordinates(Vector3 position, Matrix4 const &rotation, float scale)
{
  if (!m_is_initialized || !m_draw_enabled)
    return 0;

  DebugDrawObject* new_member = new DebugDrawObject;
  new_member->m_position = new Vector3(position);
  new_member->m_orientation = new Matrix4(rotation);
  new_member->m_scale = new Vector3(scale, scale, scale);
  new_member->m_color = Color(0.0f, 0.0f, 0.0f, 1.0f);
  new_member->m_lifetime = 0.0001f;

  m_static_queue[DS_Coordinates].push_back(new_member);

  return new_member;
}

DebugDraw::DebugDrawObject* DebugDraw::AddQuad(Vector3 position, Matrix4 const &rotation, Vector2 scale)
{
  if (!m_is_initialized || !m_draw_enabled)
    return 0;

  DebugDrawObject* new_member = new DebugDrawObject;
  new_member->m_position = new Vector3(position);
  new_member->m_orientation = new Matrix4(rotation);
  new_member->m_scale = new Vector3(scale.m_x, scale.m_y, 1.0f);
  new_member->m_color = Color(0.0f, 0.0f, 0.0f, 1.0f);
  new_member->m_lifetime = 0.0001f;

  m_static_queue[DS_Quad].push_back(new_member);

  return new_member;
}

DebugDraw::DebugDrawObject* DebugDraw::AddCube(Vector3 position, Matrix4 const &rotation, Vector3 scale)
{
  if (!m_is_initialized || !m_draw_enabled)
    return 0;

  DebugDrawObject* new_member = new DebugDrawObject;
  new_member->m_position = new Vector3(position);
  new_member->m_orientation = new Matrix4(rotation);
  new_member->m_scale = new Vector3(scale);
  new_member->m_color = Color(0.0f, 0.0f, 0.0f, 1.0f);
  new_member->m_lifetime = 0.0001f;

  m_static_queue[DS_Cube].push_back(new_member);

  return new_member;
}

void DebugDraw::Initialize()
{
  m_is_initialized = true;

  // create declaration
  GraphicsVertexTypes::VertexElement elements[2];

  elements[0].m_stream = 0;
  elements[0].m_type = GraphicsVertexTypes::VM_Vector3;
  elements[0].m_usage = GraphicsVertexTypes::MU_Position;

  elements[1].m_stream = 0;
  elements[1].m_type = GraphicsVertexTypes::VM_Color;
  elements[1].m_usage = GraphicsVertexTypes::MU_Color;

  m_declaration = GraphicsDevice::CreateVertexDeclaration(elements, 2);

  // create Arrow
  m_debug_shapes[DS_Arrow] = GraphicsDevice::CreateVertexBuffer(8, 0, false, m_declaration);

  Vertex *arrow_buffer = reinterpret_cast<Vertex*>(GraphicsDevice::LockVertexBuffer(m_debug_shapes[DS_Arrow]));

  arrow_buffer[0].m_position = Vector3(0.0f, 0.0f, 0.0f);
  arrow_buffer[0].m_color    = Color(0.0f, 0.0f, 0.0f, 1.0f);

  arrow_buffer[1].m_position = Vector3(0.5f, 0.0f, 0.0f);
  arrow_buffer[1].m_color    = Color(0.0f, 0.0f, 0.0f, 1.0f);

  arrow_buffer[2].m_position = Vector3(0.45f, 0.025f, 0.0f);
  arrow_buffer[2].m_color    = Color(0.0f, 0.0f, 0.0f, 1.0f);

  arrow_buffer[3].m_position = Vector3(0.45f, -0.025f, 0.0f);
  arrow_buffer[3].m_color    = Color(0.0f, 0.0f, 0.0f, 1.0f);

  arrow_buffer[4].m_position = Vector3(0.5f, 0.0f, 0.0f);
  arrow_buffer[4].m_color    = Color(0.0f, 0.0f, 0.0f, 1.0f);

  arrow_buffer[5].m_position = Vector3(0.45f, 0.0f, 0.025f);
  arrow_buffer[5].m_color    = Color(0.0f, 0.0f, 0.0f, 1.0f);

  arrow_buffer[6].m_position = Vector3(0.45f, 0.0f, -0.025f);
  arrow_buffer[6].m_color    = Color(0.0f, 0.0f, 0.0f, 1.0f);

  arrow_buffer[7].m_position = Vector3(0.5f, 0.0f, 0.0f);
  arrow_buffer[7].m_color    = Color(0.0f, 0.0f, 0.0f, 1.0f);

  GraphicsDevice::UnlockVertexBuffer(m_debug_shapes[DS_Arrow]);
  arrow_buffer = 0;

  // create the Coordinates
  m_debug_shapes[DS_Coordinates] = GraphicsDevice::CreateVertexBuffer(6, 0, false, m_declaration);

  Vertex *coordinates_buffer = reinterpret_cast<Vertex*>(GraphicsDevice::LockVertexBuffer(m_debug_shapes[DS_Coordinates]));

  coordinates_buffer[0].m_position = coordinates_buffer[2].m_position = coordinates_buffer[4].m_position = Vector3(0.0f, 0.0f, 0.0f);

  coordinates_buffer[1].m_position = Vector3(0.5f, 0.0f, 0.0f);
  coordinates_buffer[0].m_color = coordinates_buffer[1].m_color = Color(1.0f, 0.0f, 0.0f);

  coordinates_buffer[3].m_position = Vector3(0.0f, 0.5f, 0.0f);
  coordinates_buffer[2].m_color = coordinates_buffer[3].m_color = Color(0.0f, 1.0f, 0.0f);
  
  coordinates_buffer[5].m_position = Vector3(0.0f, 0.0f, 0.5f);
  coordinates_buffer[4].m_color = coordinates_buffer[5].m_color = Color(0.0f, 0.0f, 1.0f);

  GraphicsDevice::UnlockVertexBuffer(m_debug_shapes[DS_Coordinates]);
  coordinates_buffer = 0;

  // create Quad
  m_debug_shapes[DS_Quad] = GraphicsDevice::CreateVertexBuffer(5, 0, false, m_declaration);

  Vertex *quad_buffer = reinterpret_cast<Vertex*>(GraphicsDevice::LockVertexBuffer(m_debug_shapes[DS_Quad]));

  quad_buffer[0].m_position = Vector3(0.5f, 0.5f, 0.0f);
  quad_buffer[0].m_color    = Color(0.0f, 0.0f, 0.0f, 1.0f);

  quad_buffer[1].m_position = Vector3(-0.5f, 0.5f, 0.0f);
  quad_buffer[1].m_color    = Color(0.0f, 0.0f, 0.0f, 1.0f);

  quad_buffer[2].m_position = Vector3(-0.5f, -0.5f, 0.0f);
  quad_buffer[2].m_color    = Color(0.0f, 0.0f, 0.0f, 1.0f);

  quad_buffer[3].m_position = Vector3(0.5f, -0.5f, 0.0f);
  quad_buffer[3].m_color    = Color(0.0f, 0.0f, 0.0f, 1.0f);

  quad_buffer[4].m_position = Vector3(0.5f, 0.5f, 0.0f);
  quad_buffer[4].m_color    = Color(0.0f, 0.0f, 0.0f, 1.0f);

  GraphicsDevice::UnlockVertexBuffer(m_debug_shapes[DS_Quad]);
  quad_buffer = 0;

  //create Cube
  m_debug_shapes[DS_Cube] = GraphicsDevice::CreateVertexBuffer(24, 0, false, m_declaration);

  Vertex *cube_buffer = reinterpret_cast<Vertex*>(GraphicsDevice::LockVertexBuffer(m_debug_shapes[DS_Cube]));

  Vertex temp_vert;

  temp_vert.m_position = Vector3(0.5f, 0.5f, 0.5f);
  temp_vert.m_color    = Color(0.0f, 0.0f, 0.0f, 1.0f);
  cube_buffer[0] = cube_buffer[7] = cube_buffer[8] = temp_vert;

  temp_vert.m_position = Vector3(-0.5f, 0.5f, 0.5f);
  cube_buffer[1] = cube_buffer[2] = cube_buffer[10] = temp_vert;

  temp_vert.m_position = Vector3(-0.5f, -0.5f, 0.5f);
  cube_buffer[3] = cube_buffer[4] = cube_buffer[12] = temp_vert;

  temp_vert.m_position = Vector3(0.5f, -0.5f, 0.5f);
  cube_buffer[5] = cube_buffer[6] = cube_buffer[14] = temp_vert;

  temp_vert.m_position = Vector3(0.5f, 0.5f, -0.5f);
  cube_buffer[9] = cube_buffer[16] = cube_buffer[23] = temp_vert;

  temp_vert.m_position = Vector3(-0.5f, 0.5f, -0.5f);
  cube_buffer[11] = cube_buffer[17] = cube_buffer[18] = temp_vert;
  
  temp_vert.m_position = Vector3(-0.5f, -0.5f, -0.5f);
  cube_buffer[13] = cube_buffer[19] = cube_buffer[20] = temp_vert;

  temp_vert.m_position = Vector3(0.5f, -0.5f, -0.5f);
  cube_buffer[15] = cube_buffer[21] = cube_buffer[22] = temp_vert;

  GraphicsDevice::UnlockVertexBuffer(m_debug_shapes[DS_Cube]);
  cube_buffer = 0;

  //create Circles
  m_debug_shapes[DS_CircleXY] = GraphicsDevice::CreateVertexBuffer(num_segments + 1, 0, false, m_declaration);
  m_debug_shapes[DS_CircleXZ] = GraphicsDevice::CreateVertexBuffer(num_segments + 1, 0, false, m_declaration);
  m_debug_shapes[DS_CircleYZ] = GraphicsDevice::CreateVertexBuffer(num_segments + 1, 0, false, m_declaration);

  Vertex *circle_xy_buffer = reinterpret_cast<Vertex*>(GraphicsDevice::LockVertexBuffer(m_debug_shapes[DS_CircleXY]));
  Vertex *circle_xz_buffer = reinterpret_cast<Vertex*>(GraphicsDevice::LockVertexBuffer(m_debug_shapes[DS_CircleXZ]));
  Vertex *circle_yz_buffer = reinterpret_cast<Vertex*>(GraphicsDevice::LockVertexBuffer(m_debug_shapes[DS_CircleYZ]));

  for (int i = 0; i <= num_segments; ++i)
  {
    float a1 = cos(((float)i/(float)num_segments) * 2 * k_pi), 
          a2 = sin(((float)i/(float)num_segments) * 2 * k_pi);

    circle_xy_buffer[i].m_position = Vector3(a1, a2, 0.0f);
    circle_xy_buffer[i].m_color    = Color(0.0f, 0.0f, 0.0f, 1.0f);

    circle_xz_buffer[i].m_position = Vector3(a1, 0.0f, a2);
    circle_xz_buffer[i].m_color    = Color(0.0f, 0.0f, 0.0f, 1.0f);

    circle_yz_buffer[i].m_position = Vector3(0.0f, a1, a2);
    circle_yz_buffer[i].m_color    = Color(0.0f, 0.0f, 0.0f, 1.0f);
  }
}

void DebugDraw::DrawQueue(float dt)
{
  if (!m_draw_enabled)
  {
    for (unsigned shape = 0; shape < DS_Max; ++shape)
    {
      for (unsigned i = 0; i < m_static_queue[shape].size(); ++i)
      { 
        m_static_queue[shape][i]->m_lifetime -= dt;
        
        if (m_static_queue[shape][i]->m_lifetime < 0.0f)
        {
          delete m_static_queue[shape][i]->m_position;
          delete m_static_queue[shape][i]->m_orientation;
          delete m_static_queue[shape][i]->m_scale;
          delete m_static_queue[shape][i];
          m_static_queue[shape][i] = m_static_queue[shape].back();
          m_static_queue[shape].pop_back();
          --i;
        }
      }
    }
    return;
  }

  MatrixStack<Matrix4> &world = GraphicsDevice::GetWorldStack();

  VertexShader *basic_vertex = VertexShader::GetShader("Basic");
  basic_vertex->Set();
  PixelShader *color_pixel = PixelShader::GetShader("Color");
  color_pixel->Set();

  // for each shape
  for (unsigned shape = 0; shape < DS_Max; ++shape)
  {
    // coordinates and spheres are exceptions
    if (shape == DS_Coordinates || shape == DS_Sphere)
    {
      continue;
    }

    GraphicsDevice::SetVertexBuffer(m_debug_shapes[shape]);

    // for each instance
    for (unsigned i = 0; i < m_static_queue[shape].size(); ++i)
    {
      world.Push();

      // essentially the model->world matrix
      world.LoadMatrix(Matrix4::CreateTranslation(Vector4::Point(*(m_static_queue[shape][i]->m_position))));
      world.LoadMatrix(*(m_static_queue[shape][i]->m_orientation));
      world.LoadMatrix(Matrix4::CreateScaling(Vector4::Point(*(m_static_queue[shape][i]->m_scale))));
      GraphicsDevice::CommitModelViewMatrix();

      color_pixel->SetConstant("u_fill_color", m_static_queue[shape][i]->m_color);

      Draw(static_cast<DebugShape>(shape));

      world.Pop();
      
      m_static_queue[shape][i]->m_lifetime -= dt;

      if (m_static_queue[shape][i]->m_lifetime < 0.0f)
      {
        delete m_static_queue[shape][i]->m_position;
        delete m_static_queue[shape][i]->m_orientation;
        delete m_static_queue[shape][i]->m_scale;
        delete m_static_queue[shape][i];
        m_static_queue[shape][i] = m_static_queue[shape].back();
        m_static_queue[shape].pop_back();
        --i;
      }
    }
  }

  for (unsigned i = 0; i < m_static_queue[DS_Sphere].size(); ++i)
  {
    world.Push();

    // essentially the model->world matrix
    world.LoadMatrix(Matrix4::CreateTranslation(Vector4::Point(*(m_static_queue[DS_Sphere][i]->m_position))));
    world.LoadMatrix(*(m_static_queue[DS_Sphere][i]->m_orientation));
    world.LoadMatrix(Matrix4::CreateScaling(Vector4::Point(*(m_static_queue[DS_Sphere][i]->m_scale))));

    GraphicsDevice::CommitModelViewMatrix();

    GraphicsDevice::SetVertexBuffer(m_debug_shapes[DS_CircleXY]);

    Draw(DS_Sphere);

    GraphicsDevice::SetVertexBuffer(m_debug_shapes[DS_CircleXZ]);

    Draw(DS_Sphere);

    GraphicsDevice::SetVertexBuffer(m_debug_shapes[DS_CircleYZ]);

    Draw(DS_Sphere);

    world.Pop();

    m_static_queue[DS_Sphere][i]->m_lifetime -= dt;

    if (m_static_queue[DS_Sphere][i]->m_lifetime < 0.0f)
    {
      delete m_static_queue[DS_Sphere][i]->m_position;
      delete m_static_queue[DS_Sphere][i]->m_orientation;
      delete m_static_queue[DS_Sphere][i]->m_scale;
      delete m_static_queue[DS_Sphere][i];
      m_static_queue[DS_Sphere][i] = m_static_queue[DS_Sphere].back();
      m_static_queue[DS_Sphere].pop_back();
      --i;
    }
  }

  for (unsigned i = 0; i < m_static_queue[DS_Coordinates].size(); ++i)
  {
    world.Push();

    // essentially the model->world matrix
    world.LoadMatrix(Matrix4::CreateTranslation(Vector4::Point(*(m_static_queue[DS_Coordinates][i]->m_position))));
    world.LoadMatrix(*(m_static_queue[DS_Coordinates][i]->m_orientation));
    world.LoadMatrix(Matrix4::CreateScaling(Vector4::Point(*(m_static_queue[DS_Coordinates][i]->m_scale))));

    GraphicsDevice::CommitModelViewMatrix();

    GraphicsDevice::SetVertexBuffer(m_debug_shapes[DS_Coordinates]);

    Draw(DS_Coordinates);

    world.Pop();

    m_static_queue[DS_Coordinates][i]->m_lifetime -= dt;

    if (m_static_queue[DS_Coordinates][i]->m_lifetime < 0.0f)
    {
      delete m_static_queue[DS_Coordinates][i]->m_position;
      delete m_static_queue[DS_Coordinates][i]->m_orientation;
      delete m_static_queue[DS_Coordinates][i]->m_scale;
      delete m_static_queue[DS_Coordinates][i];
      m_static_queue[DS_Coordinates][i] = m_static_queue[DS_Coordinates].back();
      m_static_queue[DS_Coordinates].pop_back();
      --i;
    }
  }

  basic_vertex->Unset();
  color_pixel->Unset();
}

void DebugDraw::Draw(DebugShape shape_type)
{
  // draw the shapes (numbers are the number of lines, not number of verts)
  switch (shape_type)
  {
  case DS_Arrow:
    GraphicsDevice::DrawLineStripBuffer(7);
    break;
  case DS_Coordinates:
    GraphicsDevice::DrawLineListBuffer(3);
    break;
  case DS_Quad:
    GraphicsDevice::DrawLineStripBuffer(4);
    break;
  case DS_Cube:
    GraphicsDevice::DrawLineListBuffer(12);
    break;
  case DS_CircleXY:
  case DS_CircleXZ:
  case DS_CircleYZ:
  case DS_Sphere:
    GraphicsDevice::DrawLineStripBuffer(num_segments);
  default:
    ErrorIf(true, "Attempted to draw bad debug shape");
  }
}

void DebugDraw::Uninitialize()
{
  for (unsigned i = 0; i < DS_Max; ++i)
  {
    GraphicsDevice::DeleteVertexBuffer(m_debug_shapes[i]);
  }

  GraphicsDevice::DeleteVertexDeclaration(m_declaration);
}

}