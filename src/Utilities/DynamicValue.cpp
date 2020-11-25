// David Evans
// 2011/10/11
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "DynamicValue.h"
#include "Variable.h"

namespace Improbability
{

DynamicValue::DynamicValue(char const *str)
{
  m_obj = 0;
  m_str_value = str;
  m_type = "String";
}

DynamicValue::DynamicValue(String str)
{
  m_obj = 0;
  m_str_value = str;
  m_type = "String";
}

DynamicValue::DynamicValue(Variable &rhs) : m_obj(rhs.m_obj)
{
  if (rhs.m_type == "String") m_str_value = static_cast<String>(rhs);
  m_type = rhs.m_type;
}

DynamicValue::operator String()
{
  ErrorIf(m_type != "String", "DynamicValue: String type requested but internal type is %s.", m_type.GetCString());
  return m_str_value;
}

DynamicValue::operator char const*()
{
  ErrorIf(m_type != "String", "DynamicValue: String type requested but internal type is %s.", m_type.GetCString());
  return m_str_value.GetCString();
}

}