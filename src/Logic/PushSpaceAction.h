// David Evans
// 2011/09/27
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "Action.h"

namespace Improbability
{

class Serializer;
class MetaComposition;

class PushSpaceAction : public Action
{
public:
  static void BindMeta(MetaComposition*);

  // Inherited methods
  ActionReturn::Type Update(float dt);
  void Deserialize(Serializer &serializer);
  Action* Clone() {return new PushSpaceAction(*this);}

  // Methods
  void SetSpaceName(char const *name) {m_space_name = name;}
  void SetIsDeep(bool deep) {m_is_deep = deep;}

private:
  String m_space_name;
  bool m_is_deep;
};

}