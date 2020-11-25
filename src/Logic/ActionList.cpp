// David Evans
// 2011/09/20
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "ActionList.h"
#include "../FileIO/Serializer.h"

namespace Improbability
{

MakeMeta(ActionList);

void ActionList::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

void ActionList::Load(char const *filepath)
{
  // Open the file
  Serializer serializer(filepath);

  // Iterate over actions and load them
  for (serializer.Begin(); serializer.IsGood(); serializer.Iterate())
  {
    // Get name of action and create the action
    String name = serializer.GetName();
    Action *action = Meta::Create<Action>(name);

    // Set and serializer attributes about the action
    action->DeserializeBase(serializer);
    Read(serializer, *action);

    // Add the action and move to the next one
    m_actions.push_back(action);
  }
}

void ActionList::Unload()
{
  auto it = m_actions.begin();
  auto end = m_actions.end();
  for (; it != end; ++it)
    delete *it;
  
  m_actions.clear();
}

}