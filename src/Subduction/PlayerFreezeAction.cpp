// Aaron Biddlecom
// 2011/11/29
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "PlayerFreezeAction.h"
#include "../Core/EngineCore.h"
#include "../Composition/Composition.h"
#include "../FileIO/Serializer.h"

namespace Improbability
{

  MakeMeta(PlayerFreezeAction);

  void PlayerFreezeAction::BindMeta(MetaComposition *comp)
  {
    comp->MetaAddComponent(MetaNewable);
    comp->MetaAddComponent(MetaCallable);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAxisChangeComplete);
  }

  PlayerFreezeAction::PlayerFreezeAction():
    m_action_complete(false)
  {}

  void PlayerFreezeAction::Initialize()
  {
    g_engine_core->AddEventListener(MessageBank::OnAxisChangeComplete, this);
  }

  void PlayerFreezeAction::Uninitialize()
  {
    g_engine_core->RemoveEventListener(MessageBank::OnAxisChangeComplete, this);
  }

  void PlayerFreezeAction::OnAxisChangeComplete(int axis)
  {
    m_action_complete = true;
  }

  ActionReturn::Type PlayerFreezeAction::Update(float dt)
  {
    if(m_action_complete)
      return ActionReturn::Remove;
    else
      return ActionReturn::Block;
  }

  void PlayerFreezeAction::Deserialize(Serializer &serializer)
  {
  }
}