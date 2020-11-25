// David Evans
// 2011/05/23
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "MetaComponent.h"
#include "../Containers/String.h"
#include "../Containers/HashMap.h"

namespace Improbability
{

class MetaComposition;
extern MetaComposition *g_meta_comp_head;
extern MetaComposition *g_meta_comp_tail;

// Use this to add a meta component (e.g. comp->MetaAddComponent(MyComponent))
// T - Name of component
#define MetaAddComponent(ComponentType) \
  AddComponent<ComponentType<ThisType> >(#ComponentType)

// Use this to get a meta component
// T - Name of component
#define MetaGetComponent(ComponentType) GetComponent<ComponentType<ThisType> >(#ComponentType)
#define MetaGetComponentBase(ComponentType) GetComponent<ComponentType##Base>(#ComponentType)

// Used to store a pointer to the composition accessible
// by a typename
// T - Typename of class
template <typename T>
class MetaGetter
{
public:
  static MetaComposition *s_meta;
};

class Meta;

// The meta base class includes an implicitly linked list
// and defines functions that metadata objects can implement
class MetaComposition
{
  friend class Meta;
  
public:
  static void BindMeta(MetaComposition*);
  
  // Constructor
  MetaComposition(char const *name, void (*BindFunction)(MetaComposition*));

  // Uninitialize the composition
  void Uninitialize();

  // Add a component
  // name - Name of the component to add
  // ComponentType - Type of the component to add
  template <typename ComponentType>
  void AddComponent(char const *name);

  // Get a component
  // name - Name of the component to get
  // ComponentType - Type of the component to get
  template <typename ComponentType>
  ComponentType* GetComponent(char const *name);

  // Get type this is meta for
  String GetName() {return m_name;}

private:
  // The name of the type this is meta for
  char const *m_name;

  // Map of meta components
  HashMap<String, MetaComponent*> *m_components;

  void (*m_bind_function)(MetaComposition*);

  // The implicitly linked list pointer to next meta base
  MetaComposition *m_next;
};

template <typename ComponentType>
void MetaComposition::AddComponent(char const *name)
{
  MetaComponent *component = GetComponent<ComponentType>(name);
  if (!component) 
    component = new ComponentType();
  (*m_components)[name] = component;
  component->m_parent = this;
}

template <typename ComponentType>
ComponentType* MetaComposition::GetComponent(char const *name)
{
  auto it = m_components->Find(name);
  if (it == m_components->End())
    return 0;

  return reinterpret_cast<ComponentType*>(it->second);
}

}
