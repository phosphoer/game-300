// David Evans
// 2011/10/23
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "DebugHelper.h"

namespace Improbability
{

namespace DebugUtilities
{

void EnableMemoryLeakChecking(int break_alloc)
{
  //Set the leak checking flag
  int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
  flag |= _CRTDBG_LEAK_CHECK_DF;

  _CrtSetDbgFlag(flag);

  //If a valid break alloc provided set the breakAlloc
  if (break_alloc != -1) { _CrtSetBreakAlloc(break_alloc); }
}

}

}