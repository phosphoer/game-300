// Nicholas Baldwin
// 2012/02/05
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "..\Logic\Action.h"

namespace Improbability
{

class Serializer;
class MetaComposition;

class AxisCamera : public Action
{
public:
  static void BindMeta(MetaComposition*);

  AxisCamera();

  // Inherited methods
  void Initialize();
  void Start();
  ActionReturn::Type Update(float dt);
  void Uninitialize();
  void Deserialize(Serializer &serializer);
  Action* Clone() {return new AxisCamera(*this);}

  // Messages
  void OnAxisChange(int old_axis, int new_axis);
  void OnRotationFinish();

  // Methods
  void AxisChangeRotateCamera(float percentage, float dt, float initial_duration);
  int GetTransitionAxis();
  void OnHelpActivated();

private:
  int m_axis;
  int m_prev_axis;
  int m_rotate_animation_ID;
  float m_rotation_progress;
  float m_rotation_duration;
  float m_rotation_count;
};

}