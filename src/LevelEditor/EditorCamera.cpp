// David Evans
// 2011/07/23
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "EditorCamera.h"
#include "../Composition/Composition.h"
#include "../Physics/DynamicComponent.h"
#include "../Graphics/Camera.h"
#include "../Math/Vector4.h"
#include "../Math/Matrix4.h"
#include "../FileIO/Serializer.h"
#include "../Core/EngineCore.h"

namespace Improbability
{

MakeMeta(EditorCamera);

void EditorCamera::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnMouseDown);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnMouseScroll);
}

EditorCamera::EditorCamera()
{
}

void EditorCamera::Initialize()
{
  g_engine_core->AddEventListener(MessageBank::OnMouseDown, this);
  g_engine_core->AddEventListener(MessageBank::OnMouseScroll, this);
}

void EditorCamera::Uninitialize()
{
  g_engine_core->RemoveEventListener(MessageBank::OnMouseDown, this);
  g_engine_core->RemoveEventListener(MessageBank::OnMouseScroll, this);
}

Component* EditorCamera::Clone()
{
  EditorCamera *clone = new EditorCamera(*this);
  return clone;
}

void EditorCamera::Deserialize(Serializer &serializer)
{
}

void EditorCamera::OnMouseDown(MouseEvent event)
{
  // Get the camera
  Camera *camera = GetSibling<Camera>();
  if (!camera)
    return;

  // Camera pan controls
  if (event.m_buttoncode == kMouseMiddleButton && event.m_keystates[kKeyShift])
    Pan(event.m_x_move * 0.3f, event.m_y_move * 0.3f);

  // Camera orbit controls
  else if (event.m_buttoncode == kMouseMiddleButton)
    Orbit(event.m_x_move * 1.0f, event.m_y_move * 1.0f);
}

void EditorCamera::OnMouseScroll(MouseEvent event)
{
  Camera *camera = GetSibling<Camera>();
  if (!camera)
    return;
    
  // Zoom camera
  camera->Zoom(event.m_scroll_move * 0.005f);
}

void EditorCamera::Pan(float x, float y)
{
  Camera *camera = GetSibling<Camera>();
  if (!camera)
    return;

  Vector4 mouse_move(-0.05f * x, 0.05f * y, 0.0f, 0.0f);
  Vector3 target = camera->GetTarget();
  Vector3 up = camera->GetUp();
  Vector3 view = (target - GetParent()->GetPosition().ToVector3()).Normalized();
  Matrix4 move_transform = Matrix4::CreateInverseOrthogonal(Vector4::Vector(view.Cross(up)), 
                                                            Vector4::Vector(up), 
                                                            Vector4::Vector(view));
  move_transform = Matrix4::CreateTranslation(GetParent()->GetPosition()) * move_transform;
  mouse_move = move_transform * mouse_move;
  camera->Pan(mouse_move.ToVector3());
}

void EditorCamera::Orbit(float x, float y)
{
  Camera *camera = GetSibling<Camera>();
  if (!camera)
    return;

  float dx = -0.005f * x;
  float dy = -0.005f * y;

  camera->Orbit(Vector3(0.0f, 1.0f, 0.0f), dx);
  camera->Orbit(camera->GetRight(), dy);
}

}