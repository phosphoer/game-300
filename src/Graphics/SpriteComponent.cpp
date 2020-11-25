// Nicholas Baldwin
// 2012/03/20
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "SpriteComponent.h"
#include "..\FileIO\Serializer.h"
#include "..\math\MatrixStack.h"
#include "..\math\Matrix4.h"
#include "GraphicsDevice.h"
#include "..\Composition\Component.h"
#include "..\Composition\Composition.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "Animation.h"
#include "..\Core\EngineCore.h"
#include "..\Resource\ResourceManager.h"
#include "Renderable.h"
#include "..\Tile\Tilemap.h"
#include "Camera.h"

namespace Improbability
{

MakeMeta(SpriteComponent);

struct Vertex
{
  Vector3 m_position;
  Vector2 m_texture;
};

void SpriteComponent::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(SetAnimation);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(SetFrame);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(IncrementFrame);
}

void SpriteComponent::Initialize()
{
  // create mesh
  GraphicsVertexTypes::VertexElement elements[2];

  elements[0].m_stream = elements[1].m_stream = 0;
  elements[0].m_type = GraphicsVertexTypes::VM_Vector3;
  elements[1].m_type = GraphicsVertexTypes::VM_Vector2;
  elements[0].m_usage = GraphicsVertexTypes::MU_Position;
  elements[1].m_usage = GraphicsVertexTypes::MU_TexCoord;

  m_declaration = GraphicsDevice::CreateVertexDeclaration(elements, 2);

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

  verts[0].m_texture  = Vector2(0.0f, 0.0f);
  verts[1].m_texture  = Vector2(1.0f, 0.0f);
  verts[2].m_texture  = Vector2(0.0f, 1.0f / 3.0f);
  verts[3].m_texture  = Vector2(1.0f, 1.0f / 3.0f);

  verts[4].m_texture  = Vector2(0.0f, 1.0f / 3.0f);
  verts[5].m_texture  = Vector2(1.0f, 1.0f / 3.0f);
  verts[6].m_texture  = Vector2(0.0f, 2.0f / 3.0f);
  verts[7].m_texture  = Vector2(1.0f, 2.0f / 3.0f);

  verts[8].m_texture  = Vector2(0.0f, 2.0f / 3.0f);
  verts[9].m_texture  = Vector2(1.0f, 2.0f / 3.0f);
  verts[10].m_texture = Vector2(0.0f, 1.0f);
  verts[11].m_texture = Vector2(1.0f, 1.0f);

  GraphicsDevice::UnlockVertexBuffer(m_mesh);

  Animation *animation = g_engine_core->GetResourceManager()->GetResource<Animation>(m_animation_name);
  m_track = animation->GetCurrentTrack();
  m_frame = animation->GetCurrentFrame();

  m_fade_time[0] = 0.0f; m_fade_time[1] = 0.0f; m_fade_time[2] = 0.0f;
  m_value[0] = 0.0f;     m_value[1] = 0.0f;     m_value[2] = 0.0f;
  m_a_value[0] = 0.0f;   m_a_value[1] = 0.0f;   m_a_value[2] = 0.0f;
  m_axis = 0;
}

void SpriteComponent::Uninitialize()
{
  GraphicsDevice::DeleteVertexBuffer(m_mesh);
  GraphicsDevice::DeleteVertexDeclaration(m_declaration);
}

Component* SpriteComponent::Clone()
{
  Component *clone = new SpriteComponent(*this); 
  return clone;
}

void SpriteComponent::Deserialize(Serializer &serializer)
{
  Read(serializer, m_animation_name, "Animation");
}

void SpriteComponent::Update(float dt, unsigned axis)
{
  using namespace SpriteConstants;

  m_axis = axis;

  if (m_fade_time[m_axis] < k_total_fade_time)
  {
    m_fade_time[m_axis] += dt;

    int anchor_axis = g_engine_core->SystemCall("TileSystem", "GetAnchorAxis");
    int focus_layer = static_cast<TilePos3>(g_engine_core->SystemCall("TileSystem", "GetFocusPosition")).m_a[axis];
    int layer = int(GetParent()->GetPosition().m_a[axis]);
    int dl;

    if (anchor_axis == m_axis)
    {
      int anchor_begin = g_engine_core->SystemCall("TileSystem", "GetAnchorBegin");
      int anchor_end = g_engine_core->SystemCall("TileSystem", "GetAnchorEnd");

      if (anchor_begin < anchor_end)
        std::swap(anchor_begin, anchor_end);

      if (anchor_end >= 0 && focus_layer >= anchor_end && focus_layer <= anchor_begin)
      {
        if (layer > anchor_begin)
        {
          // memory layer, outside of anchor layers
          dl = abs(layer - anchor_begin);
        }
        else if (layer < anchor_end)
        {
          // memory layer, outside of anchor layers
          dl = abs(layer - anchor_end);
        }
        // Sprite is anchored
        else
        {
          m_value[m_axis] = 1.0f;
          m_a_value[m_axis] = 1.0f;

          return;
        }
      }
      else
      {
        // memory layer
        dl = abs(layer - focus_layer);
      }
    }
    else
    {
      // memory layer
      dl = abs(layer - focus_layer);
    }

    int layerCount = g_engine_core->SystemCall("TileSystem", "GetMemoryRange");
    float percentageOfMaxDistance = float(dl) / float(layerCount);
    float adjustedFadeDurationRange = k_total_fade_time - k_min_fade_time;
    float actualTotalFadeTime = adjustedFadeDurationRange * percentageOfMaxDistance + k_min_fade_time;
    float currentProgress = min(1.0f, m_fade_time[m_axis] / actualTotalFadeTime);

    m_value[m_axis] = 1 - percentageOfMaxDistance;
    m_a_value[m_axis] = currentProgress * m_value[m_axis];
  }
}

void SpriteComponent::Draw()
{
  SetUpShaders();

  Animation *animation = g_engine_core->GetResourceManager()->GetResource<Animation>(m_animation_name);
  animation->SetTrack(m_track);
  animation->SetFrame(m_frame);
  animation->Set();

  GraphicsDevice::SetAlphaEnable(true);

  GraphicsDevice::SetVertexBuffer(m_mesh);
  if (m_axis == 2)
    GraphicsDevice::DrawTriStripBuffer(2, 0);
  else if (m_axis == 1)
    GraphicsDevice::DrawTriStripBuffer(2, 4);
  else
    GraphicsDevice::DrawTriStripBuffer(2, 8);

  animation->Unset();

  VertexShader::GetActiveShader()->Unset();
  PixelShader::GetActiveShader()->Unset();
}

unsigned SpriteComponent::GetDrawOrder(Camera const *cam)
{
  Matrix4 model_view = GraphicsDevice::GetViewStack().Top() * 
                       GraphicsDevice::GetWorldStack().Top() * 
                       GetWorldTransform();

  Vector4 origin = model_view * Vector4(0.0f, 0.0f, 0.0f, 1.0f);

  float camera_depth = origin.m_z;

  float depth = (camera_depth - cam->GetNear()) / (cam->GetFar() - cam->GetNear());

  return unsigned(UINT_MAX * depth); // return the normalized depth
}

Matrix4 SpriteComponent::GetWorldTransform()
{
  Vector4 pos = GetParent()->GetPosition();

  if (m_axis == 2)
    return Matrix4::CreateTranslation(pos.m_x, pos.m_y, float(int(pos.m_z)) - 0.001f);
  else if (m_axis == 1)
    return Matrix4::CreateTranslation(pos.m_x, float(int(pos.m_y)) - 0.01f, pos.m_z);
  else
    return Matrix4::CreateTranslation(float(int(pos.m_x)) - 0.01f, pos.m_y, pos.m_z);
}

void SpriteComponent::SetAnimation(unsigned i)
{
  ErrorIf(int(i) >= g_engine_core->GetResourceManager()->GetResource<Animation>(m_animation_name)->GetTrackCount(),
          "Attempted to set a bad animation");

  m_track = i;
}

void SpriteComponent::SetFrame(unsigned i)
{
  ErrorIf(int(i) >= g_engine_core->GetResourceManager()->GetResource<Animation>(m_animation_name)->GetFrameCountInTrack(m_track),
          "Attempted to set a bad frame");

  m_frame = i;
}

void SpriteComponent::IncrementFrame()
{
  m_frame = (m_frame + 1) % g_engine_core->GetResourceManager()->GetResource<Animation>(m_animation_name)->GetFrameCountInTrack(m_track);
}

int SpriteComponent::GetCurrentAnimation()
{
  return m_track;
}

int SpriteComponent::GetCurrentFrame()
{
  return m_frame;
}

void SpriteComponent::SetUpShaders()
{
  VertexShader *animation_vertex = VertexShader::GetShader("animation");

  animation_vertex->Set();

  int anchor_axis = g_engine_core->SystemCall("TileSystem", "GetAnchorAxis");
  int focus_layer = static_cast<TilePos3>(g_engine_core->SystemCall("TileSystem", "GetFocusPosition")).m_a[m_axis];
  int layer = int(GetParent()->GetPosition().m_a[m_axis]);
  int dl;
  PixelShader *pixel;

  if (anchor_axis == m_axis)
  {
    int anchor_begin = g_engine_core->SystemCall("TileSystem", "GetAnchorBegin");
    int anchor_end = g_engine_core->SystemCall("TileSystem", "GetAnchorEnd");

    if (anchor_begin < anchor_end)
      std::swap(anchor_begin, anchor_end);

    if (anchor_end >= 0 && focus_layer >= anchor_end && focus_layer <= anchor_begin)
    {
      if (layer > anchor_begin)
      {
        // memory layer, outside of anchor layers
        dl = layer - anchor_begin;
      }
      else if (layer < anchor_end)
      {
        // memory layer, outside of anchor layers
        dl = layer - anchor_end;
      }
      // Sprite is anchored
      else
      {
        // anchor'd sprite
        int obscure_layer = g_engine_core->SystemCall("TileSystem", "GetObscured");

        if (obscure_layer > -1 && layer - obscure_layer >= 0)
        {
          pixel = PixelShader::GetShader("anchor_obscured_sprite");
          pixel->Set();

          pixel->SetConstant("u_alpha", m_a_value[m_axis]);
        }
        else
        {
          pixel = PixelShader::GetShader("textured");
          pixel->Set();
        }

        return;
      }
    }
    else
    {
      // memory layer
      dl = layer - focus_layer;
    }
  }
  else
  {
    // memory layer
    dl = layer - focus_layer;
  }

  if (dl < 0)
  {
    pixel = PixelShader::GetShader("back_sprite");
    pixel->Set();

    pixel->SetConstant("u_lightness", m_value[m_axis]);
    pixel->SetConstant("u_alpha", m_a_value[m_axis]);
  }
  else if (dl > 0)
  {
    pixel = PixelShader::GetShader("front_sprite");
    pixel->Set();

    pixel->SetConstant("u_lightness", m_value[m_axis]);
    pixel->SetConstant("u_alpha", m_a_value[m_axis]);
  }
  else
  {
    pixel = PixelShader::GetShader("textured");

    pixel->Set();
  }
}

}