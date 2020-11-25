// David Evans
// 2011/10/02
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Logic/Action.h"
#include "../Math/Vector3.h"

namespace Improbability
{

class Serializer;
class MetaComposition;

class AxisCameraFollow : public Action
{
public:
  static void BindMeta(MetaComposition*);

  AxisCameraFollow();

  // Inherited methods
  void Initialize();
  void Start();
  ActionReturn::Type Update(float dt);
  void Uninitialize();
  void Deserialize(Serializer &serializer);
  Action* Clone() {return new AxisCameraFollow(*this);}

  // Messages
  void OnAxisChange(int old_axis, int new_axis);
  void OnRotationFinish();

  // Methods
  void SetDistance(float distance) {m_distance = distance;}
  void SetObjectName(String name) {m_object_name = name;}
  void AxisChangeRotateCamera(float percentage, float dt, float initial_duration);
  int GetTransitionAxis();
  void OnPlayerNotGrounded();
  void OnPlayerGrounded();
  void OnHelpActivated();
private:
  float m_distance;
  int m_axis;
  int m_prev_axis;
  String m_object_name;
  int m_rotate_animation_ID;
  float m_rotation_progress;
  float m_rotation_duration;
  bool m_grounded;
  float m_rotation_count;
};

}