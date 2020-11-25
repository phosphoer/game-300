// Aaron Biddlecom
// 2011/09/22
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "PlatformDropAction.h"
#include "../Composition/Composition.h"
#include "../Physics/DynamicComponent.h"
#include "../Tile/TileComponent.h"
#include "../FileIO/Serializer.h"
#include "../Core/EngineCore.h"

namespace Improbability
{

MakeMeta(PlatformDropAction);

void PlatformDropAction::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAxisChange);
}

void PlatformDropAction::Start()
{
  m_axis = g_engine_core->SystemCall("TileSystem", "GetWorldAxis");
}

void PlatformDropAction::Initialize()
{
  g_engine_core->AddEventListener(MessageBank::OnAxisChange, this);
}

ActionReturn::Type PlatformDropAction::Update(float dt)
{
  if(m_axis == 1)
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
  }
  else
  {
    g_engine_core->DispatchEvent("OnPlayerDrop");
  }
  return ActionReturn::Continue;
}

ActionReturn::Type PlatformDropAction::InactiveUpdate( float dt )
{
  return ActionReturn::Continue;
}

void PlatformDropAction::Uninitialize()
{
  g_engine_core->RemoveEventListener(MessageBank::OnAxisChange, this);
}

void PlatformDropAction::Deserialize(Serializer &serializer)
{
  Read(serializer, m_vector, "Speed");
  Read(serializer, m_ignore_zeroes, "IgnoreZeroes");
}

void PlatformDropAction::OnAxisChange(int old_axis, int new_axis)
{
  m_axis = new_axis;
}
}
