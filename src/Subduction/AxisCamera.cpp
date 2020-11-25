// Nicholas Baldwin
// 2012/02/12
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "AxisCamera.h"
#include "..\Core\EngineCore.h"
#include "..\math\Vector4.h"
#include "..\Composition\Composition.h"
#include "..\Graphics\Camera.h"
#include "..\Logic\LogicComponent.h"

namespace Improbability
{

MakeMeta(AxisCamera);

void AxisCamera::BindMeta(MetaComposition* comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAxisChange);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnHelpActivated);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(AxisChangeRotateCamera);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnRotationFinish);
}

AxisCamera::AxisCamera() :
  m_axis(0),
  m_prev_axis(0),
  m_rotate_animation_ID(NULL),
  m_rotation_duration(0.0f),
  m_rotation_progress(0.f),
  m_rotation_count(.75f)
{

}

void AxisCamera::Initialize()
{
  g_engine_core->AddEventListener(MessageBank::OnAxisChange, this);
}

void AxisCamera::Start()
{
  m_axis = g_engine_core->SystemCall("TileSystem", "GetWorldAxis");
  m_prev_axis = m_axis;

  switch (m_axis)
  {
  case 0:
    GetParent()->GetComponent<Camera>()->SetUpVector(Vector3(0,1,0));
    break;
  case 1:
    GetParent()->GetComponent<Camera>()->SetUpVector(Vector3(0,0,-1));
    break;
  case 2:
    GetParent()->GetComponent<Camera>()->SetUpVector(Vector3(0,1,0));
    break;
  }
}

ActionReturn::Type AxisCamera::Update(float dt)
{
  Composition *camera_obj = GetParent();

  Vector4 pos = camera_obj->GetPosition();
  Vector3 axis(0, 0, 0);
  axis.m_a[m_axis] = 1.0f;

  pos = Vector4::Point(axis) * 17.f;
  camera_obj->SetPosition(pos);

  camera_obj->GetComponent<Camera>()->SetTarget(Vector3(0.0f, 0.0f, 0.0f));

  return ActionReturn::Continue;
}

void AxisCamera::Uninitialize()
{
  g_engine_core->RemoveEventListener(MessageBank::OnAxisChange, this);
}

void AxisCamera::Deserialize(Serializer &serializer)
{

}

void AxisCamera::OnAxisChange(int old_axis, int new_axis)
{
  m_axis = new_axis;
  m_prev_axis = old_axis;
  if(new_axis == old_axis)
    return;
  float rotationDuration = m_rotation_count;
  m_rotation_duration = k_half_pi;

  // if you are already in the middle of a rotation
  if(m_rotate_animation_ID != NULL)
  {
    // stop the rotation
    PropertyAnimator::RemoveAnimation(m_rotate_animation_ID);
    // find the rotation duration
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

void AxisCamera::OnRotationFinish()
{
  SetPaused(false);
  m_rotate_animation_ID = NULL;
  m_rotation_progress = 0;
  g_engine_core->DispatchEvent(MessageBank::OnAxisChangeComplete, m_axis);
}

void AxisCamera::AxisChangeRotateCamera(float percentage, float dt, float initial_duration)
{
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

void AxisCamera::OnHelpActivated()
{
  m_rotation_count *= .75f;
}

}