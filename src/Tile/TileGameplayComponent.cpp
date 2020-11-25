// Aaron Biddlecom
// 2011/07/23
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "TileGameplayComponent.h"
#include "../FileIO/Serializer.h"
#include "../Composition/Composition.h"
#include "../Math/Matrix4.h"
#include "../Core/EngineCore.h"

namespace Improbability
{

MakeMeta(TileGameplayComponent);

void TileGameplayComponent::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaAddComponent(MetaFieldable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetDefaultAxis);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetPreviousAxis);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(SetPreviousAxis);
}

TileGameplayComponent::TileGameplayComponent():
  m_anchor_axis(-1),
  m_anchor_begin(false),
  m_default_axis(0),
  m_prev_axis(-1),
  m_rotation_active(true),
  m_anchor_active(true),
  m_tile_type(0)
{
}

TileGameplayComponent::TileGameplayComponent(TileGameplayComponent const &rhs):
  m_anchor_axis(rhs.m_anchor_axis),
  m_anchor_begin(rhs.m_anchor_begin),
  m_default_axis(rhs.m_default_axis),
  m_prev_axis(rhs.m_prev_axis),
  m_rotation_active(rhs.m_rotation_active),
  m_anchor_active(rhs.m_anchor_active),
  m_tile_type(rhs.m_tile_type)
{
}

void TileGameplayComponent::Initialize()
{
}

void TileGameplayComponent::Uninitialize()
{
}

Component* TileGameplayComponent::Clone()
{
  TileGameplayComponent *clone = new TileGameplayComponent(*this);
  return clone;
}

void TileGameplayComponent::Deserialize(Serializer &serializer)
{
  Read(serializer, m_default_axis, "DefaultAxis");
  Read(serializer, m_anchor_axis, "AnchorAxis");
  Read(serializer, m_tile_type, "TileType");
  m_prev_axis = -1;
}

int TileGameplayComponent::GetDefaultAxis()
{
  return m_rotation_active ? m_default_axis : -1;
}
int TileGameplayComponent::GetPreviousAxis()
{
  return m_rotation_active ? m_prev_axis : -1;
}
void TileGameplayComponent::SetPreviousAxis( int axis )
{
  m_prev_axis = axis;
}
void TileGameplayComponent::SetRotationActive( bool active )
{
  m_rotation_active = active;
}
int TileGameplayComponent::GetAnchorAxis()
{
  return m_anchor_axis;
}
void TileGameplayComponent::SetAnchorActive( bool active )
{
  m_anchor_active = active;
}

}