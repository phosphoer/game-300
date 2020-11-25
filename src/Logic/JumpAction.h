// David Evans
// 2011/09/22
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "Action.h"
#include "../Math/Vector3.h"

namespace Improbability
{

class Serializer;
class MetaComposition;

class JumpAction : public Action
{
public:
  static void BindMeta(MetaComposition*);

  // Inherited methods
  ActionReturn::Type Update(float dt);
  ActionReturn::Type InactiveUpdate(float dt);
  void Deserialize(Serializer &serializer);
  void Initialize();
  void Uninitialize();
  Action* Clone() {return new JumpAction(*this);}
  JumpAction():m_jumping(false), m_power(0.f), m_count(0.f), m_max_duration(.5f), m_min_duration(.1f), m_rotating(false){;}

  // Methods
  void SetPower(float power) {m_power = power;}
  void OnPlayerHeadCollision();
  void OnRotateStart(int start_axis, int end_axis);
  void OnRotateComplete(int start_axis, int end_axis);
private:
  float m_power;
  float m_count;
  float m_max_duration;
  float m_min_duration;
  float m_currMax_duration;
  bool m_rotating;
  bool m_jumping;
};

}