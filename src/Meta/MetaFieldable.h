// David Evans
// 2011/10/03
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "MetaComponent.h"
#include "MetaComposition.h"
#include "../Utilities/Defines.h"
#include "../Utilities/DebugErrors.h"
#include "../Containers/HashMap.h"
#include "../Containers/String.h"

namespace Improbability
{

class MetaComposition;

#define MetaRegisterField(FieldType, Field) \
  RegisterVariable<FieldType>(#Field, OffsetOf(ThisType, Field))

class MetaFieldableBase : public MetaComponent
{
public:
  // Get a pointer to the variable from the fieldable
  // name - Name of the variable
  // obj - pointer to the instance
  virtual void* GetVariable(char const *name, void *obj) = 0;

  // Get a pointer to the meta for a field
  // name - Name of the field
  virtual MetaComposition* GetVariableMeta(char const *name) = 0;

  // Inherited methods
  String GetComponentName() {return "MetaFieldable";}
};

template <typename T>
class MetaFieldable : public MetaFieldableBase
{
public:
  // Register a variable with the meta component
  // name - Name of the field
  // offset - Byte offset into the class
  // MemberType - Type of the field
  template <typename MemberType>
  void RegisterVariable(char const *name, int offset);

  // Get a pointer to a member variable
  // name - Name of the field
  // obj - Pointer to the instance
  void* GetVariable(char const *name, void *obj);

  // Get the meta composition of a member variable
  // name - Name of the field
  MetaComposition* GetVariableMeta(char const *name);

private:
  // Map of member field names to their offset into the class
  HashMap<String, int> m_member_offsets;

  // Map of member field names to their own meta composition
  HashMap<String, MetaComposition*> m_member_meta;

private:
  void GetMembers(String str, std::list<String> &members);
};

template <typename T>
template <typename MemberType>
void MetaFieldable<T>::RegisterVariable(char const *name, int offset)
{
  m_member_offsets[name] = offset;
  m_member_meta[name] = MetaGetter<MemberType>::s_meta;
}

template <typename T>
void* MetaFieldable<T>::GetVariable(char const *name, void *obj)
{
  std::list<String> members;
  GetMembers(name, members);
  String var_name = members.front();
  members.pop_front();

  auto it = m_member_offsets.Find(var_name);
  ErrorIf(it == m_member_offsets.End(), "MetaFieldable::GetVariable: Field %s doesn't exist!", name);

  size_t offset = it->second;
  char *self_byte = (char *)obj;
  void *var = (void*)(self_byte + offset);

  MetaComposition *member_meta = GetVariableMeta(var_name);
  String remaining_members;

  auto str_it = members.begin();
  auto str_end = members.end();
  for (; str_it != str_end; ++str_it)
    remaining_members += *str_it;

  void *final_var = var;

  if (remaining_members.GetLength())
    final_var = member_meta->MetaGetComponentBase(MetaFieldable)->GetVariable(remaining_members, var);

  return final_var;
}

template <typename T>
MetaComposition* MetaFieldable<T>::GetVariableMeta(char const *name)
{
  std::list<String> members;
  GetMembers(name, members);
  String var_name = members.front();
  members.pop_front();

  auto it = m_member_meta.Find(var_name);
  if (it == m_member_meta.End())
    return 0;

  MetaComposition *member_meta = it->second;
  String remaining_members;

  auto str_it = members.begin();
  auto str_end = members.end();
  for (; str_it != str_end; ++str_it)
    remaining_members += *str_it;

  MetaComposition *final_var = member_meta;

  if (remaining_members.GetLength())
    final_var = member_meta->MetaGetComponentBase(MetaFieldable)->GetVariableMeta(remaining_members);

  return final_var;
}

template <typename T>
void MetaFieldable<T>::GetMembers(String str, std::list<String> &members)
{
  while (str.GetLength())
  {
    // Find the first dot
    int index = str.Find(".");

    // If no dot is found, add the remaining string
    // as a member and return
    if (index == String::k_end)
    {
      members.push_back(str);
      return;
    }

    // Get the member variable in front of the string
    // and add it
    String member(str, 0, index);
    members.push_back(member);

    // Remove that part of the string
    str = String(str, index + 1, str.GetLength());
  }
}

}