// Nicholas Baldwin
// 2011/10/25
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "ModelViewerSystem.h"
#include "..\core\EngineCore.h"
#include "..\FileIO\Serializer.h"

namespace Improbability
{
  
MakeMeta(ModelViewerSystem);

void ModelViewerSystem::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnObjectInitialized);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnObjectDeleted);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnSpaceReady);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnKeyPress);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnMousePress);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnMouseRelease);
}

void ModelViewerSystem::InitializeGlobal()
{

}

void ModelViewerSystem::InitializeInstance()
{
  g_engine_core->AddEventListener(MessageBank::OnObjectInitialized, this);
  g_engine_core->AddEventListener(MessageBank::OnObjectDeleted, this);
  g_engine_core->AddEventListener(MessageBank::OnSpaceReady, this);
  g_engine_core->AddEventListener(MessageBank::OnKeyPress, this);
  g_engine_core->AddEventListener(MessageBank::OnMousePress, this);
  g_engine_core->AddEventListener(MessageBank::OnMouseRelease, this);
}

void ModelViewerSystem::UninitializeGlobal()
{

}

void ModelViewerSystem::UninitializeInstance()
{
  g_engine_core->RemoveEventListener(MessageBank::OnObjectInitialized, this);
  g_engine_core->RemoveEventListener(MessageBank::OnObjectDeleted, this);
  g_engine_core->RemoveEventListener(MessageBank::OnSpaceReady, this);
  g_engine_core->RemoveEventListener(MessageBank::OnKeyPress, this);
  g_engine_core->RemoveEventListener(MessageBank::OnMousePress, this);
  g_engine_core->RemoveEventListener(MessageBank::OnMouseRelease, this);
}

void ModelViewerSystem::UpdateInstance(float dt)
{

}

void ModelViewerSystem::Copy(System *dest, bool deep)
{

}

void ModelViewerSystem::Deserialize(Serializer &serializer)
{
  Read(serializer, m_model_name, "Model");
}

void ModelViewerSystem::OnObjectInitialized(CompositionId id)
{

}

void ModelViewerSystem::OnObjectDeleted(CompositionId id)
{

}

void ModelViewerSystem::OnSpaceReady()
{
  g_engine_core->SystemCall("ObjectManager", "CreateObjectAtPosition", "editor-camera.bp", Vector3(0.0f, 0.0f, 8.0f));
  
  g_engine_core->SystemCall("ObjectManager", "CreateObject", m_model_name);
}

void ModelViewerSystem::OnKeyPress(KeyboardEvent)
{

}

void ModelViewerSystem::OnMousePress(MouseEvent)
{

}

void ModelViewerSystem::OnMouseRelease(MouseEvent)
{

}

}