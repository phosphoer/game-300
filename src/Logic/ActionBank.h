// David Evans
// 2011/09/18
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{

class ActionBank
{
public:
  #define NewAction(action) action##,

  enum Type
  {
    #include "Actions.h"
    Null,
    Count
  };

  static char const* GetAction(int id) {return s_actions[id];}

private:
  static char const *s_actions[Count];
};

}