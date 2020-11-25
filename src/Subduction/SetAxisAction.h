// David Evans
// 2011/10/02
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Logic/Action.h"

namespace Improbability
{

class Serializer;
class MetaComposition;

class SetAxisAction : public Action
{
public:
  static void BindMeta(MetaComposition*);

  SetAxisAction();

  // Inherited methods
  ActionReturn::Type Update(float dt);
  void Deserialize(Serializer &serializer);
  Action* Clone() {return new SetAxisAction(*this);}

  // Methods
  void SetAxis(int axis) {m_axis = axis;}

private:
  int m_axis;
};

}