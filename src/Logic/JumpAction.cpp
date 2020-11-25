// David Evans
// 2011/09/22
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "JumpAction.h"
#include "../Composition/Composition.h"
#include "../Physics/DynamicComponent.h"
#include "../Tile/TileComponent.h"
#include "../FileIO/Serializer.h"
#include "../Core/EngineCore.h"

namespace Improbability
{

MakeMeta(JumpAction);

void JumpAction::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnPlayerHeadCollision);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnRotateStart);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnRotateComplete);
}

ActionReturn::Type JumpAction::Update(float dt)
{
  if(m_rotating)
    return ActionReturn::Continue;
  Composition *obj = GetParent();
  DynamicComponent *dyn = obj->GetComponent<DynamicComponent>();
  TileComponent *tile = obj->GetComponent<TileComponent>();
  Vector3 vel = dyn->GetVelocity();

  //if(dt > .016f)
  //  dt = .016f;
  if (!tile || (tile->IsGrounded() && !m_count) || (vel.m_y > 0 && m_count < m_currMax_duration))
  {
    if (dyn)
    {
      int axis = tile->GetAxis();
      if(axis != 1)
      {
        if(tile->IsGrounded())
        {
          //m_currMax_duration = abs(vel.m_x) < 1.f && abs(vel.m_z) < 1.f ? m_max_duration * .5f : m_max_duration;
          m_currMax_duration = m_max_duration;
        }
        if(!m_jumping && m_count > .03f)
        {
          m_jumping = true;
          g_engine_core->DispatchEvent("PushAudioEvent", "Play_jump");
        }
        m_count += dt;
        dyn->SetVelocity(vel.m_x, m_power, vel.m_z);
        float sign = !axis ? (vel.m_x > 0.0f ? 1.0f : -1.0f) : (vel.m_z > 0.0f ? 1.0f : -1.0f);
        dyn->SetJumpDirection(sign);
        dyn->SetJump(true);
      }
    }
  }
  else
  {
    m_jumping = false;
    dyn->SetJump(false);
  }

  return ActionReturn::Continue;
}

ActionReturn::Type JumpAction::InactiveUpdate( float dt )
{
  if(m_rotating)
    return ActionReturn::Continue;
  Composition *obj = GetParent();
  DynamicComponent *dyn = obj->GetComponent<DynamicComponent>();
  TileComponent *tile = obj->GetComponent<TileComponent>();
  Vector3 vel = dyn->GetVelocity();

  if(dt > .016f)
    dt = .016f;
  if (tile && vel.m_y > 0 && m_count < m_min_duration && m_count)
  {
    if (dyn)
    {
      int axis = tile->GetAxis();
      if(axis != 1)
      {
        m_count += dt;
        dyn->SetVelocity(vel.m_x, m_power, vel.m_z);
        float sign = !axis ? (vel.m_x > 0.0f ? 1.0f : -1.0f) : (vel.m_z > 0.0f ? 1.0f : -1.0f);
        dyn->SetJumpDirection(sign);
      }
    }
  }

  else if(tile && dyn)
  {
    m_jumping = false;
    dyn->SetJump(false);
    if(!tile->IsGrounded())
      m_count = m_max_duration;
    else
      m_count = 0;
  }
  return ActionReturn::Continue;
}

void JumpAction::Deserialize(Serializer &serializer)
{
  Read(serializer, m_power, "Power");
  Read(serializer, m_min_duration, "MinDuration");
  Read(serializer, m_max_duration, "MaxDuration");
  Read(serializer, m_currMax_duration, "MaxDuration");
}

void JumpAction::Initialize()
{
  g_engine_core->AddEventListener(MessageBank::OnPlayerHeadCollision, this);
  g_engine_core->AddEventListener(MessageBank::OnRotateStart, this);
  g_engine_core->AddEventListener(MessageBank::OnRotateComplete, this);
}
void JumpAction::Uninitialize()
{
  g_engine_core->RemoveEventListener(MessageBank::OnPlayerHeadCollision, this);
  g_engine_core->RemoveEventListener(MessageBank::OnRotateStart, this);
  g_engine_core->RemoveEventListener(MessageBank::OnRotateComplete, this);
}
void JumpAction::OnPlayerHeadCollision()
{
  m_count = m_max_duration;
}

void JumpAction::OnRotateStart(int start_axis, int end_axis)
{
  m_rotating = true;
}
void JumpAction::OnRotateComplete(int start_axis, int end_axis)
{
  m_rotating = false;
}
}
