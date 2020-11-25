// Nicholas Baldwin
// 2012/06/05
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "LightComponent.h"
#include "GraphicsDevice.h"
#include "..\Tile\Tilemap.h"
#include "..\Core\EngineCore.h"
#include "..\Composition\composition.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "..\FileIO\Serializer.h"
#include "Camera.h"
#include "RenderTarget.h"

namespace Improbability
{
  
MakeMeta(LightComponent);

struct Vertex
{
  Vector3 m_position;
};

void LightComponent::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

void LightComponent::Initialize()
{
  // create mesh
  GraphicsVertexTypes::VertexElement elements[1];

  elements[0].m_stream = 0;
  elements[0].m_type = GraphicsVertexTypes::VM_Vector3;
  elements[0].m_usage = GraphicsVertexTypes::MU_Position;

  m_declaration = GraphicsDevice::CreateVertexDeclaration(elements, 1);

  m_mesh = GraphicsDevice::CreateVertexBuffer(12, 0, false, m_declaration);

  Vertex *verts = reinterpret_cast<Vertex*>(GraphicsDevice::LockVertexBuffer(m_mesh));

  verts[0].m_position  = Vector3(-0.5f,  0.5f, 0.0f);
  verts[1].m_position  = Vector3( 0.5f,  0.5f, 0.0f);
  verts[2].m_position  = Vector3(-0.5f, -0.5f, 0.0f);
  verts[3].m_position  = Vector3( 0.5f, -0.5f, 0.0f);

  verts[4].m_position  = Vector3(-0.5f, 0.0f, -0.5f);
  verts[5].m_position  = Vector3( 0.5f, 0.0f, -0.5f);
  verts[6].m_position  = Vector3(-0.5f, 0.0f,  0.5f);
  verts[7].m_position  = Vector3( 0.5f, 0.0f,  0.5f);

  verts[8].m_position  = Vector3(0.0f,  0.5f,  0.5f);
  verts[9].m_position  = Vector3(0.0f,  0.5f, -0.5f);
  verts[10].m_position = Vector3(0.0f, -0.5f,  0.5f);
  verts[11].m_position = Vector3(0.0f, -0.5f, -0.5f);

  GraphicsDevice::UnlockVertexBuffer(m_mesh);
}

void LightComponent::Uninitialize()
{
  GraphicsDevice::DeleteVertexBuffer(m_mesh);
  GraphicsDevice::DeleteVertexDeclaration(m_declaration); 
}

Component* LightComponent::Clone()
{
  Component *comp = new LightComponent(*this);
  return comp;
}

void LightComponent::Deserialize(Serializer &serializer)
{
  Read(serializer, m_radius, "Radius");
  Read(serializer, m_diffuse_color, "Diffuse");
}

void LightComponent::Update(float dt, unsigned axis)
{
  m_axis = axis;

  int focus_layer = static_cast<TilePos3>(g_engine_core->SystemCall("TileSystem", "GetFocusPosition")).m_a[axis];
  int layer = int(GetParent()->GetPosition().m_a[axis]);

  m_draw = layer == focus_layer;
}

void LightComponent::Draw()
{
  if (m_draw)
  {
    VertexShader *light_shader = VertexShader::GetShader("light_vertex");
    light_shader->Set();
    PixelShader *deferred_shader = PixelShader::GetShader("sphere_light");
    deferred_shader->Set();

    Vector4 light_offset(0.0f, 0.0f, 0.0f, 1.0f);
    if (m_axis == 2)
      light_offset.m_z = 0.5f;
    else if (m_axis == 1)
      light_offset.m_y = 0.5f;
    else
      light_offset.m_x = 0.5f;

    static_cast<RenderTarget*>(g_engine_core->SystemCall("GraphicsSystem", "GetGBuffer"))->SetUniformTexture("s_g_buffer");
    deferred_shader->SetConstant("u_light_position", GraphicsDevice::GetViewStack().Top() * GraphicsDevice::GetWorldStack().Top() * light_offset);
    deferred_shader->SetConstant("u_light_diffuse", m_diffuse_color);
    deferred_shader->SetConstant("u_light_attenuation", Vector3(m_radius, 2.0f / m_radius, 1.0f / (m_radius * m_radius)));

    GraphicsDevice::SetAlphaEnable(true);

    GraphicsDevice::SetVertexBuffer(m_mesh);
    if (m_axis == 2)
      GraphicsDevice::DrawTriStripBuffer(2, 0);
    else if (m_axis == 1)
      GraphicsDevice::DrawTriStripBuffer(2, 4);
    else
      GraphicsDevice::DrawTriStripBuffer(2, 8);

    deferred_shader->Unset();
    light_shader->Unset();
  }
}

unsigned LightComponent::GetDrawOrder(Camera const * cam)
{
  Matrix4 model_view = GraphicsDevice::GetViewStack().Top() * 
                       GraphicsDevice::GetWorldStack().Top() * 
                       GetWorldTransform();

  Vector4 origin = model_view * Vector4(0.0f, 0.0f, 0.0f, 1.0f);

  float camera_depth = origin.m_z;

  float depth = (camera_depth - cam->GetNear()) / (cam->GetFar() - cam->GetNear());

  return unsigned(UINT_MAX * depth); // return the normalized depth
}

Matrix4 LightComponent::GetWorldTransform()
{
  Vector4 pos = GetParent()->GetPosition();

  Matrix4 scale = Matrix4::CreateScaling(2.0f * m_radius);

  if (m_axis == 2)
    return Matrix4::CreateTranslation(pos.m_x, pos.m_y, float(int(pos.m_z)) + 0.001f) * scale;
  else if (m_axis == 1)
    return Matrix4::CreateTranslation(pos.m_x, float(int(pos.m_y)) + 0.01f, pos.m_z) * scale;
  else
    return Matrix4::CreateTranslation(float(int(pos.m_x)) + 0.01f, pos.m_y, pos.m_z) * scale;
}

}