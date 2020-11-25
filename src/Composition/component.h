// David Evans
// 2011/05/18
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "../Math/Vector4.h"
#include "../Containers/String.h"

#pragma once

#undef GetClassName

namespace Improbability
{

class Serializer;
class Composition;
class String;

// Component base class
// All components inherit from this
class Component
{
  friend class Composition;

public:
   // Destructor
  virtual ~Component() {}

  // Initializes the component's values
  virtual void Initialize() = 0;

  // Post initialize is invoked after all components are initialized
  virtual void PostInitialize(){}

  // Uninitializes the component
  virtual void Uninitialize() = 0;

  // Clone the component
  virtual Component* Clone() = 0;

  // Copies the base data from the component into another one
  void BaseCopy(Component *comp) {comp->m_name = m_name;}

  // Load the component from file
  virtual void Deserialize(Serializer &serializer) = 0;

  // Save the component to file
  virtual void Serialize(Serializer &serializer) = 0;

  // Get the ID of the component type
  String GetClassName() {return m_name;}

  // Get the component's parent
  Composition* GetParent() {return m_parent;}

  // Get a component from the parent
  template <typename T>
  T* GetSibling() {return m_parent->GetComponent<T>();}

private:
  // Name of the component
  String m_name;

  // Parent of the component
  Composition *m_parent;
};

}