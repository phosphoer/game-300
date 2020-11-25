// Nicholas Baldwin
// 2011/09/19
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Camera.h"
#include "../FileIO/Serializer.h"
#include "GraphicsDevice.h"
#include "../core/EngineCore.h"
#include "../composition/composition.h"
#include "../Meta/PropertyAnimator.h"

namespace Improbability
{

MakeMeta(Camera);

void Camera::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(EmitterReady);
}

void Camera::Initialize()
{
  FixPerspective();
}

Component* Camera::Clone()
{
  return new Camera(*this);
}

void Camera::Deserialize(Serializer &serializer)
{
  Read(serializer, m_target, "Target");
  Read(serializer, m_up, "Up");
  m_up.Normalize();

  Read(serializer, m_near, "Near");
  Read(serializer, m_far, "Far");
  Read(serializer, m_focal, "Focal");

  Read(serializer, m_fov, "FOV");

  String projection;

  Read(serializer, projection, "Projection");

  if (projection == "Perspective")
  {
    m_how_orthographic = 0.0f;
  }
  else if (projection == "Orthographic")
  {
    m_how_orthographic = 1.0f;
  }
  else
  {
    CriticalErrorIf(true, 
      "attempting to deserialize invalid camera, Projection can either be Perspective or Orthographic");
  }
}

void Camera::SetTarget(Vector3 new_target)
{
  m_target = new_target;
}

void Camera::SetUpVector(Vector3 new_up)
{
  m_up = new_up;
}

void Camera::SetProjectionValues(float new_width, float new_height, float new_near, float new_far, float new_focal)
{
  m_width = new_width;
  m_height = new_height;
  m_near = new_near;
  m_far = new_far;
  m_focal = new_focal;
}

void Camera::Zoom(float zoom_factor)
{
  Vector3 view = (m_target - GetParent()->GetPosition().ToVector3()).Normalized();

  GetParent()->SetPosition(GetParent()->GetPosition() + Vector4::Point(view * zoom_factor));
}

void Camera::Pan(Vector3 change_in_position)
{
  GetParent()->SetPosition(GetParent()->GetPosition() + Vector4::Point(change_in_position));
  m_target += change_in_position;
}

void Camera::Orbit(Vector3 axis, float angle)
{
  Matrix3 rotation = Matrix3::CreateRotation(axis, angle);
  Vector3 eye = GetParent()->GetPosition().ToVector3();
  eye -= m_target;
  eye = rotation * eye;
  eye += m_target;
  GetParent()->SetPosition(eye);
}

void Camera::FixPerspective()
{
  m_width = (float)GraphicsDevice::GetDeviceWidth() / 20.0f;
  m_height = (float)GraphicsDevice::GetDeviceHeight() / 20.0f;
}

void Camera::UsePerspective()
{
  m_how_orthographic = 0.0f;
}

void Camera::UseOrthographic()
{
  m_how_orthographic = 1.0f;
}

bool Camera::InterpolateProjection(float dt)
{
  if (m_how_orthographic + dt > 0.9999f)
  {
    m_how_orthographic = 1.0f;
    return true;
  }
  else if (m_how_orthographic + dt < 0.0001f)
  {
    m_how_orthographic = 0.0f;
    return true;
  }

  m_how_orthographic += dt;
  return false;
}

void Camera::Set()
{
  MatrixStack<Matrix4> &view = GraphicsDevice::GetViewStack();
  MatrixStack<Matrix4> &projection = GraphicsDevice::GetProjectionStack();

  view.Push();
  projection.Push();

  view.LoadMatrix(Matrix4::CreateLookAt(Vector4::Point(GetParent()->GetPosition().ToVector3()), 
                                        Vector4::Point(m_target), 
                                        Vector4::Vector(m_up)));

  if (m_how_orthographic > 0.9999f)
  {
    float scaling_factor = (m_target - GetParent()->GetPosition().ToVector3()).Magnitude() / 200.f;
    projection.LoadMatrix(Matrix4::CreateOrthographicProjection(1.0f / tan(DegreeToRadian(m_fov)), 
                                                                m_height / m_width, 
                                                                m_near, m_far, scaling_factor));
  }
  else if (m_how_orthographic < 0.0001f)
  {
    projection.LoadMatrix(Matrix4::CreatePerspectiveProjection(1.0f / tan(DegreeToRadian(m_fov)), 
                                                               m_height / m_width, 
                                                               m_near, m_far));
  }
  else
  {
    Matrix4 pers = Matrix4::CreatePerspectiveProjection(1.0f / tan(DegreeToRadian(m_fov)), m_height / m_width, m_near, m_far);
    float scaling_factor = (m_target - GetParent()->GetPosition().ToVector3()).Magnitude() / 200.f;
    Matrix4 ortho = Matrix4::CreateOrthographicProjection(1.0f / tan(DegreeToRadian(m_fov)), m_height / m_width, m_near, m_far, scaling_factor);

    float t = m_how_orthographic;
    ;
    Matrix4 proj;
    proj.m_00 = ortho.m_00 + (pers.m_00 - ortho.m_00) * t; proj.m_01 = ortho.m_01 + (pers.m_01 - ortho.m_01) * t; proj.m_02 = ortho.m_02 + (pers.m_02 - ortho.m_02) * t; proj.m_03 = ortho.m_03 + (pers.m_03 - ortho.m_03) * t;
    proj.m_10 = ortho.m_10 + (pers.m_10 - ortho.m_10) * t; proj.m_11 = ortho.m_11 + (pers.m_11 - ortho.m_11) * t; proj.m_12 = ortho.m_12 + (pers.m_12 - ortho.m_12) * t; proj.m_13 = ortho.m_13 + (pers.m_13 - ortho.m_13) * t;
    proj.m_20 = ortho.m_20 + (pers.m_20 - ortho.m_20) * t; proj.m_21 = ortho.m_21 + (pers.m_21 - ortho.m_21) * t; proj.m_22 = ortho.m_22 + (pers.m_22 - ortho.m_22) * t; proj.m_23 = ortho.m_23 + (pers.m_23 - ortho.m_23) * t;
    proj.m_30 = ortho.m_30 + (pers.m_30 - ortho.m_30) * t; proj.m_31 = ortho.m_31 + (pers.m_31 - ortho.m_31) * t; proj.m_32 = ortho.m_32 + (pers.m_32 - ortho.m_32) * t; proj.m_33 = ortho.m_33 + (pers.m_33 - ortho.m_33) * t;
    
    projection.LoadMatrix(proj);
  }
}

void Camera::Unset()
{
  MatrixStack<Matrix4> &view = GraphicsDevice::GetViewStack();
  MatrixStack<Matrix4> &projection = GraphicsDevice::GetProjectionStack();

  view.Pop();
  projection.Pop();
}

Vector3 Camera::GetRight()
{
  Vector3 eye = GetParent()->GetPosition().ToVector3();
  Vector3 view = m_target - eye;
  return (view.Cross(m_up)).Normalized();
}

void Camera::EmitterReady()
{
  //GetParent()->Invoke("PlaySound");
}

void Camera::SetNearPlane(int distance)
{
  m_near = (float)distance;
}

}