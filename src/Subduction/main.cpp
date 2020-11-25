// David Evans
// 2011/05/12
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "../Core/EngineCore.h"
#include "../Composition/Composition.h"
#include "../Core/WindowSystem.h"
#include "../Utilities/CrashHandler.h"
#include "../Utilities/DebugHelper.h"
#include "Test.h"

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dsound.lib")
#pragma comment (lib, "dbghelp.lib")

#ifdef _DEBUG
  // Include debug libraries here
  #pragma comment (lib, "tinyxml_d.lib")
  #pragma comment (lib, "d3dx9d.lib")
  #pragma comment (lib, "DxErr.lib")
  #pragma comment (lib, "libcef_d.lib")
  #pragma comment (lib, "libcef_dll_wrapper_d.lib")
#else
  // Include non-debug libraries here
  #define AK_OPTIMIZED
  #pragma comment (lib, "tinyxml.lib")
  #pragma comment (lib, "d3dx9.lib")
  #pragma comment (lib, "libcef.lib")
  #pragma comment (lib, "libcef_dll_wrapper.lib")
  //#pragma comment (lib, "Profiler.lib")
#endif

#pragma comment (lib, "AkMusicEngine.lib")
#pragma comment (lib, "AkStreamMgr.lib")
#pragma comment (lib, "AkSoundEngine.lib")
#pragma comment (lib, "AkMemoryMgr.lib")
#pragma comment (lib, "AkVorbisDecoder.lib")
#pragma comment (lib, "AkPitchShifterFX.lib")
#pragma comment (lib, "AkRoomVerbFX.lib")


int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, PSTR cmd_line, int cmd_show)
{
  using namespace Improbability;

  DebugUtilities::RegisterCrashHandler();
  //DebugUtilities::EnableMemoryLeakChecking();

  WindowSystem::s_instance = instance;
  Meta::Initialize();
  UISystemInterface::Initialize();
  
  EngineCore core;
  core.Initialize("init-space.spc", "res/window-config.cfg");

  core.MainLoop();

  core.Uninitialize();
  UISystemInterface::Uninitialize();
  Meta::Uninitialize();

  return 0;
}