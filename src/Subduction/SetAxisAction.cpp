// David Evans
// 2011/09/18
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "SetAxisAction.h"
#include "../Core/EngineCore.h"
#include "../FileIO/Serializer.h"

namespace Improbability
{

MakeMeta(SetAxisAction);

void SetAxisAction::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

SetAxisAction::SetAxisAction():
  m_axis(0)
{
}

ActionReturn::Type SetAxisAction::Update(float dt)
{
  g_engine_core->SystemCall("TileSystem", "SetWorldAxis", m_axis);
  return ActionReturn::Continue;
}

void SetAxisAction::Deserialize(Serializer &serializer)
{
  Read(serializer, m_axis, "Axis");
}

}