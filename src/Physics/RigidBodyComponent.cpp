// David Evans
// 2011/09/10
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "RigidBodyComponent.h"
#include "../Composition/Composition.h"
#include "../Math/Matrix2.h"

namespace Improbability
{

MakeMeta(RigidBodyComponent);

void RigidBodyComponent::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

RigidBodyComponent::RigidBodyComponent():
  m_type(CollisionType::Box),
  m_size(1.0f, 1.0f, 1.0f)
{
}

void RigidBodyComponent::Initialize()
{
}

void RigidBodyComponent::Uninitialize()
{
}

Component* RigidBodyComponent::Clone()
{
  Component *clone = new RigidBodyComponent(*this);
  return clone;
}

void RigidBodyComponent::Deserialize(Serializer &serializer)
{
}

}