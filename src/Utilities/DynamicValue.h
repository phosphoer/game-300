// David Evans
// 2011/07/14
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Containers/String.h"
#include "../Meta/MetaComposition.h"

namespace Improbability
{

class Variable;

class DynamicValue
{
public:
  DynamicValue() : m_obj(0) {}
  DynamicValue(Variable &rhs);
  DynamicValue(DynamicValue const &rhs) : m_obj(rhs.m_obj), m_str_value(rhs.m_str_value), m_type(rhs.m_type) {}
  DynamicValue(char const *str);
  DynamicValue(String str);

  operator char const*();
  operator String();

  String GetType() {return m_type;}

  template <typename T>
  DynamicValue(T const &obj) : m_obj(reinterpret_cast<void*>(const_cast<T*>(&obj))) {m_type = MetaGetter<T const &>::s_meta->GetName();}

  template <typename T>
  DynamicValue(T const *obj) : m_obj(reinterpret_cast<void*>(const_cast<T*>(obj))) {m_type = MetaGetter<T const *>::s_meta->GetName();}

  template <typename T>
  DynamicValue(T *obj) : m_obj(reinterpret_cast<void*>(obj)) {m_type = MetaGetter<T*>::s_meta->GetName();}

  template <typename T>
  DynamicValue(T *obj, String objType) : m_obj(reinterpret_cast<void*>(obj)), m_type(objType) { }

  template <typename T>
  operator T*() {return reinterpret_cast<T*>(m_obj);}

  template <typename T>
  operator T&() {return *(reinterpret_cast<T*>(m_obj));}

private:
  friend class Variable;
  void *m_obj;
  String m_str_value;
  String m_type;
};

}