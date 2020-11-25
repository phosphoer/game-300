// David Evans
// 2011/09/20
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "Action.h"
#include "../Resource/Resource.h"

namespace Improbability
{

class LogicComponent;
class MetaComposition;

class ActionList : public Resource
{
  friend class LogicComponent;

public:
  static void BindMeta(MetaComposition*);

  // Inherited methods
  void Load(char const *filepath);
  void Unload();

private:
  std::list<Action*> m_actions;
};

}