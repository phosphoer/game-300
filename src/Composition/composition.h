// David Evans
// 2011/05/18
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "Component.h"
#include "../Math/Vector4.h"
#include "../Math/Matrix4.h"
#include "../Containers/HashMap.h"
#include "../math/Quaternion.h"

#undef GetClassName

namespace Improbability
{

// This type defines the Id of a composition
typedef unsigned int CompositionId;

class Serializer;
class ObjectManager;
class MetaComposition;
class String;

// A composition is a container for components
// Essentially a game object for most purposes
class Composition
{
public:
  friend class ObjectManager;

  // This type is a fast iterate-able container for components
  typedef std::vector<Component*> ComponentArray;

  // Container used in conjunction with ComponentArray to make lookups based on type
  typedef HashMap<String, int> ComponentLookup;

public:
  static void BindMeta(MetaComposition*);
  
  // Constructor
  Composition();
  ~Composition();

  // Initializes all components
  void Initialize();

  // Uninitializes all components
  void Uninitialize();

  // Write the composition to file
  void Serialize(Serializer &serializer) { }

  // Load the composition from file
  void Deserialize(Serializer &serializer);

  // Add a component to the composition
  void AddComponent(Component *component);

  // Clone the composition, copies all components
  Composition* Clone();

  // Invokes a message on the composition and all components
  // contained
  // fn - Name of the function to call
  // p0...p3 - Parameters to the member function
  // P0...P3 - Type of each parameter
  void Invoke(char const *fn);
  template <typename P0>
  void Invoke(char const *fn, P0);
  template <typename P0, typename P1>
  void Invoke(char const *fn, P0, P1);
  template <typename P0, typename P1, typename P2>
  void Invoke(char const *fn, P0, P1, P2);
  template <typename P0, typename P1, typename P2, typename P3>
  void Invoke(char const *fn, P0, P1, P2, P3);

  // Set position of the composition
  void SetPosition(Vector4 position) {m_position = position;}
  void SetPosition(Vector3 position) {m_position = Vector4(position, 1.0f);}
  void AddPosition(Vector3 vector) {m_position += vector;}

  // Set orientation of composition
  void SetOrientation(Vector3 orientation) {m_orientation = orientation;}
  void AddOrientation(Vector3 orientation) {m_orientation += orientation;}

  // Get position of the composition
  Vector4 GetPosition() {return m_position;}

  // Get orientation of the composition
  Vector3 GetOrientation() {return m_orientation.Normalized();}

  // Gets a component from the composition by template type
  // T - Type of the component
  template <typename T>
  T* GetComponent();

  // Get a component from the composition by string name
  // type - String type of the component
  Component* GetComponent(char const *type);

  // Gets the instance's id
  CompositionId GetId() {return m_id;}

  // Get the composition type name
  String GetType() {return m_type_name;}
  String GetName() {return m_instance_name;}

  // Get next composition of same type
  Composition* GetNext() {return m_next;}

  // Get whether is spaceable
  bool IsSpaceable() {return m_spaceable;}

private:
  // Position of the instance
  Vector4 m_position;

  // Orientation of the object
  Vector3 m_orientation;

  // Id of the instance
  CompositionId m_id;

  // Name of the game object type
  String m_type_name;

  // Name of the instance
  String m_instance_name;

  // Whether or not the object can propagate to spaces
  bool m_spaceable;

  // Pointer to next object of same type
  Composition *m_next;
  Composition *m_prev;

  // Array of components in the composition
  ComponentArray m_component_array;

  // Lookup table to find a component in the array by type
  ComponentLookup m_component_lookup;
};

template <typename P0>
void Composition::Invoke(char const *fn, P0 p0)
{
  for (auto it = m_component_array.begin(); it != m_component_array.end(); ++it)
  {
    String component_type = (*it)->GetClassName();
    if (Meta::HasMethod(component_type, fn))
      Meta::Invoke(component_type, fn, *it, p0);
  }
}

template <typename P0, typename P1>
void Composition::Invoke(char const *fn, P0 p0, P1 p1)
{
  for (auto it = m_component_array.begin(); it != m_component_array.end(); ++it)
  {
    String component_type = (*it)->GetClassName();
    if (Meta::HasMethod(component_type, fn))
      Meta::Invoke(component_type, fn, *it, p0, p1);
  }
}

template <typename P0, typename P1, typename P2>
void Composition::Invoke(char const *fn, P0 p0, P1 p1, P2 p2)
{
  for (auto it = m_component_array.begin(); it != m_component_array.end(); ++it)
  {
    String component_type = (*it)->GetClassName();
    if (Meta::HasMethod(component_type, fn))
      Meta::Invoke(component_type, fn, *it, p0, p1, p2);
  }
}

template <typename P0, typename P1, typename P2, typename P3>
void Composition::Invoke(char const *fn, P0 p0, P1 p1, P2 p2, P3 p3)
{
  for (auto it = m_component_array.begin(); it != m_component_array.end(); ++it)
  {
    String component_type = (*it)->GetClassName();
    if (Meta::HasMethod(component_type, fn))
      Meta::Invoke(component_type, fn, *it, p0, p1, p2, p3);
  }
}

template <typename T>
T* Composition::GetComponent()
{
  // Lookup component index
  auto it = m_component_lookup.Find(Meta::GetMeta<T>()->GetName());

  // Return the component if found
  if (it != m_component_lookup.End())
  {
    return reinterpret_cast<T*>(m_component_array[it->second]);
  }

  return 0;
}

}