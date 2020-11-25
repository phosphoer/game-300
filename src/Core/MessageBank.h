// David Evans
// 2011/09/15
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#undef GetMessage

namespace Improbability
{

class MessageBank
{
public:
  #define NewMessage(msg) msg##,

  enum Type
  {
    #include "Messages.h"
    Null,
    Count
  };

  static char const* GetMessage(int id) {return s_messages[id];}

private:
  static char const *s_messages[Count];
};

}