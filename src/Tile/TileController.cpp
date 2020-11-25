// David Evans
// 2011/07/23
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "TileController.h"
#include "../Composition/Composition.h"
#include "../Physics/DynamicComponent.h"
#include "../FileIO/Serializer.h"
#include "../Core/EngineCore.h"
#include "../Math/Vector4.h"

namespace Improbability
{

MakeMeta(TileController);

void TileController::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnActionPress);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnActionRelease);
}

TileController::TileController()
{
}

void TileController::Initialize()
{
  g_engine_core->AddEventListener(MessageBank::OnActionPress, this);
  g_engine_core->AddEventListener(MessageBank::OnActionRelease, this);
}

void TileController::Uninitialize()
{
  g_engine_core->RemoveEventListener(MessageBank::OnActionPress, this);
  g_engine_core->RemoveEventListener(MessageBank::OnActionRelease, this);
}

Component* TileController::Clone()
{
  TileController *clone = new TileController(*this);
  return clone;
}

void TileController::Deserialize(Serializer &serializer)
{
  Read(serializer, m_speed, "Speed");
  Read(serializer, m_jump, "Jump");
}

void TileController::OnActionPress(ActionEvent event)
{
  DynamicComponent *dyn = GetSibling<DynamicComponent>();
  Vector3 v = dyn->GetVelocity();

  if (event.m_name == "PlayerMoveLeft")
    dyn->AddImpulse(Vector3(-m_speed, 0.0f, 0.0f));
  else if (event.m_name == "PlayerMoveRight")
    dyn->AddImpulse(Vector3(m_speed, 0.0f, 0.0f));
  else if (event.m_name == "PlayerMoveUp")
    dyn->AddImpulse(Vector3(0.0f, m_speed, 0.0f));
  else if (event.m_name == "PlayerMoveDown")
    dyn->AddImpulse(Vector3(0.0f, -m_speed, 0.0f));  
  else if (event.m_name == "PlayerJump")
    dyn->AddImpulse(Vector3(0.0f, m_jump, 0.0f));
}

void TileController::OnActionRelease(ActionEvent event)
{
  DynamicComponent *dyn = GetSibling<DynamicComponent>();
  Vector3 v = dyn->GetVelocity();

  //if (event.m_name == "PlayerMoveLeft")
  //  dyn->AddImpulse(Vector3(m_speed, 0.0f, 0.0f));
  //else if (event.m_name == "PlayerMoveRight")
  //  dyn->AddImpulse(Vector3(-m_speed, 0.0f, 0.0f));
  //else if (event.m_name == "PlayerMoveUp")
  //  dyn->AddImpulse(Vector3(0.0f, -m_speed, 0.0f));  
  //else if (event.m_name == "PlayerMoveDown")
  //  dyn->AddImpulse(Vector3(0.0f, m_speed, 0.0f));
}


}