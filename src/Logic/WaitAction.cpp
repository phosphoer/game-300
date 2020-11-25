// David Evans
// 2011/10/28
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "WaitAction.h"
#include "../Composition/Composition.h"
#include "../FileIO/Serializer.h"

namespace Improbability
{

MakeMeta(WaitAction);

void WaitAction::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

ActionReturn::Type WaitAction::Update(float dt)
{
  return ActionReturn::Block;
}

void WaitAction::Deserialize(Serializer &serializer)
{
}

}