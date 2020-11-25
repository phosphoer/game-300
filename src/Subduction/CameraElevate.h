// Aaron Biddlecom
// 2011/12/9
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Logic/Action.h"
#include "../Math/Vector3.h"

namespace Improbability
{

class Serializer;
class MetaComposition;

class CameraElevate : public Action
{
public:
  static void BindMeta(MetaComposition*);

  CameraElevate();

  // Inherited methods
  void Initialize();
  void Start();
  ActionReturn::Type Update(float dt);
  void Uninitialize();
  void Deserialize(Serializer &serializer);
  Action* Clone() {return new CameraElevate(*this);}

  // Methods
  void AnimationEnd();

  // Mutators
  void SetSpeed(float val){m_speed = val;}
  void SetDistance(float val){m_speed = val;}

private:
  float m_distance;
  float m_speed;
  int m_animation_id;
  bool m_finished;
};

}