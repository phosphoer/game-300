// David Evans
// 2011/10/01
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Variable.h"

namespace Improbability
{

Variable::Variable(Variable const &rhs)
{
  m_size = rhs.m_size;
  m_type = rhs.m_type;

  // Do something terrible
  if (!Meta::GetMeta(m_type)->MetaGetComponentBase(MetaNewable))
  {
    m_obj = new char[m_size];
    std::memcpy(m_obj, rhs.m_obj, m_size);
    return;
  }

  m_obj = Meta::Create(m_type, rhs.m_obj);
}

Variable::~Variable()
{
  FreeData();
}

Variable& Variable::operator=(Variable const &rhs)
{
  m_size = rhs.m_size;
  m_type = rhs.m_type;

  // Do something terrible
  if (!Meta::GetMeta(m_type)->MetaGetComponentBase(MetaNewable))
  {
    m_obj = new char[m_size];
    std::memcpy(m_obj, rhs.m_obj, m_size);
    return *this;
  }

  m_obj = Meta::Create(m_type, rhs.m_obj); 

  return *this;
}

/* Horrible horrible hacks needed by PyMessaging */
Variable& Variable::operator=(DynamicValue const &rhs)
{
  m_type = rhs.m_type;
  m_obj = rhs.m_obj;
  m_size = 0;

  return *this;
}

void Variable::FreeData()
{
  // Do terrible things
  if (m_type == "Null" || *(m_type.GetCString()) == 0)
    return;
    
  MetaComposition *comp = Meta::GetMeta(m_type);

  if (!comp->MetaGetComponentBase(MetaNewable))
  {
    delete (char*)m_obj;
    return;
  }

  Meta::Destroy(m_type, m_obj);

  //m_type = "Null";
  m_size = 0;
  m_obj = 0;
}

}