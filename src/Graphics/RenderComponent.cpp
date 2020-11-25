// Nicholas Baldwin
// 2011/09/27
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "RenderComponent.h"
#include "..\FileIO\Serializer.h"
#include "..\core\EngineCore.h"
#include "..\Resource\ResourceManager.h"
#include "Shader.h"
#include "..\math\MatrixStack.h"
#include "GraphicsDevice.h"
#include "..\composition\composition.h"
#include "Texture.h"
#include "Mesh.h"
#include "Renderable.h"

namespace Improbability
{

MakeMeta(RenderComponent);

void RenderComponent::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

void RenderComponent::Initialize()
{  
  m_renderable = g_engine_core->GetResourceManager()->GetResource<Renderable>(m_renderable_name);
}

void RenderComponent::Uninitialize()
{

}

void RenderComponent::Deserialize(Serializer &serializer)
{
  Read(serializer, m_renderable_name, "Renderable");
}

void RenderComponent::Draw()
{
  MatrixStack<Matrix4> &world = GraphicsDevice::GetWorldStack();

  world.Push();
  world.LoadMatrix(Matrix4::CreateTranslation(GetParent()->GetPosition()));

  m_renderable->SetShader();
  m_renderable->SetConstants();
  m_renderable->SetMesh();
  m_renderable->Draw();

  m_renderable->Clear();

  world.Pop();
}

float RenderComponent::GetDepth()
{
  MatrixStack<Matrix4> &world = GraphicsDevice::GetWorldStack();

  world.Push();
  world.LoadMatrix(Matrix4::CreateTranslation(GetParent()->GetPosition()));

  float depth =  m_renderable->GetDepth();
    
  world.Pop();

  return depth;
}

}