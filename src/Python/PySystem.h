// Jake Leonard
// 2011/05/25
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

// The majority of files that you are going to
// include to use with Python are this and PyBinding.
// You should not need to include any other file.
#pragma once

#include "../Core/System.h"
#include "../Input/InputEvent.h"

namespace Improbability
{

class PythonModule;
class MetaComposition;

class PythonSystem : public System
{
public:
  static void BindMeta(MetaComposition*);

  void InitializeGlobal();
  void PostInitializeGlobal();
  void InitializeInstance();
  void PostInitializeInstance() { }
  void UpdateInstance(float dt);
  void UninitializeInstance();
  void UninitializeGlobal();
  void Copy(System *dest, bool deep) { }
  void Deserialize(Serializer &serializer) { }

  // Events
  void OnKeyRelease(KeyboardEvent event);

  // This is a wrapper to PyFiles::ReloadFile as PyFiles has Python objects that other files wouldn't know about
  // ReloadPythonFile - Reloads a Python file.
  //   fileName - the name of a python file (file ext optional).
  static void ReloadPythonFile(const char* fileName);

  // CallFunction - Calls a Python Function
  //  fileName - The name of the python file (.py extension optional)
  //  funcName - The name of the function to call
  //  argFormat - If the function takes nothing, pass NULL
  //            - If it does take stuff, this is the format string
  //            - s = string, i = int, d = double, f = float, u/I = unsigned, b = bool
  static void CallFunction(char * const fileName, char * const funcName, char * const argFormat, ...);

  static void InitializeRuntime();

  static bool IsInitialized() { return sm_initialized; }

private:
  // Nothing for now. Maybe in the future, shmaybe not.
  static PythonModule *globalModule;
  bool m_consoleOpened;
  static bool sm_initialized;
};

}
