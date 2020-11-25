// David Evans
// 2011/05/23
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Containers/String.h"

namespace Improbability
{

class MetaConstantBase;
extern MetaConstantBase *g_constant_head;
extern MetaConstantBase *g_constant_tail;

class MetaConstantBase
{
public:
  MetaConstantBase() : m_next(0) {}
  virtual ~MetaConstantBase() {}
  virtual String GetName() = 0;
  virtual void* GetValue() = 0;

  MetaConstantBase *m_next;
};

template <typename T>
class MetaConstant : public MetaConstantBase
{
public:
  MetaConstant(String name, T value):
    MetaConstantBase(),
    m_name(name), 
    m_value(value)
  {
    // Insert the meta data into the linked list
    if (!g_constant_head)
    {
      g_constant_head = this;
    }
    else
    {
      m_next = g_constant_head;
      g_constant_head = this;
    }

    g_constant_tail = this;
  }

  String GetName() {return m_name;}
  void* GetValue() {return &m_value;}

  String m_name;
  T m_value;
};

}