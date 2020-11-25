// David Evans
// 2011/09/27
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "PushSpaceAction.h"
#include "LogicComponent.h"
#include "../FileIO/Serializer.h"
#include "../Core/EngineCore.h"

namespace Improbability
{

MakeMeta(PushSpaceAction);

void PushSpaceAction::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

ActionReturn::Type PushSpaceAction::Update(float dt)
{
  g_engine_core->PushSpace(m_space_name, m_is_deep);
  return ActionReturn::Continue;
}

void PushSpaceAction::Deserialize(Serializer &serializer)
{
  Read(serializer, m_space_name, "Name");
  Read(serializer, m_is_deep, "IsDeep");
}

}