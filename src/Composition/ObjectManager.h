// David Evans
// 2011/05/23
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Core/System.h"
#include "../Composition/Composition.h"

#undef GetObject

namespace Improbability
{

class MetaComposition;
class World;

// Object manager system handles
// game objects
class ObjectManager : public System
{
public:
  typedef std::vector<Composition*> ObjectArray;
  typedef std::map<String, Composition*> StringObjectMap;

public:
  static void BindMeta(MetaComposition*);

  // Constructor
  ObjectManager();

  // Inherited methods

  // Initialize method that is called once at startup
  void InitializeGlobal();

  // Initialize method that is called each time a space is created
  void InitializeInstance();

  // Updates the system every frame
  // dt - Elapsed seconds since last call
  void UpdateInstance(float dt);

  // Uninitialize method that is called each time a space is destroyed
  void UninitializeInstance();

  // Uninitialize method that is called once at the end of the program
  void UninitializeGlobal();

  // Copy system data into another system
  // dest - System to copy data into
  // deep - Whether or not to perform a "deep" copy
  void Copy(System *dest, bool deep);

  // Read system from file
  // serializer - Serializer to to use
  void Deserialize(Serializer &serializer);

  // Messages

  // Load a game object from a message
  void OnNewGame();
  void OnSpaceReady();
  void OnWorldLoaded(World *world);

  // Methods

  // Create a game object from a blueprint (No disk access occurs)
  // filename - Filename of the blueprint to use
  // return - A loaded game object
  Composition* CreateObject(char const *filename);
  Composition* CreateNamedObject(char const *filename, char const *instance_name);
  Composition* CreateObjectAtPosition(char const *filename, Vector3 position);
  Composition* CreateNamedObjectAtPosition(char const *filename, Vector3 position, char const *instance_name);

  // Deletes a game object (deferred)
  void DeleteObject(CompositionId id);

  // Get a gameobject by its ID
  // id - Id of the object to get
  // return - Pointer to the object
  Composition* GetObject(CompositionId id);

  // Get an array gameobjects with a given type name
  // type - Name of the type
  // return - Pointer to the first object
  Composition* GetObjectsByType(String type);

  // Get an object by its instance name
  // name - Name of the instance
  Composition* GetObjectByName(String name);

private:
  // Delete all objects in the manager
  void DeleteObjects(bool check_spaceable = false, bool send_message = true);

  // Name of the tilemap to use to load objects
  String m_object_layer_name;

  // Name of the blueprint
  String m_blueprint_map_name;

  // Array of objects with id as the index
  ObjectArray m_objects;

  // Array of objects to delete
  ObjectArray m_trash;

  // Map of object types to maps of object ids
  StringObjectMap m_by_types;

  // Map of object names to objects
  StringObjectMap m_by_names;

  std::vector<String> m_deserialized_objects;
};

}