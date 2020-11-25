// David Evans
// 2011/09/09
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "DynamicComponent.h"
#include "../Core/EngineCore.h"
#include "../Math/Vector3.h"
#include "../Composition/Composition.h"
#include "../FileIO/Serializer.h"
#include "../Graphics/DebugDraw.h"

namespace Improbability
{

MakeMeta(DynamicComponent);

void DynamicComponent::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetInverseMass);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetVelocity);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAxisChange);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAxisChangeComplete);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(DelayedJump);
  comp->MetaAddComponent(MetaFieldable);
  comp->MetaGetComponent(MetaFieldable)->MetaRegisterField(float, m_invmass);
}

DynamicComponent::DynamicComponent():
  m_invmass(1.0f),
  m_restitution(1.0f),
  m_gravity(0.0f, 0.0f, 0.0f),
  m_velocity(0.0f, 0.0f, 0.0f),
  m_jump_velocity(0),
  m_acceleration_vector(0.0f, 0.0f, 0.0f),
  m_acceleration(0.0f),
  m_angular_velocity(0.0f, 0.0f, 0.0f),
  m_horizontal_velocity(0,0,0),
  m_vertical_velocity(0),
  m_rotating(false)
{
}

void DynamicComponent::Initialize()
{
  g_engine_core->AddEventListener(MessageBank::OnAxisChange, this);
  g_engine_core->AddEventListener(MessageBank::OnAxisChangeComplete, this);
}

void DynamicComponent::Uninitialize()
{
  g_engine_core->RemoveEventListener(MessageBank::OnAxisChange, this);
  g_engine_core->RemoveEventListener(MessageBank::OnAxisChangeComplete, this);
}

Component* DynamicComponent::Clone()
{
  Component *clone = new DynamicComponent(*this);
  return clone;
}

void DynamicComponent::Deserialize(Serializer &serializer)
{
  float mass = 1.0f;
  Read(serializer, mass, "Mass");
  Read(serializer, m_restitution, "Restitution");
  Read(serializer, m_friction, "Friction");
  Read(serializer, m_acceleration, "Acceleration");

  ErrorIf(mass <= 0.0f, "DynamicComponent: Cannot have mass of %f", mass);
  m_invmass = 1.0f / mass;
}

void DynamicComponent::AddImpulse(Vector3 vector)
{
  AddVelocity(vector * m_invmass);
}

void DynamicComponent::ResolveCollision(Vector3 normal, bool friction)
{
  float res_scale = 1.0f;

  // If the velocity is very low, we should
  // ignore the effects of gravity and scale 
  // the restitution
  // This doesn't seem to be necessary
  if (GetVelocity().Magnitude() < 0.6f && normal.m_y != 0.0f)
  {
    // AddImpulse(-m_gravity);
    // res_scale = 0.01f;
  }

  // Apply friction perpendicular to the normal (this is so fake)
  if(friction)
  {
    Vector3 friction = -GetVelocity();
    friction.m_y = 0.0f;
    if (normal.m_x == 0.0f && normal.m_z == 0.0f)
    {
      AddImpulse(friction * m_friction);
    }
  }

  // Get relative and contact velocity
  Vector3 relative_v = -GetVelocity();
  float contact_v = relative_v.Dot(normal);
  if (contact_v > 0)
    return;

  // Calculate impulse
  float jnormal = -(1 + m_restitution * res_scale) * contact_v;
  jnormal /= m_invmass;
  AddImpulse(normal * -jnormal);
}

void DynamicComponent::DebugDraw()
{
  Vector3 pos = GetParent()->GetPosition().ToVector3();
  DebugDraw::AddArrow(pos, m_velocity, m_velocity.Magnitude());
}

void DynamicComponent::AddVelocity( Vector3 velocity )
{
  if(velocity.m_y < 0 && m_velocity.m_y >= 0)
    GetParent()->Invoke("OnFallStart"); 

  //Notify siblings if moving from Horizontal Standstill
  if((std::abs(m_velocity.m_x) < k_epsilon && std::abs(m_velocity.m_z) < k_epsilon) && (std::abs(velocity.m_x) > k_epsilon || std::abs(velocity.m_z) > k_epsilon))
    GetParent()->Invoke("OnHorizMoveStart"); 

  //Apply Velocity
  Vector3 TempVel = m_velocity;
  m_velocity += velocity;

  if((std::abs(m_velocity.m_x) < 0.01f && std::abs(m_velocity.m_z) < 0.01f) && (std::abs(TempVel.m_x) > 0.01f || std::abs(TempVel.m_z) > 0.01f))
    GetParent()->Invoke("OnHorizMoveFinish"); 
}

void DynamicComponent::SetVelocity( float vx, float vy, float vz )
{
  //Set velocity
  SetVelocity(Vector3(vx, vy, vz));
}

void DynamicComponent::SetVelocity( Vector3 velocity )
{
  if(velocity.m_y < 0 && m_velocity.m_y >= 0)
    GetParent()->Invoke("OnFallStart"); 

  //Notify siblings if moving from Horizontal Standstill
  if((std::abs(m_velocity.m_x) < k_epsilon && std::abs(m_velocity.m_z) < k_epsilon) && (std::abs(velocity.m_x) > k_epsilon || std::abs(velocity.m_z) > k_epsilon))
    GetParent()->Invoke("OnHorizMoveStart"); 

  //Set Velocity
  Vector3 TempVel = m_velocity;
  m_velocity = velocity;
  
  if((std::abs(m_velocity.m_x) < 0.01f && std::abs(m_velocity.m_z) < 0.01f) && (std::abs(TempVel.m_x) > 0.01f || std::abs(TempVel.m_z) > 0.01f))
    GetParent()->Invoke("OnHorizMoveFinish"); 
}

void DynamicComponent::AccelerateVelocity( Vector3 acceleration )
{
  //Set Acceleration Vector
  m_acceleration_vector = acceleration;
}

void DynamicComponent::SetHorizontalVelocity( Vector3 velocity )
{
  m_horizontal_velocity = velocity;
  m_horizontal_velocity.m_y = 0.f;
}

void DynamicComponent::SetLinearVelocity( Vector3 velocity )
{
  m_horizontal_velocity.m_x = velocity.m_x;
  m_horizontal_velocity.m_z = velocity.m_z;
  m_horizontal_velocity.m_y = 0.f;
  m_vertical_velocity = velocity.m_y;
}

void DynamicComponent::AddHorizontalVelocity( Vector3 velocity )
{
  m_horizontal_velocity += velocity;
  m_horizontal_velocity.m_y = 0.f;
  m_horizontal_velocity.Normalize();
  m_horizontal_velocity *= abs(m_max_horizontal);
}

void DynamicComponent::OnAxisChange( int old_axis, int new_axis )
{
  m_rotating = true;
}

void DynamicComponent::OnAxisChangeComplete( int axis )
{
  m_rotating = false;
}

void DynamicComponent::DelayedJump(float power)
{
  m_jump_power = power;
  m_jump_delayed = true;
}

}