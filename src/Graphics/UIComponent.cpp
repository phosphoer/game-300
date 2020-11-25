// Nicholas Baldwin
// 2011/09/30
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "UIComponent.h"
#include "GraphicsVertexTypes.h"
#include "GraphicsDevice.h"
#include "..\core\EngineCore.h"
#include "..\Resource\ResourceManager.h"
#include "Texture.h"
#include "..\FileIO\Serializer.h"
#include "..\composition\composition.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "..\Resource\Resource.h"
#include "TextureFromFile.h"

namespace Improbability
{
unsigned UIComponent::m_declaration;
unsigned UIComponent::m_mesh;
int UIComponent::m_references = -1;

struct Vertex
{
  Vector3 m_position;
  Vector2 m_texture;
};

MakeMeta(UIComponent);

void UIComponent::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

void UIComponent::Initialize()
{
  if (m_references == -1)
  {
    // create screen space mesh
    GraphicsVertexTypes::VertexElement elements[2];

    elements[0].m_stream = elements[1].m_stream = 0;
    elements[0].m_type = GraphicsVertexTypes::VM_Vector3;
    elements[1].m_type = GraphicsVertexTypes::VM_Vector2;
    elements[0].m_usage = GraphicsVertexTypes::MU_Position;
    elements[1].m_usage = GraphicsVertexTypes::MU_TexCoord;

    m_declaration = GraphicsDevice::CreateVertexDeclaration(elements, 2);

    m_mesh = GraphicsDevice::CreateVertexBuffer(4, 0, false, m_declaration);

    Vertex *verts = reinterpret_cast<Vertex*>(GraphicsDevice::LockVertexBuffer(m_mesh));

    verts[0].m_position = Vector3(-0.5f,  0.5f, 0.0f);
    verts[1].m_position = Vector3( 0.5f,  0.5f, 0.0f);
    verts[2].m_position = Vector3(-0.5f, -0.5f, 0.0f);
    verts[3].m_position = Vector3( 0.5f, -0.5f, 0.0f);
    
    verts[0].m_texture = Vector2(0.0f, 0.0f);
    verts[1].m_texture = Vector2(1.0f, 0.0f);
    verts[2].m_texture = Vector2(0.0f, 1.0f);
    verts[3].m_texture = Vector2(1.0f, 1.0f);

    GraphicsDevice::UnlockVertexBuffer(m_mesh);

    m_references = 0;
  }
  ++m_references;

  if (m_texture_name)
    m_texture = g_engine_core->GetResourceManager()->GetResource<TextureFromFile>(m_texture_name)->GetTexture();
}

void UIComponent::Uninitialize()
{
  --m_references;

  if (m_references == 0)
  {
    GraphicsDevice::DeleteVertexBuffer(m_mesh);
    GraphicsDevice::DeleteVertexDeclaration(m_declaration);
  }
}

void UIComponent::Deserialize(Serializer &serializer)
{
  Read(serializer, m_texture_name, "Texture");

  serializer.EnterElement("Size");
  Read(serializer, m_size.m_x, "X");
  Read(serializer, m_size.m_y, "Y");
  serializer.LeaveElement();
}

void UIComponent::Draw(float width, float height)
{
  MatrixStack<Matrix4> &world = GraphicsDevice::GetWorldStack();
  MatrixStack<Matrix4> &projection = GraphicsDevice::GetProjectionStack();

  Vector4 position = GetParent()->GetPosition();

  //position = Matrix4::CreateTranslation(m_size.m_x / 2.0f, -m_size.m_y / 2.0f, 0.0f) * position;
  position.m_z = 0.0f;

  world.Push();
  world.LoadIdentity();
  world.LoadMatrix(Matrix4::CreateTranslation(2.0f * position.m_x / width, 2.0f * position.m_y / height, position.m_z));
  world.LoadMatrix(Matrix4::CreateScaling(2.0f * m_size.m_x / width, 2.0f * m_size.m_y / height, 0.0f));

  VertexShader *basic_vertex = VertexShader::GetShader("basic_texture");
  PixelShader *textured_pixel = PixelShader::GetShader("textured");

  basic_vertex->Set();
  textured_pixel->Set();

  m_texture->Set("s_sampler");

  //screen_space->SetConstant("u_pixel_size", Vector2(1.0f / width, 1.0f / height));

  GraphicsDevice::SetVertexBuffer(m_mesh);
  GraphicsDevice::DrawTriStripBuffer(2);

  m_texture->Unset("s_sampler");

  basic_vertex->Unset();
  textured_pixel->Unset();

  world.Pop();
}

void UIComponent::SetStartPositionPixel(Vector2 start_pos)
{
  GetParent()->SetPosition(Vector3::Point(start_pos));
}

void UIComponent::SetTexture(TextureFromFile *resource)
{
  m_texture = resource->GetTexture();
}

void UIComponent::SetTexture(Texture *tex)
{
  m_texture = tex;
}

}