// David Evans
// 2011/09/18
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "ActionBank.h"

namespace Improbability
{

#undef NewAction
#define NewAction(action) #action##,

char const *ActionBank::s_actions[Count] = 
{
  #include "Actions.h"
  "Null"
};

}