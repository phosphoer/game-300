// David Evans
// 2011/07/21
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "TileSystem.h" 
#include "World.h"
#include "Tilemap.h"
#include "../Core/EngineCore.h"
#include "../Resource/ResourceManager.h"
#include "../FileIO/Serializer.h"

namespace Improbability
{

MakeMeta(TileSystem);

void TileSystem::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnSpaceReady);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(SetActiveWorld);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(SetWorldAxis);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetTilemaps);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetLevelNumber);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetWorldAxis);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetFocusBegin);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetFocusEnd);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetFocusBeginWithArea);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetFocusEndWithArea);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetFocusPosition);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetBorderSize);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetAnchorAxis);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetAnchorBegin);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetAnchorEnd);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetAnchorRange);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetObscured);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetMemoryRange);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(SetAnchorAxis);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(SetAnchorBegin);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(SetAnchorEnd);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(SetAnchorRange);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(SetObscured);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(SetMemoryRange);
}

TileSystem::TileSystem():
  m_world(0),
  m_axis(0),
  m_border_size(10),
  m_anchor_axis(-1),
  m_anchor_begin(-1),
  m_anchor_end(-1),
  m_anchor_range(0),
  m_obscured(false)
{
}

void TileSystem::InitializeGlobal()
{
}

void TileSystem::InitializeInstance()
{
  g_engine_core->AddEventListener(MessageBank::OnSpaceReady, this);
  g_engine_core->AddEventListener(MessageBank::SetActiveWorld, this);
}

void TileSystem::UpdateInstance(float dt)
{
  // Get the object the tile system is focusing on
  // And calculate the tiles that should be drawn
  Composition *focus_obj = g_engine_core->SystemCall("ObjectManager", "GetObjectByName", m_world->GetFocusObject());
  if (focus_obj)
  {
    // Get the object's position
    Vector3 focus_pos = focus_obj->GetPosition().ToVector3();

    // Depth axis begins one layer in front of the focus object
    // and ends one behind
    m_focus_begin.m_a[m_axis] = static_cast<int>(focus_pos.m_a[m_axis]) - 1;
    m_focus_end.m_a[m_axis] = static_cast<int>(focus_pos.m_a[m_axis]) + 2;

    // Set "width" and "height" of focus area based on axis
    if (m_axis == WorldAxis::Right)
    {
      // Width
      m_focus_begin.m_z = static_cast<int>(focus_pos.m_z) - m_border_size;
      m_focus_end.m_z = static_cast<int>(focus_pos.m_z) + m_border_size;

      // Height
      m_focus_begin.m_y = static_cast<int>(focus_pos.m_y) - m_border_size;
      m_focus_end.m_y = static_cast<int>(focus_pos.m_y) + m_border_size;
    }
    else if (m_axis == WorldAxis::Top)
    {
      // Width
      m_focus_begin.m_x = static_cast<int>(focus_pos.m_x) - m_border_size;
      m_focus_end.m_x = static_cast<int>(focus_pos.m_x) + m_border_size;

      // Height
      m_focus_begin.m_z = static_cast<int>(focus_pos.m_z) - m_border_size;
      m_focus_end.m_z = static_cast<int>(focus_pos.m_z) + m_border_size;
    }
    else if (m_axis == WorldAxis::Front)
    {
      // Width
      m_focus_begin.m_x = static_cast<int>(focus_pos.m_x) - m_border_size;
      m_focus_end.m_x = static_cast<int>(focus_pos.m_x) + m_border_size;

      // Height
      m_focus_begin.m_y = static_cast<int>(focus_pos.m_y) - m_border_size;
      m_focus_end.m_y = static_cast<int>(focus_pos.m_y) + m_border_size;
    }
  }
}

void TileSystem::UninitializeInstance()
{
  g_engine_core->RemoveEventListener(MessageBank::OnSpaceReady, this);
  g_engine_core->RemoveEventListener(MessageBank::SetActiveWorld, this);
}

void TileSystem::UninitializeGlobal()
{
}

void TileSystem::Copy(System *dest, bool deep)
{
  if (deep)
  {
    TileSystem *other = reinterpret_cast<TileSystem*>(dest);
    other->m_tilemaps = m_tilemaps;
  }
}

void TileSystem::Deserialize(Serializer &serializer)
{
  Read(serializer, m_initial_world, "World");
  Read(serializer, m_axis, "StartAxis");
  Read(serializer, m_level_number, "LevelNumber");
}

void TileSystem::OnSpaceReady()
{
  SetActiveWorld(m_initial_world);
  SetWorldAxis(m_axis);
  g_engine_core->DispatchEvent(MessageBank::OnAxisInitalize, m_axis);
  g_engine_core->DispatchEvent("OnRotationComplete", m_axis, m_axis);
  g_engine_core->DispatchEvent("OnAxisChangeComplete", m_axis);
}

void TileSystem::SetActiveWorld(char const *filename)
{
  if (!filename) return;
  
  ResourceManager *res = g_engine_core->GetResourceManager();
  m_world = res->GetResource<World>(filename);
  World::StringsMap &strings_map = m_world->GetMap();

  // Dispatch an event saying the current world is about to be deleted
  g_engine_core->DispatchEvent(MessageBank::OnWorldUnloading);

  // Delete old tilemap arrays
  m_tilemaps.Clear();

  // Add new tilemaps arrays
  auto end = strings_map.End();
  for (auto it = strings_map.Begin(); it != end; ++it)
  {
    World::StringArray *string_array = it->second;

    for (unsigned int i = 0; i < string_array->size(); ++i)
    {
      String tilemap_file = (*string_array)[i];
      String tilemap_type = it->first;
      Tilemap *tilemap = res->GetResource<Tilemap>(tilemap_file);

      auto it = m_tilemaps.Find(tilemap_type);
      if (it == m_tilemaps.End())
      {
        m_tilemaps[tilemap_type] = tilemap;
      }
      else
      {
        Tilemap *head = it->second;
        tilemap->m_next = head;
        m_tilemaps[tilemap_type] = tilemap;
      }
      if(tilemap_type == "Special")
      {
        tilemap->BuildObjectMap(0);
      }
    }
  }

  // Dispatch a message saying a new world was loaded
  g_engine_core->DispatchEvent(MessageBank::OnWorldLoaded, m_world);
}

void TileSystem::SetWorldAxis(int axis)
{
  ErrorIf(axis < 0 || axis > 2, "TileSystem::SetAxis: Invalid axis of %i", axis);
  g_engine_core->DispatchEvent(MessageBank::OnAxisChange, m_axis, axis);
  m_axis = axis;
}

Tilemap* TileSystem::GetTilemaps(char const *type)
{
  auto it = m_tilemaps.Find(type);
  ErrorIf(it == m_tilemaps.End(), "TileSystem::GetTilemaps: No group of tilemaps with type: %s", type);
  return it->second;
}
  
Improbability::TilePos3 TileSystem::GetFocusBeginWithArea( TilePos3 begin, int axis, TilePos3 focus_pos )
{
  TilePos3 focus_begin = focus_pos;
  int x_axis = axis, y_axis = (axis + 1) % 3, z_axis = (axis + 2) % 3;
  focus_begin.m_a[x_axis] -= begin.m_x;
  focus_begin.m_a[y_axis] -= begin.m_y;
  focus_begin.m_a[z_axis] -= begin.m_z;

  return focus_begin;
}

Improbability::TilePos3 TileSystem::GetFocusEndWithArea( TilePos3 end, int axis, TilePos3 focus_pos  )
{
  TilePos3 focus_end = focus_pos;
  int x_axis = axis, y_axis = (axis + 1) % 3, z_axis = (axis + 2) % 3;
  focus_end.m_a[x_axis] += end.m_x;
  focus_end.m_a[y_axis] += end.m_y;
  focus_end.m_a[z_axis] += end.m_z;

  return focus_end;
}

Improbability::TilePos3 TileSystem::GetFocusPosition()
{
  Composition *focus_object = g_engine_core->SystemCall("ObjectManager", "GetObjectByName", m_world->GetFocusObject());

  ErrorIf(!focus_object, "There was no focus object!");

  Vector3 focus_vector = focus_object->GetPosition().ToVector3();

  return TilePos3(focus_vector.m_x, focus_vector.m_y, focus_vector.m_z);
}

void TileSystem::SetAnchorAxis( int axis )
{
  m_anchor_axis = axis;
}

void TileSystem::SetAnchorBegin( int begin )
{
  m_anchor_begin = begin;
}

void TileSystem::SetAnchorEnd( int end )
{
  m_anchor_end = end;
}

void TileSystem::SetAnchorRange( int range )
{
  m_anchor_range = range;
}

void TileSystem::SetObscured( int obscured )
{
  m_obscured = obscured;
}

void TileSystem::SetMemoryRange( int range )
{
  m_memory_limit = range;
}

}