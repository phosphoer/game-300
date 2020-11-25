// Nicholas Baldwin
// 2011/10/27
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "..\Resource\Resource.h"
#include "..\Tile\Tilemap.h"

namespace Improbability
{

class GraphicsMap : public Resource
{
public:
  static void BindMeta(MetaComposition *comp);

  void Load(const char*filename);

  String GetRenderable(Tile key) {return m_renderable_names[key];}

  void Unload() {}

private:
  HashMap<int, String> m_renderable_names;
};

}