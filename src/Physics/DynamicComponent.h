// David Evans
// 2011/09/09
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Composition/Component.h"
#include "../Math/Vector3.h"

namespace Improbability
{

class Serializer;
class Physics;
class MetaComposition;

class DynamicComponent : public Component
{
  friend class Physics;

public:
  static void BindMeta(MetaComposition*);

  // Inherited methods
  DynamicComponent();
  void Initialize();
  void Uninitialize();
  Component* Clone();
  void Serialize(Serializer &serializer) { }
  void Deserialize(Serializer &serializer);

  //Msgs
  void OnAxisChange(int old_axis, int new_axis);
  void OnAxisChangeComplete(int axis);

  // Methods
  void AddImpulse(Vector3 vector);
  void ResolveCollision(Vector3 normal, bool friction = true);
  void DebugDraw();
  void SetVelocity(Vector3 velocity);
  void SetVelocity(float vx, float vy, float vz);
  void SetJumpVelocity(float magnitude){m_jump_velocity = magnitude;}
  void AccelerateVelocity( Vector3 acceleration );
  void SetAccelerationVector(Vector3 acceleration) {m_acceleration_vector = acceleration;}
  void AddVelocity(Vector3 velocity);
  void SetAngularVelocity(Vector3 velocity) {m_angular_velocity = velocity;}
  void SetLinearVelocity(Vector3 velocity);
  void SetHorizontalVelocity(Vector3 velocity);
  void AddHorizontalVelocity(Vector3 velocity);
  void AddHorizontalVelocityMax(Vector3 velocity);
  void SetVerticalVelocity(float velocity) {m_vertical_velocity = velocity;}
  void KillHorizontalVelocity(int axis){if(axis)m_velocity.m_z = 0;else m_velocity.m_x = 0;}
  void SetMaxHorizontalVelocity(float max){m_max_horizontal = max;}
  void SetJumpDirection(float dir){m_jump_direction = dir;}
  void SetDelayedJump(bool jump){m_jump_delayed = jump;}
  void SetJump(bool jumping){m_jumping = jumping;}

  // Getters
  float GetInverseMass() {return m_invmass;}
  float GetRestitution() {return m_restitution;}
  float GetFriction() {return m_friction;}
  Vector3 GetVelocity() {return m_velocity;}
  float GetJumpVelocity(){return m_jump_velocity;}
  float GetAcceleration() {return m_acceleration;}
  Vector3 GetAccelerationVector() {return m_acceleration_vector;}
  Vector3 GetAngularVelocity() {return m_angular_velocity;}
  Vector3 GetHorizontalVelocity() {return m_horizontal_velocity;}
  float GetVerticalVelocity() {return m_vertical_velocity;}
  float GetMaxHorizontalVelocity(){return m_max_horizontal;}
  float GetJumpDirection(){return m_jump_direction;}
  bool GetDelayedJump(){return m_jump_delayed;}
  float GetJumpPower(){return m_jump_power;}
  bool IsJumping(){ return m_jumping; }

  // Messages
  void DelayedJump(float power);
private:
  float m_invmass;
  float m_restitution;
  float m_friction;

  Vector3 m_gravity;
  Vector3 m_velocity;
  Vector3 m_angular_velocity;
  Vector3 m_acceleration_vector;
  float m_acceleration;
  float m_jump_velocity;
  Vector3 m_horizontal_velocity;
  float m_vertical_velocity;
  float m_max_horizontal;
  float m_jump_direction;
  bool m_rotating;
  bool m_jump_delayed;
  float m_jump_power;
  bool m_jumping;
};

}