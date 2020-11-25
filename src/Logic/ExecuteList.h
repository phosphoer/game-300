// David Evans
// 2011/09/20
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "Action.h"

namespace Improbability
{

class Serializer;
class MetaComposition;

class ExecuteList : public Action
{
public:
  static void BindMeta(MetaComposition*);

  // Inherited methods
  ActionReturn::Type Update(float dt);
  void Deserialize(Serializer &serializer);
  Action* Clone() {return new ExecuteList(*this);}

  // Methods
  void SetActionList(char const *name) {m_action_name = name;}

private:
  String m_action_name;
};

}