// David Evans
// 2011/10/28
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "Action.h"

namespace Improbability
{

class Serializer;
class MetaComposition;

class WaitAction : public Action
{
public:
  static void BindMeta(MetaComposition*);

  // Inherited methods
  ActionReturn::Type Update(float dt);
  void Deserialize(Serializer &serializer);
  Action* Clone() {return new WaitAction(*this);}

private:
};

}