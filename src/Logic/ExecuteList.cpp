// David Evans
// 2011/09/20
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "ExecuteList.h"
#include "LogicComponent.h"
#include "../Composition/Composition.h"
#include "../FileIO/Serializer.h"

namespace Improbability
{

MakeMeta(ExecuteList);

void ExecuteList::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

ActionReturn::Type ExecuteList::Update(float dt)
{
  Composition *obj = GetParent();
  LogicComponent *logic = obj->GetComponent<LogicComponent>();

  logic->AddActionList(m_action_name);

  return ActionReturn::Remove;
}

void ExecuteList::Deserialize(Serializer &serializer)
{
  Read(serializer, m_action_name, "Name");
}

}