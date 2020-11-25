// Nicholas Baldwin
// 2011/09/17
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "../composition/component.h"
#include "../Input/InputEvent.h"

namespace Improbability
{

class MetaComposition;

class Camera : public Component
{
public:  
  // instantiation of base class stuff
  void Initialize();
  void Uninitialize() {}
  Component* Clone();

  void Serialize(Serializer &serializer) { }
  void Deserialize(Serializer &serializer);

  static void BindMeta(MetaComposition *comp);

public:  
  // camera functionality

  // manually set the values of the camera and projection
  void SetPosition(Vector3 new_position);
  void SetView(Vector3 new_view);
  void SetTarget(Vector3 new_target);
  void SetUpVector(Vector3 new_up);
  void SetProjectionValues(float new_width, float new_height, 
                           float new_near, float new_far, float new_focal);

  // moves the position of the camera towards the target, doesn't move the target
  // zoom_factor - the change in position to make towards the target
  void Zoom(float zoom_factor);

  // changes the position of the camera relative to the local axis
  // change_in_position - the change in position to occur relative to the local
  //                      axis this frame
  void Pan(Vector3 change_in_position);

  // changes the position and view of the camera such that it orbits about
  //   the target
  // axis - the axis to orbit about the target
  // angle - the angle to orbit about the target
  void Orbit(Vector3 axis, float angle);

  // fixes the perspective of the camera by getting width and height from the 
  // device
  void FixPerspective();

  // changes the projection matrix to use either perspective or orthographic
  // projection
  void UsePerspective();
  void UseOrthographic();
  void ToggleProjection() {m_how_orthographic = 1.0f - m_how_orthographic;}

  // attempts to interpolate the projection matrix by dt from perspective to 
  //   orthographic
  // dt - positive means the scale to change from perspective to orthographic
  //      this frame, negative is the change from orthographic to perspective
  // returns whether or not the attempt to scale the projection resulted in 
  //   either a full perspective/orthographic projection (true) or not (false).
  //   Essentially true means the interpolation is finished.
  bool InterpolateProjection(float dt);

  // grabs the model_view and projection matrices and applies the view and 
  // projection to them
  void Set();
  void Unset();

  Vector3 GetTarget() const {return m_target;}
  Vector3 GetUp() const {return m_up;}
  Vector3 GetRight();
  float GetNear() const {return m_near;}
  float GetFar() const {return m_far;}
  void EmitterReady();
  void SetNearPlane(int distance);
public:
  Vector3 m_target, m_up;

  float m_how_orthographic;

  float m_width, m_height, m_near, m_far, m_focal;
  float m_fov;
};

}