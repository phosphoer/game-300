// David Evans
// 2011/09/29
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "PopSpaceAction.h"
#include "LogicComponent.h"
#include "../FileIO/Serializer.h"
#include "../Core/EngineCore.h"

namespace Improbability
{

MakeMeta(PopSpaceAction);

void PopSpaceAction::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

ActionReturn::Type PopSpaceAction::Update(float dt)
{
  g_engine_core->PopSpace(m_is_deep);
  return ActionReturn::Continue;
}

void PopSpaceAction::Deserialize(Serializer &serializer)
{
  Read(serializer, m_is_deep, "IsDeep");
}

}