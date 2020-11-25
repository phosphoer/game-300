// David Evans
// 2011/09/18
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "SetVelocityAction.h"
#include "../Composition/Composition.h"
#include "../Physics/DynamicComponent.h"
#include "../FileIO/Serializer.h"

namespace Improbability
{

MakeMeta(SetVelocityAction);

void SetVelocityAction::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

ActionReturn::Type SetVelocityAction::Update(float dt)
{
  Composition *obj = GetParent();
  DynamicComponent *dyn = obj->GetComponent<DynamicComponent>();
  if (!dyn)
    return ActionReturn::Continue;

  if (m_ignore_zeroes)
  {
    Vector3 current_v = dyn->GetVelocity();
    current_v.m_x = (m_vector.m_x != 0.0f) ? m_vector.m_x : current_v.m_x;
    current_v.m_y = (m_vector.m_y != 0.0f) ? m_vector.m_y : current_v.m_y;
    current_v.m_z = (m_vector.m_z != 0.0f) ? m_vector.m_z : current_v.m_z;
    dyn->SetVelocity(current_v);
  }
  else
  {
    dyn->SetVelocity(m_vector);
  }

  return ActionReturn::Continue;
}

void SetVelocityAction::Deserialize(Serializer &serializer)
{
  Read(serializer, m_vector, "Speed");
  Read(serializer, m_ignore_zeroes, "IgnoreZeroes");
}

}