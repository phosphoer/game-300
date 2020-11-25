// David Evans
// 2011/09/18
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "AnimatePropertyAction.h"
#include "../Composition/Composition.h"
#include "../FileIO/Serializer.h"

namespace Improbability
{

MakeMeta(AnimatePropertyAction);

void AnimatePropertyAction::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

AnimatePropertyAction::AnimatePropertyAction():
  m_relative(false),
  m_time(0),
  m_begin_int(0),
  m_begin_float(0),
  m_end_int(0),
  m_end_float(0)
{
}

ActionReturn::Type AnimatePropertyAction::Update(float dt)
{
  Composition *obj = GetParent();
  String m_class_name = "Composition";
  void *anim_obj = obj;

  // If they didn't specify a component they want the composition
  if (m_component.GetLength() > 0)
  {
    m_class_name = m_component;
    anim_obj = obj->GetComponent(m_class_name);
  }

  // Find out what type the field is
  MetaComposition *meta = Meta::GetVariableMeta(m_class_name, m_field);
  String type = meta->GetName();

  // Animate the field on the object based on the type (man this is ugly)
  if (type == "int")
  {
    // Get the current value
    int start = Meta::GetVariableByClassName<int>(m_field, m_class_name, anim_obj);

    // Animate the property
    if (m_relative)
      PropertyAnimator::AnimateField(m_field, m_class_name, anim_obj, start + m_begin_int, start + m_end_int, m_time, this);
    else
      PropertyAnimator::AnimateField(m_field, m_class_name, anim_obj, m_begin_int, m_end_int, m_time, this);
  }
  else if (type == "float")
  {
    // Get the current value
    float start = Meta::GetVariableByClassName<float>(m_field, m_class_name, anim_obj);

    // Animate the property
    if (m_relative)
      PropertyAnimator::AnimateField(m_field, m_class_name, anim_obj, start + m_begin_float, start + m_end_float, m_time, this);
    else
      PropertyAnimator::AnimateField(m_field, m_class_name, anim_obj, m_begin_float, m_end_float, m_time, this);
  }

  return ActionReturn::Remove;
}

void AnimatePropertyAction::Deserialize(Serializer &serializer)
{
  Read(serializer, m_field, "Field");
  Read(serializer, m_component, "Component");
  Read(serializer, m_time, "Time");
  Read(serializer, m_relative, "IsRelative");

  Read(serializer, m_begin_int, "BeginInt");
  Read(serializer, m_begin_float, "BeginFloat");

  Read(serializer, m_end_int, "EndInt");
  Read(serializer, m_end_float, "EndFloat");
}

}