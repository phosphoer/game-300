// David Evans
// 2011/09/18
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "WaitForPropertyValue.h"
#include "../Core/EngineCore.h"
#include "../Composition/Composition.h"
#include "../Physics/DynamicComponent.h"
#include "../FileIO/Serializer.h"

namespace Improbability
{

MakeMeta(WaitForPropertyValue);

void WaitForPropertyValue::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

ActionReturn::Type WaitForPropertyValue::Update(float dt)
{
  Composition *obj = GetParent();

  void *obj_field = obj;
  if (m_component.GetLength() > 0) 
    obj_field = obj->GetComponent(m_component);

  SetBlocking(true);

  if (m_type == "float")
  {
    float value = Meta::GetVariableByClassName<float>(m_field, m_class_type, obj_field);
    if (value == m_value_f)
      SetBlocking(false);
  }
  else if (m_type == "int")
  {
    int value = Meta::GetVariableByClassName<int>(m_field, m_class_type, obj_field);
    if (value == m_value_i)
      SetBlocking(false);
  }

  return ActionReturn::Continue;
}

void WaitForPropertyValue::Deserialize(Serializer &serializer)
{
  Read(serializer, m_field, "Field");
  Read(serializer, m_component, "Component");

  m_class_type = "Composition";
  if (m_component.GetLength() > 0) m_class_type = m_component;

  m_type = Meta::GetVariableMeta(m_class_type, m_field)->GetName();

  if (m_type == "float")
    Read(serializer, m_value_f, "Value");
  else if (m_type == "int")
    Read(serializer, m_value_i, "Value");
  else
    ErrorIf(1, "WaitForPropertyValue::Deserialize: Type %s is not supported.", m_type.GetCString());
}

}