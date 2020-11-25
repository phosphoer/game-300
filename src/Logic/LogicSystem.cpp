// David Evans
// 2011/09/17
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "LogicSystem.h"
#include "LogicComponent.h"
#include "../Core/EngineCore.h"
#include "../Composition/ObjectManager.h"

namespace Improbability
{

MakeMeta(LogicSystem);

void LogicSystem::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnObjectInitialized);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnObjectDeleted);
}

// Inherited methods
void LogicSystem::InitializeGlobal()
{
}

void LogicSystem::InitializeInstance()
{
  // Register listeners
  g_engine_core->AddEventListener(MessageBank::OnObjectInitialized, this);
  g_engine_core->AddEventListener(MessageBank::OnObjectDeleted, this);
}

void LogicSystem::UpdateInstance(float dt)
{
  auto it = m_components.Begin();
  auto end = m_components.End();
  for (; it != end; ++it)
  {
    LogicComponent *logic = it->second;
    logic->Update(dt);
  }
}

void LogicSystem::UninitializeInstance()
{
  g_engine_core->RemoveEventListener(MessageBank::OnObjectInitialized, this);
  g_engine_core->RemoveEventListener(MessageBank::OnObjectDeleted, this);
}

void LogicSystem::UninitializeGlobal()
{
}

void LogicSystem::Copy(System *dest, bool deep)
{
  if (deep)
  {
    LogicSystem *other = reinterpret_cast<LogicSystem*>(dest);
  }
}

void LogicSystem::Deserialize(Serializer &serializer)
{
}

// Messages
void LogicSystem::OnObjectInitialized(CompositionId id)
{
  ErrorIf(m_components.Find(id) != m_components.End(), "LogicSystem::OnObjectInitialized: Given a duplicate CompositionId");

  Composition *obj = g_engine_core->SystemCall("ObjectManager", "GetObject", id);
  LogicComponent *comp = obj->GetComponent<LogicComponent>();

  if (comp)
    m_components[id] = comp;
}

void LogicSystem::OnObjectDeleted(CompositionId id)
{
  m_components.Erase(id);
}

}