// Nicholas Baldwin
// 2012/02/05
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "CameraFollow.h"
#include "..\Core\EngineCore.h"
#include "..\Composition\Composition.h"
#include "..\Graphics\Camera.h"
#include "..\math\Vector4.h"
#include "..\FileIO\Serializer.h"

namespace Improbability
{

MakeMeta(CameraFollow);

void CameraFollow::BindMeta(MetaComposition* comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnPlayerGrounded);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnPlayerNotGrounded);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnKeyDown);
}

CameraFollow::CameraFollow():
  m_distance(5.0f),
  m_grounded(true),
  m_axis(0)
{

}

void CameraFollow::Initialize()
{
  g_engine_core->AddEventListener(MessageBank::OnPlayerGrounded, this);
  g_engine_core->AddEventListener(MessageBank::OnPlayerNotGrounded, this);
  g_engine_core->AddEventListener(MessageBank::OnKeyDown, this);
}

void CameraFollow::Start()
{

}

void CameraFollow::OnKeyDown(KeyboardEvent event)
{
  if (event.m_keycode == kKeyR)
    m_distance += 0.5f;
  if (event.m_keycode == kKeyF)
    m_distance -= 0.5f;

  if (m_distance < 4.0f)
    m_distance = 4.0f;
  if (m_distance > 20.0f)
    m_distance = 20.0f;
}

ActionReturn::Type CameraFollow::Update(float dt)
{
  Composition *camera_obj = GetParent();
  Composition *object = g_engine_core->SystemCall("ObjectManager", "GetObjectByName", m_object_name);

  if (!object)
    return ActionReturn::Continue;

  int distance = (int)m_distance;
  int anchor_axis = g_engine_core->SystemCall("TileSystem", "GetAnchorAxis");
  Vector4 obj_pos = object->GetPosition();
  //camera_obj->GetComponent<Camera>()->SetNearPlane(m_nearplane_default);

  //if(anchor_axis == m_axis)
  //{
  //  int anchor_front = g_engine_core->SystemCall("TileSystem", "GetAnchorBegin");
  //  int anchor_back = g_engine_core->SystemCall("TileSystem", "GetAnchorEnd");

  //  if(anchor_front > anchor_back)
  //    std::swap(anchor_front, anchor_back);

  //  if(obj_pos.m_a[anchor_axis] >= anchor_front && obj_pos.m_a[anchor_axis] <= anchor_back)
  //  {
  //    m_nearplane_default = camera_obj->GetComponent<Camera>()->GetNear();
  //    camera_obj->GetComponent<Camera>()->SetNearPlane(anchor_back - obj_pos.m_a[anchor_axis]);
  //  }

  //}

  Vector4 pos = camera_obj->GetPosition();
  Vector3 axis(0, 0, 0);
  axis.m_a[m_axis] = 1.0f;

  pos = obj_pos + axis * (float)distance;
  camera_obj->SetPosition(pos);

  camera_obj->GetComponent<Camera>()->SetTarget(obj_pos.ToVector3());

  return ActionReturn::Continue;
}

void CameraFollow::Uninitialize()
{
  g_engine_core->RemoveEventListener(MessageBank::OnPlayerGrounded, this);
  g_engine_core->RemoveEventListener(MessageBank::OnPlayerNotGrounded, this);
}

void CameraFollow::Deserialize(Serializer &serializer)
{
  Read(serializer, m_distance, "Distance");
  Read(serializer, m_object_name, "ObjectName");
  Read(serializer, m_axis, "Axis");
}

void CameraFollow::OnPlayerNotGrounded()
{
  m_grounded = false;
}

void CameraFollow::OnPlayerGrounded()
{
  m_grounded = true;
}

}