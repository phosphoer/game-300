// David Evans
// 2011/09/18
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "Action.h"
#include "../Math/Vector3.h"

namespace Improbability
{

class Serializer;
class MetaComposition;

class SetVelocityAction : public Action
{
public:
  static void BindMeta(MetaComposition*);

  SetVelocityAction() {}
  ~SetVelocityAction() {}

  // Inherited methods
  ActionReturn::Type Update(float dt);
  void Deserialize(Serializer &serializer);
  Action* Clone() {return new SetVelocityAction(*this);}

  // Methods
  void SetVector(Vector3 vector) {m_vector = vector;}
  void SetIgnoreZeroes(bool flag) {m_ignore_zeroes = flag;}

private:
  Vector3 m_vector;
  bool m_ignore_zeroes;
};

}