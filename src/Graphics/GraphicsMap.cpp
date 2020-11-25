// Nicholas Baldwin
// 2011/10/27
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "GraphicsMap.h"
#include "..\FileIO\Serializer.h"

namespace Improbability
{

MakeMeta(GraphicsMap);

void GraphicsMap::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

void GraphicsMap::Load(const char*filename)
{
  Serializer loader(filename);

  int key;

  String renderable;

  for (loader.Begin(); loader.IsGood(); loader.Iterate())
  {
    Read(loader, key, "Key");
    Read(loader, renderable, "Renderable");
    m_renderable_names[key] = renderable;
  }
}

}