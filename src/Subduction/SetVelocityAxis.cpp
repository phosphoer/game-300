// David Evans
// 2011/09/18
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "SetVelocityAxis.h"
#include "../Core/EngineCore.h"
#include "../Composition/Composition.h"
#include "../Physics/DynamicComponent.h"
#include "../FileIO/Serializer.h"
#include "../Tile/TileComponent.h"

namespace Improbability
{

MakeMeta(SetVelocityAxis);

void SetVelocityAxis::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAxisChange);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnPlayerSideCollision);
}

SetVelocityAxis::SetVelocityAxis():
  m_axis(0),
  m_ignore_zeroes(false),
  m_collide(false)
{
}

void SetVelocityAxis::Initialize()
{
  g_engine_core->AddEventListener(MessageBank::OnAxisChange, this);
  g_engine_core->AddEventListener(MessageBank::OnPlayerSideCollision, this);
}

void SetVelocityAxis::Start()
{
  m_axis = g_engine_core->SystemCall("TileSystem", "GetWorldAxis");
}

ActionReturn::Type SetVelocityAxis::Update(float dt)
{
  Composition *obj = GetParent();
  DynamicComponent *dyn = obj->GetComponent<DynamicComponent>();
  if (!dyn)
    return ActionReturn::Continue;

  Vector3 current_v = dyn->GetVelocity();
  float acceleration = .01f;
  float xdirection = float(m_vector.m_x > 0 ? 1 : -1);
  float ydirection = float(m_vector.m_y > 0 ? 1 : -1);
  if (m_axis == 0)
  {
    float xspeed = abs(current_v.m_z) >= abs(m_vector.m_x) ? abs(m_vector.m_x) : abs(current_v.m_z) + acceleration * dt;
    float yspeed = abs(current_v.m_y) >= abs(m_vector.m_y) ? abs(m_vector.m_y) : abs(current_v.m_y) + acceleration * dt;
    current_v.m_z = -xspeed*xdirection;
    current_v.m_y = yspeed*ydirection;
    //current_v.m_z = (!m_ignore_zeroes || m_vector.m_x != 0.0f) ? -m_vector.m_x : 0.f;
    //current_v.m_y = (!m_ignore_zeroes || m_vector.m_y != 0.0f) ? m_vector.m_y : 0.f;
  }
  else if (m_axis == 1)
  {
    float xspeed = abs(current_v.m_x) >= abs(m_vector.m_x) ? abs(m_vector.m_x) : abs(current_v.m_x) + acceleration * dt;
    float yspeed = abs(current_v.m_z) >= abs(m_vector.m_y) ? abs(m_vector.m_y) : abs(current_v.m_z) + acceleration * dt;
    current_v.m_x = xspeed*xdirection;
    current_v.m_z = yspeed*ydirection;
    //current_v.m_x = (!m_ignore_zeroes || m_vector.m_x != 0.0f) ? m_vector.m_x : 0.f;
    //current_v.m_z = (!m_ignore_zeroes || m_vector.m_y != 0.0f) ? m_vector.m_y : 0.f; 
  }
  else if (m_axis == 2)
  {
    float xspeed = abs(current_v.m_x) >= abs(m_vector.m_x) ? abs(m_vector.m_x) : abs(current_v.m_x) + acceleration * dt;
    float yspeed = abs(current_v.m_y) >= abs(m_vector.m_y) ? abs(m_vector.m_y) : abs(current_v.m_y) + acceleration * dt;
    current_v.m_x = xspeed*xdirection;
    current_v.m_y = yspeed*ydirection;
    //current_v.m_x = (!m_ignore_zeroes || m_vector.m_x != 0.0f) ? m_vector.m_x : 0.f;
    //current_v.m_y = (!m_ignore_zeroes || m_vector.m_y != 0.0f) ? m_vector.m_y : 0.f; 
  }
  //dyn->SetHorizontalVelocity(current_v);
  if(!m_collide)
  {
    if(dyn->GetSibling<TileComponent>()->IsGrounded())
      dyn->SetMaxHorizontalVelocity(m_vector.m_x ? m_vector.m_x : m_vector.m_y);
    dyn->AddHorizontalVelocity(current_v);
  }
  //dyn->SetVelocity(current_v);
  //dyn->SetAccelerationVector(current_v.Normalized());

  m_collide = false;
  return ActionReturn::Continue;
}

void SetVelocityAxis::Uninitialize()
{
  g_engine_core->RemoveEventListener(MessageBank::OnAxisChange, this);
  g_engine_core->RemoveEventListener(MessageBank::OnPlayerSideCollision, this);
}

void SetVelocityAxis::Deserialize(Serializer &serializer)
{
  Read(serializer, m_vector, "Speed");
  Read(serializer, m_ignore_zeroes, "IgnoreZeroes");
}

void SetVelocityAxis::OnAxisChange(int old_axis, int new_axis)
{
  m_axis = new_axis;
}

void SetVelocityAxis::OnPlayerSideCollision()
{
  //m_collide = true;
}

void SetVelocityAxis::SetVector( Vector3 vector )
{
  m_vector = vector;
}

}