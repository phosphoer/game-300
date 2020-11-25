// David Evans
// 2011/05/19
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Composition.h"
#include "../FileIO/Serializer.h"

namespace Improbability
{

MakeMeta(Composition);

void Composition::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaFieldable);
  comp->MetaGetComponent(MetaFieldable)->MetaRegisterField(Vector4, m_position);
}

Composition::Composition():
  m_position(0.0f, 0.0f, 0.0f, 1.0f),
  m_orientation(0.0f, 0.0f, 0.0f),
  m_id(-1),
  m_type_name("UninitializedComposition"),
  m_instance_name("UninitializedComposition"),
  m_spaceable(true),
  m_next(0),
  m_prev(0)
{
}

Composition::~Composition()
{
  for (auto it = m_component_array.begin(); it != m_component_array.end(); ++it)
  {
    Component *component = *it;
    delete component;
  }
}

void Composition::Initialize()
{
  // Initialize name
  m_instance_name = String::Formatted("%s_%i", m_type_name.GetCString(), m_id);

  // Initialize all components
  ComponentArray::iterator it = m_component_array.begin();
  ComponentArray::iterator end = m_component_array.end();
  for (; it != end; ++it)
  {
    Component *component = *it;
    component->Initialize();
  }

  // Post initialize all components
  it = m_component_array.begin();
  for (; it != end; ++it)
  {
    Component *component = *it;
    component->PostInitialize();
  }
}

void Composition::Deserialize(Serializer &serializer)
{
  // Read in object attributes
  Read(serializer, m_type_name, "Name");
  Read(serializer, m_position, "Position");
  Read(serializer, m_spaceable, "IsSpaceable");

  // Iterate over the components and load them
  serializer.EnterElement("Components");
  for (serializer.Begin(); serializer.IsGood(); serializer.Iterate())
  {
    // Get the name of the component and create it
    String name = serializer.GetName();
    Component *component = Meta::Create<Component>(name.GetCString());

    // Set component fields and serialize it
    component->m_name = name;
    component->m_parent = this;
    Read(serializer, *component);

    // Add the component
    AddComponent(component);
  }
}

void Composition::Uninitialize()
{
  // Uninitialize all components
  ComponentArray::iterator it = m_component_array.begin();
  ComponentArray::iterator end = m_component_array.end();
  for (; it != end; ++it)
  {
    Component *component = *it;
    component->Uninitialize();
  }
}

void Composition::AddComponent(Component *component)
{
  // Add the component
  m_component_array.push_back(component);

  // Also add the id and index to the lookup table
  auto it = m_component_lookup.Find(component->GetClassName());
  ErrorIf(it != m_component_lookup.End(), "Composition::AddComponent: Duplicate Component!");
  m_component_lookup[component->GetClassName()] = m_component_array.size() - 1;
}

Composition* Composition::Clone()
{
  Composition *clone = new Composition();
  clone->m_type_name = m_type_name;
  clone->m_instance_name = m_instance_name;
  clone->m_position = m_position;
  clone->m_orientation = m_orientation;
  clone->m_spaceable = m_spaceable;

  auto it = m_component_array.begin();
  auto end = m_component_array.end();
  for (; it != end; ++it)
  {
    Component *comp = *it;
    Component *comp_copy = comp->Clone();
    comp->BaseCopy(comp_copy);
    clone->AddComponent(comp_copy);
    comp_copy->m_parent = clone;
  }

  return clone;
}

void Composition::Invoke(char const *fn)
{
  for (auto it = m_component_array.begin(); it != m_component_array.end(); ++it)
  {
    String component_type = (*it)->GetClassName();
    if (Meta::HasMethod(component_type, fn))
      Meta::Invoke(component_type, fn, *it);
  }
}

Component* Composition::GetComponent(char const *type)
{
  // Lookup component index
  auto it = m_component_lookup.Find(type);

  // Return the component if found
  if (it != m_component_lookup.End())
  {
    return (m_component_array[it->second]);
  }

  return 0;
}

}