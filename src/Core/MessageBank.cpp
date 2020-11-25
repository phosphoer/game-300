// David Evans
// 2011/09/15
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "MessageBank.h"

namespace Improbability
{

#undef NewMessage
#define NewMessage(msg) #msg##,

char const *MessageBank::s_messages[Count] = 
{
  #include "Messages.h"
  "Null"
};

}