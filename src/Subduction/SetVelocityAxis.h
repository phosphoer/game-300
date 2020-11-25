// David Evans
// 2011/09/18
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Logic/Action.h"
#include "../Math/Vector3.h"

namespace Improbability
{

class Serializer;
class MetaComposition;

class SetVelocityAxis : public Action
{
public:
  static void BindMeta(MetaComposition*);

  SetVelocityAxis();

  // Inherited methods
  void Initialize();
  void Start();
  ActionReturn::Type Update(float dt);
  void Uninitialize();
  void Deserialize(Serializer &serializer);
  Action* Clone() {return new SetVelocityAxis(*this);}

  // Messages
  void OnAxisChange(int old_axis, int new_axis);
  void OnPlayerSideCollision();

  // Methods
  void SetVector(Vector3 vector);// {m_vector = vector;}
  void SetIgnoreZeroes(bool flag) {m_ignore_zeroes = flag;}
private:
  Vector3 m_vector;
  int m_axis;
  bool m_ignore_zeroes;
  bool m_collide;
};

}