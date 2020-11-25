// David Evans
// 2011/08/31
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "World.h"
#include "Tilemap.h"
#include "../FileIO/Serializer.h"

namespace Improbability
{

MakeMeta(World);

void World::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

void World::Load(char const *filepath)
{
  Serializer loader(filepath);

  // Read meta information about the world
  Read(loader, m_graphics_map, "GraphicsMap");
  Read(loader, m_focus_object, "FocusObject");

  for (loader.Begin(); loader.IsGood(); loader.Iterate())
  {
    String name = loader.GetName();

    // Read an object definition
    if (name == "Object")
    {
      Object object;
      Read(loader, object.m_blueprint, "Blueprint");
      Read(loader, object.m_name, "Name");
      Read(loader, object.m_position, "Position");
      if (Read(loader, object.m_position, "TilePosition"))
        object.m_position += Vector3(0.5f, 0.5f, 0.5f);
      m_objects.push_back(object);
    }
    // Read a tile layer definition
    else if (name == "Layer")
    {
      String type, file;
      Read(loader, type, "Type");
      Read(loader, file, "File");

      auto it = m_maps.Find(type);
      if (it == m_maps.End())
      {
        StringArray *tilemaps = new StringArray();
        tilemaps->push_back(file);
        m_maps[type] = tilemaps;
      }
      else
      {
        StringArray *tilemaps = it->second;
        tilemaps->push_back(file);
      }
    }
  }
}

void World::Unload()
{
  auto end = m_maps.End();
  for (auto it = m_maps.Begin(); it != end; ++it)
  {
    delete it->second;
  }
}

World::StringArray* World::GetTilemapNames(char const *type)
{
  auto it = m_maps.Find(type);
  if (it == m_maps.End())
    return 0;

  return it->second;
}

}