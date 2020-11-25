// David Evans
// 2011/05/19
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "System.h"
#include "SystemProfiler.h"
#include "../Containers/HashMap.h"

namespace Improbability
{

class EngineCore;

// It's the place
// It also stores a set of systems
class Space
{
  friend class EngineCore;

public:

  // Listener class stores info about a listener
  class Listener
  {
  public:
    String m_name;
    void *m_obj;
  };

  // Provides lookup of a system by name
  typedef HashMap<String, System*> SystemMap;
  typedef std::vector<System*> SystemArray;

  // Container of listeners
  typedef std::map<void*, Listener> ListenerMap;

  // Map of strings to an array of listenrs
  typedef std::map<String, ListenerMap> EventListenerMap;

  // Map of strings  to a listener
  typedef HashMap<String, Listener> EventSingleListenerMap;

public:
  static void BindMeta(MetaComposition *comp);
  
  // Constructor
  Space();

  // Initializes all systems within the space
  void Initialize(String const &space_name);

  // Update all systems within the space
  // dt - Seconds elapsed
  void Update(float dt);

  // Uninitialize all systems in the space
  void Uninitialize();

  // Resets the space
  void Reset();

  // Add a system to the space to manager
  // system - The system to add
  void AddSystem(System *system);

  // Clones the space and all systems within it
  // full_copy - True if systems should be told to make a deep copy
  // return - Pointer to the cloned space
  Space* Clone(bool full_copy);

  // Copies data from this space to another
  // dest - Space to copy into
  // full_copy - True if system should be told to make a deep copy
  void Copy(Space *dest, bool full_copy);

  // Load the space from a file
  void Deserialize(Serializer &serializer);

  // Getters
  System* GetSystem(String name);
  template <typename T>
  T* GetSystem();

  String GetName() {return m_name;}
  void SetName(String const &name) {m_name = name;}

private:
  // Used to track how many systems have been initialized
  static HashMap<String, int> s_initialize_count;

  // Map of systems in the space
  SystemMap m_system_map;
  SystemArray m_system_array;

  // Profiler to time systems
  SystemProfiler m_profiler;

  // Time between updating profiler
  float m_profile_update;

  // Map of event strings to an array of listeners
  EventListenerMap m_listeners;

  // Map of event strings to a single listener
  EventSingleListenerMap m_single_listeners;
  String m_name;
};

template <typename T>
T* Space::GetSystem()
{
  auto it = m_system_map.Find(Meta::GetMeta<T>()->GetName());

  if (it != m_system_map.End())
  {
    System *system = it->second;
    return reinterpret_cast<T*>(system);
  }

  return 0;
}

}