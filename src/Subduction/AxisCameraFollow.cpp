// David Evans
// 2011/09/18
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "AxisCameraFollow.h"
#include "../Logic/LogicComponent.h"
#include "../Logic/AnimatePropertyAction.h"
#include "../Graphics/Camera.h"
#include "../Core/EngineCore.h"
#include "../Composition/Composition.h"
#include "../Physics/DynamicComponent.h"
#include "../FileIO/Serializer.h"
#include "../Math/Vector4.h"
#include "CameraElevate.h"

namespace Improbability
{

MakeMeta(AxisCameraFollow);

void AxisCameraFollow::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAxisChange);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(AxisChangeRotateCamera);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnRotationFinish);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnPlayerGrounded);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnPlayerNotGrounded);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnHelpActivated);
}

AxisCameraFollow::AxisCameraFollow():
  m_distance(2.0f),
  m_axis(0),
  m_prev_axis(0),
  m_rotate_animation_ID(NULL),
  m_rotation_duration(0.0f),
  m_rotation_count(1.0f),
  m_grounded(true)
  {
}

void AxisCameraFollow::Initialize()
{
  g_engine_core->AddEventListener(MessageBank::OnAxisChange, this);
  g_engine_core->AddEventListener(MessageBank::OnPlayerGrounded, this);
  g_engine_core->AddEventListener(MessageBank::OnPlayerNotGrounded, this);
}

void AxisCameraFollow::Start()
{
  m_axis = g_engine_core->SystemCall("TileSystem", "GetWorldAxis");
  m_prev_axis = m_axis;
}

ActionReturn::Type AxisCameraFollow::Update(float dt)
{
  Composition *camera_obj = GetParent();
  Composition *object = g_engine_core->SystemCall("ObjectManager", "GetObjectByName", m_object_name);

  if (!object)
    return ActionReturn::Continue;

  Vector4 pos = camera_obj->GetPosition();
  Vector4 obj_pos = object->GetPosition();
  Vector3 axis(0, 0, 0);
  axis.m_a[m_axis] = 1.0f;

  pos = obj_pos + axis * m_distance;
  camera_obj->SetPosition(pos);

  camera_obj->GetComponent<Camera>()->SetTarget(obj_pos.ToVector3());

  return ActionReturn::Continue;
}

void AxisCameraFollow::Uninitialize()
{
  g_engine_core->RemoveEventListener(MessageBank::OnAxisChange, this);
  g_engine_core->RemoveEventListener(MessageBank::OnPlayerGrounded, this);
  g_engine_core->RemoveEventListener(MessageBank::OnPlayerNotGrounded, this);
}

void AxisCameraFollow::Deserialize(Serializer &serializer)
{
  Read(serializer, m_distance, "Distance");
  Read(serializer, m_object_name, "ObjectName");
}

void AxisCameraFollow::OnRotationFinish()
{
  SetPaused(false);
  m_rotate_animation_ID = NULL;
  m_rotation_progress = 0;
  g_engine_core->DispatchEvent(MessageBank::OnAxisChangeComplete, m_axis);
}

void AxisCameraFollow::OnAxisChange(int old_axis, int new_axis)
{
  m_axis = new_axis;
  m_prev_axis = old_axis;
  if(new_axis == old_axis)
    return;
  float rotationDuration = m_rotation_count;
  m_rotation_duration = k_half_pi;

  if(m_rotate_animation_ID != NULL)
  {
    PropertyAnimator::RemoveAnimation(m_rotate_animation_ID);
    rotationDuration *= m_rotation_progress / k_half_pi;
    m_rotation_duration = m_rotation_progress;
    m_rotation_progress = k_half_pi - m_rotation_progress;
  }
  else
  {
    Composition *player = g_engine_core->SystemCall("ObjectManager", "GetObjectByName", "Player");
    LogicComponent *logic = player->GetComponent<LogicComponent>();
    logic->PushFront(ActionBank::PlayerFreezeAction,0.0f);
  }
  SetPaused(true);
  m_rotate_animation_ID = PropertyAnimator::AddProgressCallback(rotationDuration, this, "AxisChangeRotateCamera", "OnRotationFinish");

  // Set up vector
  if (new_axis == 1)
    GetParent()->GetComponent<Camera>()->SetUpVector(Vector3(0.0f, 0.0f, -1.0f));
  else
    GetParent()->GetComponent<Camera>()->SetUpVector(Vector3(0.0f, 1.0f, 0.0f));
}

void AxisCameraFollow::AxisChangeRotateCamera(float percentage, float dt, float initial_duration)
{
  if (!m_grounded)
  {
    Composition *camera_obj = GetParent();
    Composition *object = g_engine_core->SystemCall("ObjectManager", "GetObjectByName", m_object_name);

    if (!object)
      return;

    DynamicComponent *dyn = object->GetComponent<DynamicComponent>();
    Camera *camera_comp = GetParent()->GetComponent<Camera>();

    Vector4 pos = camera_obj->GetPosition();
    Vector4 player_pos = object->GetPosition();
    float y_offset = dyn->GetVelocity().m_y * dt;
    float x_offset = dyn->GetVelocity().m_x * dt;
    camera_comp->SetTarget(Vector3(player_pos.m_x, player_pos.m_y, player_pos.m_z));
    pos.m_y += y_offset;
    pos.m_x += x_offset;
    camera_obj->SetPosition(pos);
  }

  float angle = (dt / initial_duration) * m_rotation_duration;
  m_rotation_progress += angle;
  if (m_rotation_progress > k_half_pi)
    angle -= m_rotation_progress - k_half_pi;
  switch(m_axis)
  {
  case 0:  // Rotating to Side
    GetParent()->GetComponent<Camera>()->Orbit(Vector3(0,1,0), angle);
    break;
  case 1:  // Rotating to Top
    GetParent()->GetComponent<Camera>()->Orbit(Vector3(1,0,0), -angle);
    break;
  case 2:  // Rotating to Front
    if(m_prev_axis == 0) // Rotating from Side
      GetParent()->GetComponent<Camera>()->Orbit(Vector3(0,1,0), -angle);
    else  // Rotating from Top
      GetParent()->GetComponent<Camera>()->Orbit(Vector3(1,0,0), angle);
    break;
  }
}

void AxisCameraFollow::OnPlayerGrounded()
{
  m_grounded = true;
}
void AxisCameraFollow::OnPlayerNotGrounded()
{
  m_grounded = false;
}
void AxisCameraFollow::OnHelpActivated()
{
  m_rotation_count *= .75f;
}
}