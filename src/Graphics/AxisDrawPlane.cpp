// Nicholas Baldwin
// 2012/02/17
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "AxisDrawPlane.h"
#include "GraphicsVertexTypes.h"
#include "GraphicsDevice.h"
#include "PixelShader.h"

namespace Improbability
{

AxisDrawPlane::AxisDrawPlane()
{
  GraphicsVertexTypes::VertexElement vertex_decl[3];

  vertex_decl[0].m_stream = 0;
  vertex_decl[0].m_type   = GraphicsVertexTypes::VM_Vector3;
  vertex_decl[0].m_usage  = GraphicsVertexTypes::MU_Position;
  vertex_decl[1].m_stream = 0;
  vertex_decl[1].m_type   = GraphicsVertexTypes::VM_Vector3;
  vertex_decl[1].m_usage  = GraphicsVertexTypes::MU_Normal;
  vertex_decl[2].m_stream = 0;
  vertex_decl[2].m_type   = GraphicsVertexTypes::VM_Vector2;
  vertex_decl[2].m_usage  = GraphicsVertexTypes::MU_TexCoord;

  m_vertex_declaration = GraphicsDevice::CreateVertexDeclaration(vertex_decl, 3);

  m_vertex_buffer = GraphicsDevice::CreateVertexBuffer(12, 0, false, m_vertex_declaration);
  Vertex *vertex_buffer = reinterpret_cast<Vertex*>(GraphicsDevice::LockVertexBuffer(m_vertex_buffer));

  vertex_buffer[0].m_position    = Vector3( 0.5f,  0.5f, 0.0f); // top-right
  vertex_buffer[0].m_normal      = Vector3( 0.0f,  0.0f, 1.0f);
  vertex_buffer[0].m_tex_coords  = Vector2( 1.0f,  0.0f);
  vertex_buffer[1].m_position    = Vector3( 0.5f,  0.0f, 0.0f); // center-right
  vertex_buffer[1].m_normal      = Vector3( 0.0f,  0.0f, 1.0f);
  vertex_buffer[1].m_tex_coords  = Vector2( 1.0f,  0.5f);
  vertex_buffer[2].m_position    = Vector3( 0.0f,  0.5f, 0.0f); // top-center
  vertex_buffer[2].m_normal      = Vector3( 0.0f,  0.0f, 1.0f);
  vertex_buffer[2].m_tex_coords  = Vector2( 0.5f,  0.0f);
  vertex_buffer[3].m_position    = Vector3( 0.0f,  0.0f, 0.0f); // center
  vertex_buffer[3].m_normal      = Vector3( 0.0f,  0.0f, 1.0f);
  vertex_buffer[3].m_tex_coords  = Vector2( 0.5f,  0.5f);
  vertex_buffer[4].m_position    = Vector3(-0.5f,  0.5f, 0.0f); // top-left
  vertex_buffer[4].m_normal      = Vector3( 0.0f,  0.0f, 1.0f);
  vertex_buffer[4].m_tex_coords  = Vector2( 0.0f,  0.0f);
  vertex_buffer[5].m_position    = Vector3(-0.5f,  0.0f, 0.0f); // center-left
  vertex_buffer[5].m_normal      = Vector3( 0.0f,  0.0f, 1.0f);
  vertex_buffer[5].m_tex_coords  = Vector2( 0.0f,  0.5f);
  vertex_buffer[6].m_position    = Vector3( 0.5f,  0.0f, 0.0f); // center-right
  vertex_buffer[6].m_normal      = Vector3( 0.0f,  0.0f, 1.0f);
  vertex_buffer[6].m_tex_coords  = Vector2( 1.0f,  0.5f);
  vertex_buffer[7].m_position    = Vector3( 0.5f, -0.5f, 0.0f); // bottom-right
  vertex_buffer[7].m_normal      = Vector3( 0.0f,  0.0f, 1.0f);
  vertex_buffer[7].m_tex_coords  = Vector2( 1.0f,  1.0f);
  vertex_buffer[8].m_position    = Vector3( 0.0f,  0.0f, 0.0f); // center
  vertex_buffer[8].m_normal      = Vector3( 0.0f,  0.0f, 1.0f);
  vertex_buffer[8].m_tex_coords  = Vector2( 0.5f,  0.5f);
  vertex_buffer[9].m_position    = Vector3( 0.0f, -0.5f, 0.0f); // bottom-center
  vertex_buffer[9].m_normal      = Vector3( 0.0f,  0.0f, 1.0f);
  vertex_buffer[9].m_tex_coords  = Vector2( 0.5f,  1.0f);
  vertex_buffer[10].m_position   = Vector3(-0.5f,  0.0f, 0.0f); // center-left
  vertex_buffer[10].m_normal     = Vector3( 0.0f,  0.0f, 1.0f);
  vertex_buffer[10].m_tex_coords = Vector2( 0.0f,  0.5f);
  vertex_buffer[11].m_position   = Vector3(-0.5f, -0.5f, 0.0f); // bottom-left
  vertex_buffer[11].m_normal     = Vector3( 0.0f,  0.0f, 1.0f);
  vertex_buffer[11].m_tex_coords = Vector2( 0.0f,  1.0f);

  GraphicsDevice::UnlockVertexBuffer(m_vertex_buffer);

  m_render_target = GraphicsDevice::CreateRenderTarget(1.0f, 1.0f, (GraphicsOptionTypes::Usage)0);
}

AxisDrawPlane::~AxisDrawPlane()
{
  GraphicsDevice::DeleteRenderTarget(m_render_target);
  GraphicsDevice::DeleteVertexBuffer(m_vertex_buffer);
  GraphicsDevice::DeleteVertexDeclaration(m_vertex_declaration);
}

void AxisDrawPlane::SetRenderTarget()
{
  GraphicsDevice::SetRenderTarget(m_render_target, 0);
}

void AxisDrawPlane::UnsetRenderTarget()
{
  GraphicsDevice::UnsetRenderTarget();
}

void AxisDrawPlane::Draw(unsigned sub_quad)
{
  GraphicsDevice::GetWorldStack().Push();
  GraphicsDevice::GetWorldStack().LoadMatrix(m_world_transform);
  float mag = 30.0f / 1000.0f;
  Matrix4 scale = Matrix4::CreateScaling(Vector4(float(GraphicsDevice::GetDeviceWidth()) * mag, float(GraphicsDevice::GetDeviceHeight()) * mag, 1.0f, 0.0f));
  GraphicsDevice::GetWorldStack().LoadMatrix(scale);

  GraphicsDevice::CommitModelViewMatrix();

  PixelShader::GetActiveShader()->SetSampler("s_sampler", GraphicsDevice::GetRenderTargetTexture(m_render_target));

  GraphicsDevice::SetVertexBuffer(m_vertex_buffer);

  // draw the correct tris of the sub-quad
  unsigned start_tri = sub_quad * 2 + ((sub_quad < 2) ? 0 : 2);
  GraphicsDevice::DrawTriStripBuffer(2, start_tri);

  GraphicsDevice::GetWorldStack().Pop();

  PixelShader::GetActiveShader()->UnsetSampler("s_sampler");
}

}