// David Evans
// 2011/10/01
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "DynamicValue.h"

namespace Improbability
{

// The Variable class is similar to DynamicValue but it allocates
// a new instance of the type given to it and stores that.
// IMPORTANT - If the type does not have a MetaNewable, it will
// allocate it as a POD type (the constructor / destructor will NOT be called)
// MAKE SURE any types you put in a Variable either have a MetaNewable, 
// or don't need a constructor / destructor to be properly handled.
class Variable
{
public:
  Variable(): m_size(0), m_obj(0), m_type("Null") {}
  Variable(Variable const &rhs);
  ~Variable();

  template <typename T>
  Variable(T const &obj);

  template <typename T>
  Variable(T const *obj);

  template <typename T>
  Variable& operator=(T const &obj);
  Variable& operator=(Variable const &rhs);
  Variable& operator=(DynamicValue const &rhs);

  template <typename T>
  operator T();

  String GetType() {return m_type;}
  bool IsNull() {return !m_obj;}

private:
  friend class DynamicValue;
  void FreeData();

  unsigned m_size;
  void *m_obj;
  String m_type;
};

template <typename T>
Variable::Variable(T const &obj)
{
  m_type = MetaGetter<T>::s_meta->GetName();
  m_size = sizeof(T);

  // Do something terrible
  if (!Meta::GetMeta<T>()->MetaGetComponentBase(MetaNewable))
  {
    m_obj = new char[m_size];
    *((T*)m_obj) = obj;
    return;
  }

  m_obj = new T(obj);
}

template <typename T>
Variable::Variable(T const *obj)
{
  m_type = MetaGetter<T>::s_meta->GetName();
  m_size = sizeof(T);

  // Do something terrible
  if (!Meta::GetMeta<T>()->MetaGetComponentBase(MetaNewable))
  {
    m_obj = new char[m_size];
    *((T*)m_obj) = *obj;
    return;
  }

  m_obj = new T(*obj);
}

template <typename T>
Variable& Variable::operator=(T const &obj)
{
  // Delete existing data
  if (m_obj)
    FreeData();

  m_type = MetaGetter<T>::s_meta->GetName();
  m_size = sizeof(T);

  // Do something terrible
  if (!Meta::GetMeta<T>()->MetaGetComponentBase(MetaNewable))
  {
    m_obj = new char[m_size];
    *((T*)m_obj) = obj;
    return *this;
  }

  m_obj = new T(obj);

  return *this;
}

template <typename T>
Variable::operator T()
{
  return *((T*)m_obj);
}

}