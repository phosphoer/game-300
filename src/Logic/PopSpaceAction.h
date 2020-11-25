// David Evans
// 2011/09/29
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "Action.h"

namespace Improbability
{

class Serializer;
class MetaComposition;

class PopSpaceAction : public Action
{
public:
  static void BindMeta(MetaComposition*);

  // Inherited methods
  ActionReturn::Type Update(float dt);
  void Deserialize(Serializer &serializer);
  Action* Clone() {return new PopSpaceAction(*this);}

  // Methods
  void SetIsDeep(bool deep) {m_is_deep = deep;}

private:
  bool m_is_deep;
};

}