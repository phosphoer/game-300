// David Evans
// 2011/05/23
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "ObjectManager.h"
#include "Blueprint.h"
#include "../FileIO/Serializer.h"
#include "../Resource/ResourceManager.h"
#include "../Tile/TileSystem.h"
#include "../Tile/World.h"
#include "../Core/EngineCore.h"
#include "../Resource/IntToStringResource.h"
#include "../Sound/SoundSystem.h"

namespace Improbability
{

MakeMeta(ObjectManager);

void ObjectManager::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnNewGame);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnSpaceReady);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnWorldLoaded);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(CreateObject);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(CreateNamedObject);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(CreateObjectAtPosition);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(CreateNamedObjectAtPosition);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetObject);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetObjectsByType);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetObjectByName);
}

ObjectManager::ObjectManager()
{
}

void ObjectManager::InitializeGlobal()
{
}

void ObjectManager::InitializeInstance()
{
  g_engine_core->AddEventListener(MessageBank::OnNewGame, this);
  g_engine_core->AddEventListener(MessageBank::OnSpaceReady, this);
  g_engine_core->AddEventListener(MessageBank::OnWorldLoaded, this);
}

void ObjectManager::UpdateInstance(float dt)
{
  // Perform pending deletions
  auto end = m_trash.end();
  for (auto it = m_trash.begin(); it != end; ++it)
  {
    Composition *comp = *it;

    // 'Remove' it from the array
    m_objects[comp->GetId()] = 0;

    // Remove it from type maps
    if (comp == GetObjectsByType(comp->GetType()))
    {
      if (comp->m_next)
      {
        m_by_types[comp->GetType()] = comp->m_next;
        comp->m_next->m_prev = comp->m_prev;
      }
      else
        m_by_types.erase(comp->GetType());
    }

    // Remove it from name map
    m_by_names.erase(comp->GetName());

    // Uninitialize and delete
    comp->Uninitialize();
    delete comp;
  }
  m_trash.clear();
}

void ObjectManager::UninitializeInstance()
{
  g_engine_core->RemoveEventListener(MessageBank::OnNewGame, this);
  g_engine_core->RemoveEventListener(MessageBank::OnSpaceReady, this);
  g_engine_core->RemoveEventListener(MessageBank::OnWorldLoaded, this);

  DeleteObjects(false, false);
}

void ObjectManager::UninitializeGlobal()
{
}

void ObjectManager::Copy(System *dest, bool deep)
{
  ObjectManager *mgr = reinterpret_cast<ObjectManager*>(dest);

  // Clear the other system's objects
  mgr->DeleteObjects(true);

  // Resize the dest object array to our size
  int num_objects = m_objects.size();
  mgr->m_objects.resize(num_objects);

  // If it's a deep copy, copy all game objects to the next system
  if (deep)
  {
    for (int i = 0; i < num_objects; ++i)
    {
      if (m_objects[i] && m_objects[i]->IsSpaceable())
      {
        Composition *comp = m_objects[i];

        // Add object to manager
        mgr->m_objects[i] = comp->Clone();
        Composition *clone = mgr->m_objects[i];
        clone->m_id = i;

        // Add the clone to type map
        Composition *head = mgr->GetObjectsByType(clone->GetType());
        clone->m_next = head;
        if (head) head->m_prev = clone;
        mgr->m_by_types[clone->GetType()] = clone;

        // Add the object to name map
        mgr->m_by_names[clone->GetName()] = clone;
        
        // Initialize the clone
        clone->Initialize();

        // Inform the rest of the engine that an object was created
        g_engine_core->DispatchEvent(MessageBank::OnObjectInitialized, clone->m_id);
      }
      else
        mgr->m_objects[i] = 0;
    }    
  }
}

void ObjectManager::Deserialize(Serializer &serializer)
{
  Read(serializer, m_object_layer_name, "ObjectLayerName");
  Read(serializer, m_blueprint_map_name, "BlueprintMapName");

  String new_object;

  if (serializer.EnterElement("ObjectsToCreate"))
  {
    for (serializer.Begin(); serializer.IsGood(); serializer.Iterate())
    {
      Read(serializer, new_object);
      m_deserialized_objects.push_back(new_object);
    }
    serializer.LeaveElement();
  }
  serializer.LeaveElement();
}

void ObjectManager::OnNewGame()
{
  g_coin_count = 0;
  g_curr_coin_count = 0;
  g_coins_needed = 0;
  g_rotate_blocks = false;
  g_rotate_ability = false;
  g_anchor_blocks = false;
  g_anchor_ability = false;
  g_the_end = false;
  g_nega_binding = false;
  g_player_facing = -1;
  g_curr_level = 1;

  //Progress Variables
  g_coin_get = false;
  g_coins_complete = false;
  g_moved = false;
  g_jumped = false;
  g_first_rotate = false;
  g_first_level = false;
  g_second_level = false;
  g_final_level = false;
  g_unlocked_rblocks = false;
  g_unlocked_rability = false;
  g_unlocked_ablocks = false;
  g_unlocked_aability = false;
  g_first_anchor = false;
  g_anchor_cleared = false;
  g_anchor_dropped = false;
  g_anchor_set = false;
  g_anchor_tried = false;
  g_free_rotated = false;
  g_warped_back = false;
  g_walked_behind = false;
  g_isLoaded = false;

  ResourceManager* res = g_engine_core->GetResourceManager();
  ResourceManager::ResourceMap* temp = res->GetResourceMap();
  for(auto it = temp->Begin(); it != temp->End(); ++it)
  {
    if((it)->second->GetClassName() == "Tilemap")
      reinterpret_cast<Tilemap*>((it)->second)->ResetTilemap();
  }
  g_engine_core->DispatchEvent("OnNewGameStart");
  g_engine_core->DispatchEvent("PushAudioEvent", "Stop_audio");
  SoundSystem::ForceUpdate();
}

void ObjectManager::OnSpaceReady()
{
  for (unsigned i = 0; i < m_deserialized_objects.size(); ++i)
  {
    CreateObject(m_deserialized_objects[i].GetCString());
  }
}

void ObjectManager::OnWorldLoaded(World *world)
{
  World::ObjectArray *objects = world->GetObjectArray();
  for (auto it = objects->begin(); it != objects->end(); ++it)
  {
    Composition *obj = CreateNamedObjectAtPosition(it->m_blueprint, it->m_position, it->m_name);
  }
}

Composition* ObjectManager::CreateObject(char const *filename)
{
  // Get the blueprint and clone it
  Blueprint *blueprint = g_engine_core->GetResourceManager()->GetResource<Blueprint>(filename);
  Composition *comp = blueprint->GetComposition()->Clone();

  // Add the object to the object manager
  comp->m_id = m_objects.size();
  m_objects.push_back(comp);

  // Store it by type
  Composition *head = GetObjectsByType(comp->GetType());
  comp->m_next = head;
  if (head) head->m_prev = comp;
  m_by_types[comp->GetType()] = comp;

  // Initialize the object
  comp->Initialize();

  // Store it by name
  m_by_names[comp->GetName()] = comp;

  // Inform the rest of the engine that an object was created
  g_engine_core->DispatchEvent(MessageBank::OnObjectInitialized, comp->m_id);

  return comp;
}

Composition* ObjectManager::CreateNamedObject(char const *filename, char const *name)
{
  // Get the blueprint and clone it
  Blueprint *blueprint = g_engine_core->GetResourceManager()->GetResource<Blueprint>(filename);
  Composition *comp = blueprint->GetComposition()->Clone();

  // Add the object to the object manager
  comp->m_id = m_objects.size();
  m_objects.push_back(comp);

  // Store it by type
  Composition *head = GetObjectsByType(comp->GetType());
  comp->m_next = head;
  if (head) head->m_prev = comp;
  m_by_types[comp->GetType()] = comp;

  // Initialize the object
  comp->Initialize();
  String strname = name;
  if (strname != "") comp->m_instance_name = name;

  // Store it by name
  m_by_names[comp->GetName()] = comp;

  // Inform the rest of the engine that an object was created
  g_engine_core->DispatchEvent(MessageBank::OnObjectInitialized, comp->m_id);

  return comp;
}

Composition* ObjectManager::CreateObjectAtPosition(char const *filename, Vector3 pos)
{
  Composition *comp = CreateObject(filename);
  comp->SetPosition(pos);
  return comp;
}

Composition* ObjectManager::CreateNamedObjectAtPosition(char const *filename, Vector3 pos, char const *name)
{
  Composition *comp = CreateNamedObject(filename, name);
  comp->SetPosition(pos);
  return comp;
}

void ObjectManager::DeleteObject(CompositionId id)
{
  m_trash.push_back(GetObject(id));

  // Inform everyone else that the object is being deleted
  g_engine_core->DispatchEvent(MessageBank::OnObjectDeleted, id);
}

Composition* ObjectManager::GetObject(CompositionId id)
{
  if (id < 0 || id >= m_objects.size())
    return 0;
  return m_objects[id];
}

Composition* ObjectManager::GetObjectsByType(String name)
{
  // Find the map of objects with this name
  auto it = m_by_types.find(name);
  if (it == m_by_types.end())
    return 0;

  return it->second;
}

Composition* ObjectManager::GetObjectByName(String name)
{
  auto it = m_by_names.find(name);
  if (it == m_by_names.end())
    return 0;

  return it->second;
}

void ObjectManager::DeleteObjects(bool check_spaceable, bool send_message)
{
  auto end = m_objects.end();
  for (auto it = m_objects.begin(); it != end; ++it)
  {
    Composition *comp = *it;
    if (comp && (!check_spaceable || comp->IsSpaceable()))
    {
      // Inform everyone else that the object is being deleted
      if (send_message)
        g_engine_core->DispatchEvent(MessageBank::OnObjectDeleted, comp->GetId());

      // Delete the object
      m_objects[comp->GetId()] = 0;
      comp->Uninitialize();
      delete comp;
    }
  }
}

}