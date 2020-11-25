// Aaron Biddlecom
// 2011/12/9
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "CameraElevate.h"
#include "../Logic/LogicComponent.h"
#include "../Logic/AnimatePropertyAction.h"
#include "../Graphics/Camera.h"
#include "../Core/EngineCore.h"
#include "../Composition/Composition.h"
#include "../Physics/DynamicComponent.h"
#include "../FileIO/Serializer.h"
#include "../Math/Vector4.h"

namespace Improbability
{

MakeMeta(CameraElevate);

void CameraElevate::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(AnimationEnd);
}

CameraElevate::CameraElevate():
  m_distance(20.0f),
  m_speed(5.0f),
  m_animation_id(0),
  m_finished(false)
{
}

void CameraElevate::Initialize()
{
}

void CameraElevate::Start()
{
  Composition *camera_obj = GetParent();
  float startPos = camera_obj->GetPosition().m_y;
  float endPos = startPos + m_distance;
  m_animation_id = PropertyAnimator::AnimateField
  (
    "m_position.m_y",
    *camera_obj,
    startPos,
    endPos,
    m_distance / m_speed,
    this,
    "AnimationEnd"
  );
}

ActionReturn::Type CameraElevate::Update(float dt)
{
  if(m_finished)
    return ActionReturn::Remove;
  else
    return ActionReturn::Block;
}

void CameraElevate::Uninitialize()
{
}

void CameraElevate::Deserialize(Serializer &serializer)
{
  Read(serializer, m_distance, "Distance");
  Read(serializer, m_speed, "Speed");
}

void CameraElevate::AnimationEnd()
{
  m_finished = true;
}
}