// David Evans
// 2011/09/08
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "InputActions.h"
#include "../FileIO/Serializer.h"

namespace Improbability
{

MakeMeta(InputActions);

void InputActions::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

void InputActions::Load(char const *filepath)
{
  Serializer loader(filepath);

  for (loader.Begin(); loader.IsGood(); loader.Iterate())
  {
    String node_name = loader.GetName();
    String key_action;
    Read(loader, key_action);
    unsigned *value = Meta::GetConstantValue<unsigned>(node_name);
    m_actions[*value] = key_action;
  }
}

void InputActions::Unload()
{
}

String InputActions::GetAction(unsigned key)
{
  ErrorIf(key < 0 || key >= 256, "InputActions::GetAction(): Invalid key of value %i", key);
  return m_actions[key];
}

}