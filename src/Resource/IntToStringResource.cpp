// David Evans
// 2011/09/27
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "IntToStringResource.h"
#include "../FileIO/Serializer.h"

namespace Improbability
{

MakeMeta(IntToStringResource);

void IntToStringResource::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

void IntToStringResource::Load(char const *filepath)
{
  Serializer loader(filepath);

  for (loader.Begin(); loader.IsGood(); loader.Iterate())
  {
    String name;
    int id;
    Read(loader, name, "String");
    Read(loader, id, "Id");

    auto it = m_map.Find(id);
    ErrorIf(it != m_map.End(), "IntToStringResource::Load: Duplicate id of %i given!", id);
    m_map[id] = name;
  }
}

void IntToStringResource::Unload()
{
  m_map.Clear();
}

String IntToStringResource::GetString(int id)
{
  auto it = m_map.Find(id);
  if (it == m_map.End())
    return String();
  
  return it->second;
}

}