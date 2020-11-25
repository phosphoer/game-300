// David Evans
// 2011/05/12
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#undef GetClassName

namespace Improbability
{

class Serializer;
class Space;

class System
{
  friend class Space;

public:
  // Destructor
  virtual ~System(){}

  // This is guaranteed to be called once and only once
  // when the engine is initialized
  virtual void InitializeGlobal() = 0;

  // This is called after all systems have been initialized
  virtual void PostInitializeGlobal(){}

  // This is called each time a new space is pushed to the engine
  virtual void InitializeInstance(){}

  // This is called after the entire space has been initialized
  virtual void PostInitializeInstance(){}

  // This is called every frame on the current space
  virtual void UpdateInstance(float dt){}

  // This is called each time a space is popped from the engine
  virtual void UninitializeInstance(){}

  // This is guarenteed to be called once and only once
  // when the engine is uninitialized
  virtual void UninitializeGlobal() = 0;

  // Copy system data into another system
  // dest - System to copy data into
  // deep - Whether or not to perform a "deep" copy
  virtual void Copy(System *dest, bool deep) = 0;

  // Read data into the system from file
  virtual void Deserialize(Serializer &serializer) = 0;

  // Set the component id
  void SetId(int id) {m_id = id;}

  // Get the ID of the component type
  int GetId() {return m_id;}

  // Get the class name of the resource
  String GetClassName() {return m_name;}

private:
  // Set the name of the system
  // Don't call this...ever
  // Has to be public for resource manager to use it
  void SetClassName(char const *name) {m_name = name;}

  // ID of the meta type
  int m_id;

  // Name of the class
  String m_name;
};

}