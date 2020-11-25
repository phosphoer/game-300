// David Evans
// 2011/09/27
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "PresentationSystem.h"
#include "../Core/EngineCore.h"
#include "../Logic/LogicComponent.h"
#include "../Logic/AnimatePropertyAction.h"
#include "../Resource/ResourceManager.h"
#include "../Composition/ObjectManager.h"
#include "../Input/InputEvent.h"
#include "../Graphics/Camera.h"
#include "../Graphics/UIComponent.h"
#include "../Graphics/Texture.h"
#include "../Graphics/GraphicsDevice.h"
#include "../FileIO/Serializer.h"

namespace Improbability
{

MakeMeta(PresentationSystem);

void PresentationSystem::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnObjectInitialized);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnObjectDeleted);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnSpaceReady);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnKeyPress);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnMousePress);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnMouseRelease);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnDeviceReset);
}

// Inherited methods
void PresentationSystem::InitializeGlobal()
{
}

void PresentationSystem::InitializeInstance()
{
  // Register listeners
  g_engine_core->AddEventListener(MessageBank::OnObjectInitialized, this);
  g_engine_core->AddEventListener(MessageBank::OnObjectDeleted, this);
  g_engine_core->AddEventListener(MessageBank::OnSpaceReady, this);
  g_engine_core->AddEventListener(MessageBank::OnKeyPress, this);
  g_engine_core->AddEventListener(MessageBank::OnMousePress, this);
  g_engine_core->AddEventListener(MessageBank::OnMouseRelease, this);
  g_engine_core->AddEventListener(MessageBank::OnDeviceReset, this);
}

void PresentationSystem::UpdateInstance(float dt)
{
}

void PresentationSystem::UninitializeInstance()
{
  g_engine_core->RemoveEventListener(MessageBank::OnObjectInitialized, this);
  g_engine_core->RemoveEventListener(MessageBank::OnObjectDeleted, this);
  g_engine_core->RemoveEventListener(MessageBank::OnSpaceReady, this);
  g_engine_core->RemoveEventListener(MessageBank::OnKeyPress, this);
  g_engine_core->RemoveEventListener(MessageBank::OnMousePress, this);
  g_engine_core->RemoveEventListener(MessageBank::OnMouseRelease, this);
  g_engine_core->RemoveEventListener(MessageBank::OnDeviceReset, this);
}

void PresentationSystem::UninitializeGlobal()
{
}

void PresentationSystem::Copy(System *dest, bool deep)
{
}

void PresentationSystem::Deserialize(Serializer &serializer)
{
  for (serializer.Begin(); serializer.IsGood(); serializer.Iterate())
  {
    if (serializer.GetName() == "Slide")
    {
      String file;
      Read(serializer, file);
      m_slidenames.push_back(file);
    }
  }
  serializer.LeaveElement();
}

// Messages
void PresentationSystem::OnObjectInitialized(CompositionId id)
{

}

void PresentationSystem::OnObjectDeleted(CompositionId id)
{
}

void PresentationSystem::OnKeyPress(KeyboardEvent event)
{
}

void PresentationSystem::OnSpaceReady()
{  
  for (unsigned i = 0; i < m_slidenames.size(); ++i)
  {
    Composition *slide = g_engine_core->SystemCall("ObjectManager", "CreateObject", "slide.bp");
    UIComponent *sprite = slide->GetComponent<UIComponent>();
    
    sprite->SetSize(Vector2(GraphicsDevice::GetDeviceWidth(), GraphicsDevice::GetDeviceHeight()));
    slide->SetPosition(Vector3(i * sprite->GetSize().m_x, 0.0f, 0.0f));

    sprite->SetTexture(g_engine_core->GetResourceManager()->GetResource<TextureFromFile>(m_slidenames[i]));

    m_slides.push_back(slide->GetId());
    
    LogicComponent *logic = slide->GetComponent<LogicComponent>();
    std::list<Action*> const &actions = logic->GetActionList();
    auto it = actions.begin();
    auto end = actions.end();
    for (; it != end; ++it)
    {
      Action *action = *it;
      if (action->GetType() == "AnimatePropertyAction")
      {
        AnimatePropertyAction *anim = reinterpret_cast<AnimatePropertyAction*>(action);
        if (anim->GetEndFloat() < 0)
          anim->SetEnd(GraphicsDevice::GetDeviceWidth() * -1.0f);
        else
          anim->SetEnd(GraphicsDevice::GetDeviceWidth() * 1.0f);
      }
    }
  }
}

void PresentationSystem::OnMousePress(MouseEvent event)
{
}

void PresentationSystem::OnMouseRelease(MouseEvent event)
{
}

void PresentationSystem::ResizeSlides()
{
  for (unsigned i = 0; i < m_slides.size(); ++i)
  {
    Composition *slide = g_engine_core->SystemCall("ObjectManager", "GetObject", m_slides[i]);

    UIComponent *sprite = slide->GetComponent<UIComponent>();
    LogicComponent *logic = slide->GetComponent<LogicComponent>();
    sprite->SetSize(Vector2(GraphicsDevice::GetDeviceWidth(), GraphicsDevice::GetDeviceHeight()));
    slide->SetPosition(Vector3(i * GraphicsDevice::GetDeviceWidth() * 1.0f, 0.0f, 0.0f));

    std::list<Action*> const &actions = logic->GetActionList();
    auto it = actions.begin();
    auto end = actions.end();
    for (; it != end; ++it)
    {
      Action *action = *it;
      if (action->GetType() == "AnimatePropertyAction")
      {
        AnimatePropertyAction *anim = reinterpret_cast<AnimatePropertyAction*>(action);
        if (anim->GetEndFloat() < 0)
          anim->SetEnd(GraphicsDevice::GetDeviceWidth() * -1.0f);
        else
          anim->SetEnd(GraphicsDevice::GetDeviceWidth() * 1.0f);
      }
    }
  }
}

void PresentationSystem::OnDeviceReset()
{
  ResizeSlides();
}

}