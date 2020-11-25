// David Evans
// 2011/05/18
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

// global include
#include "../Utilities/Precompiled.h"

// standard libraries
#include <stack>
#include <vector>

// 3rd-party dependencies
//#include <windows.h>
#include <AK/SoundEngine/Common/AkSoundEngine.h> // Everything
#include <AK/IBytes.h>
#include <AK/SoundEngine/Common/AkModule.h> // Modules
#include <AK/SoundEngine/Common/AkMemoryMgr.h> // Sound MemManager
#include <AK/SoundEngine/Common/IAkStreamMgr.h> // Streams
#include <AK/MusicEngine/Common/AkMusicEngine.h> // Music engine
#include <AK/Tools/Common/AkPlatformFuncs.h> // Threads
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AK/SoundEngine/Common/AkQueryParameters.h>
#include <AK/Plugin/AkVorbisFactory.h> // Vorbis
#include <AK/Plugin/AkRoomVerbFXFactory.h> // Room Reverb
#include <AK/Plugin/AkPitchShifterFXFactory.h> // Pitch shifting
