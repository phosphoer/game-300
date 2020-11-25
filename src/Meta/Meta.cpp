// David Evans
// 2011/10/01
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Meta.h"

namespace Improbability
{

Meta::CompositionMap Meta::s_compositions;
Meta::ConstantMap Meta::s_constants;

void* Meta::Create(char const *name, void *rhs)
{
  String strname(name);
  if (strname.Find("*") != String::k_end)
    return new int(*((int*)rhs));

  auto it = s_compositions.Find(name);
  ErrorIf(it == s_compositions.End(), "The type %s is not registered with meta!", name);
  MetaComposition *comp = it->second;
  return comp->MetaGetComponentBase(MetaNewable)->New(rhs);  
}

void Meta::Destroy(char const *name, void *obj)
{
  String strname(name);
  if (strname.Find("*") != String::k_end)
  {
    delete ((int*)obj);
    return;
  }

  auto it = s_compositions.Find(strname);
  ErrorIf(it == s_compositions.End(), "The type %s is not registered with meta!", name);

  /* To stop double deletions in Release */
  if(it == s_compositions.End())
    return;

  MetaComposition *comp = it->second;

  comp->MetaGetComponentBase(MetaNewable)->Delete(obj);
}

MetaComposition* Meta::GetMeta(char const *name)
{
  auto it = s_compositions.Find(name);
  if(it == s_compositions.End() && String(name) != "Null")
    it = s_compositions.Find("String");
  ErrorIf(it == s_compositions.End(), "The type %s is not registered with meta!", name);
  return it->second;
}

bool Meta::HasMethod(char const *type, char const *fn)
{
  // Find the meta object by type name
  // Maybe this should assert?
  auto it = s_compositions.Find(type);
  if (it == s_compositions.End())
    return false;

  MetaComposition *comp = it->second;
  MetaCallableBase *callable = comp->MetaGetComponentBase(MetaCallable);
  return callable ? callable->HasMethod(fn) : false;
}

MetaComposition* Meta::GetVariableMeta(char const *type, char const *field)
{
  MetaComposition *composition = GetMeta(type);
  MetaFieldableBase *fieldable = composition->MetaGetComponentBase(MetaFieldable);
  ErrorIf(!fieldable, "Meta::GetVariableMeta: %s does not have MetaFieldable!", composition->m_name);
  return fieldable->GetVariableMeta(field);
}

void Meta::Invoke(char const *type, char const *fn, void *obj)
{
  auto it = s_compositions.Find(type);
  ErrorIf(it == s_compositions.End(), "The type %s is not registered with meta!", type);
  MetaComposition *comp = it->second;
  comp->MetaGetComponentBase(MetaCallable)->Invoke(fn, obj);
}

Variable Meta::InvokeRet(char const *type, char const *fn, void *obj)
{
  auto it = s_compositions.Find(type);
  ErrorIf(it == s_compositions.End(), "The type %s is not registered with meta!", type);
  MetaComposition *comp = it->second;
  return comp->MetaGetComponentBase(MetaCallable)->InvokeRet(fn, obj);
}

void Meta::Initialize()
{
  MetaComposition *node = g_meta_comp_head;
  
  // s_compositions.Reserve(150);
  while (node)
  {
    // node->m_components = new HashMap<String, MetaComponent*>();
    node->m_components = new HashMap<String, MetaComponent*>();
    if (node->m_bind_function)
      node->m_bind_function(node);
    s_compositions[node->GetName()] = node;
    node = node->m_next;
  }

  // s_constants.Reserve(150);
  MetaConstantBase *node_const = g_constant_head;
  while (node_const)
  {
    s_constants[node_const->GetName()] = node_const;
    node_const = node_const->m_next;
  }
}

void Meta::Uninitialize()
{
  for (auto it = s_compositions.Begin(); it != s_compositions.End(); ++it)
  {
    MetaComposition *comp = it->second;
    comp->Uninitialize();
    delete comp->m_components;
  }
}

}