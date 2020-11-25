// Jake Leonard
// 2011/10/02
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "SoundSystem.h"
#include "../core/EngineCore.h"
#include "WwiseIO/AkDefaultIOHookBlocking.h"
#include "../composition/ObjectManager.h"
#include "../Math/Vector3.h"

AkInitSettings initSettings;
AkMusicSettings musicInit;
AkPlatformInitSettings platformInitSettings;
AkMemSettings memSettings;
AkDeviceSettings deviceSettings;
AkStreamMgrSettings stmSettings;
AkBankID bankID;
CAkDefaultIOHookBlocking g_lowLevelIO;

void asyncBankLoad(AkUInt32 bnkId, AKRESULT rslt, AkMemPoolId memPoolID, void* data)
{
  if(rslt != AK_Success)
    throw "Soundbank could not be loaded.";
}

namespace Improbability
{
  MakeMeta(SoundSystem);

  void SoundSystem::BindMeta(MetaComposition *comp)
  {
    comp->MetaAddComponent(MetaNewable);
    comp->MetaAddComponent(MetaCallable);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnObjectInitialized);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnQueryAudioMute);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnChangeAudioMute);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(PushGlobalAudioEvent);
  }

  void SoundSystem::LoadSoundBank(char const* bnkFile)
  {
    if(!bnkFile)
      return;

    AK::SoundEngine::LoadBank(bnkFile, (AkBankCallbackFunc)asyncBankLoad, NULL, AK_DEFAULT_POOL_ID, bankID);
  }

  void SoundSystem::InitializeGlobal()
  {
    /* Initialize the Wwise memory system */
    memSettings.uMaxNumPools = 20;
    CriticalErrorIf(AK::MemoryMgr::Init(&memSettings) != AK_Success, "Wwise Memory Manager could not be allocated");

    /* Initialize Wwise */
    AK::SoundEngine::GetDefaultInitSettings(initSettings);
    AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

    /* Create streaming filemanager here */
    AK::StreamMgr::GetDefaultSettings(stmSettings);
    CriticalErrorIf(!AK::StreamMgr::Create(stmSettings), "Wwise Streaming Manager could not be allocated");

    /* Create the device */
    AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);
    CriticalErrorIf(g_lowLevelIO.Init(deviceSettings, true) != AK_Success, "Wwise Device Manager could not be allocated");
    
    /* Set up paths */
    g_lowLevelIO.SetBasePath( AKTEXT("audio/") );

    CriticalErrorIf(AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success, "Wwise could not be initiated.");

    /* Initialize Dynamic Audio */
    AK::MusicEngine::GetDefaultInitSettings(musicInit);
    CriticalErrorIf(AK::MusicEngine::Init(&musicInit) != AK_Success, "Wwise cannot initiate Dynamic Audio");

    /* Ogg files */
    AK::SoundEngine::RegisterCodec(AKCOMPANYID_AUDIOKINETIC, AKCODECID_VORBIS, CreateVorbisFilePlugin, CreateVorbisBankPlugin);

    /* Room Reverb */
    AK::SoundEngine::RegisterPlugin(AkPluginTypeEffect, AKCOMPANYID_AUDIOKINETIC, AKEFFECTID_ROOMVERB, CreateRoomVerbFX, CreateRoomVerbFXParams);

    /* Pitch shifter */
    AK::SoundEngine::RegisterPlugin(AkPluginTypeEffect, AKCOMPANYID_AUDIOKINETIC, AKEFFECTID_PITCHSHIFTER, CreatePitchShifterFX, CreatePitchShifterFXParams);

    /* Load the initial soundbank that Wwise creates. */
    LoadSoundBank("Init.bnk");

    LoadSoundBank("subduction.bnk");

    /* Fire the sound loaded event */
    g_engine_core->DispatchEvent(MessageBank::OnLoadPass, "SoundInitialize");
  }

  void SoundSystem::InitializeInstance()
  {
    // Generate sound group id
    m_environID = sm_environmentNum;

    ++sm_environmentNum;

    // Grab all previous sounds
    if(sm_soundSystems.empty() == false)
      AK::SoundEngine::BypassEnvironment(sm_soundSystems.top()->m_environID, true);

    AK::SoundEngine::SetEnvironmentVolume(m_environID, sm_volume);
    g_engine_core->AddEventListener(MessageBank::OnObjectInitialized, this);
    g_engine_core->AddEventListener(MessageBank::OnChangeAudioMute, this);
    g_engine_core->AddEventListener(MessageBank::OnQueryAudioMute, this);
    g_engine_core->AddEventListener(MessageBank::PushGlobalAudioEvent, this);
    sm_soundSystems.push(this);
  }

  void SoundSystem::UpdateInstance(float dt)
  {
    for(SoundListenerList::iterator it = m_soundListeners.begin(); it != m_soundListeners.end(); ++it)
      (*it)->update();

    for(SoundEmitterList::iterator it = m_soundEmitters.begin(); it != m_soundEmitters.end(); ++it)
      (*it)->update();

    AK::SoundEngine::RenderAudio();
  }

  void SoundSystem::ForceUpdate()
  {
    AK::SoundEngine::RenderAudio();
  }

  void SoundSystem::PauseSounds(bool paused)
  {
    if(paused)
      AK::SoundEngine::BypassEnvironment(m_environID, false);
    else
      AK::SoundEngine::BypassEnvironment(m_environID, true);

    paused = !paused;
  }

  void SoundSystem::StopSounds()
  {
    AK::SoundEngine::StopAll();
  }

  void SoundSystem::UninitializeInstance()
  {
    AK::SoundEngine::BypassEnvironment(m_environID, true);

    /* Reset sound groups? */
    for(auto it = m_soundEmitters.begin(); it != m_soundEmitters.end(); ++it)
      (*it)->RemoveEnvironment(m_environID);
    
    g_engine_core->RemoveEventListener(MessageBank::OnChangeAudioMute, this);
    g_engine_core->RemoveEventListener(MessageBank::OnQueryAudioMute, this);
    g_engine_core->RemoveEventListener(MessageBank::OnObjectInitialized, this);
    g_engine_core->RemoveEventListener(MessageBank::PushGlobalAudioEvent, this);
    sm_soundSystems.pop();

    if(sm_soundSystems.empty() == false)
    {
      AK::SoundEngine::BypassEnvironment(sm_soundSystems.top()->m_environID, false);
      AK::SoundEngine::SetEnvironmentVolume(sm_soundSystems.top()->m_environID, sm_volume);
    }
  }

  void SoundSystem::UninitializeGlobal()
  {
    AK::MusicEngine::Term();
    AK::SoundEngine::Term();

    g_lowLevelIO.Term();

    if (AK::IAkStreamMgr::Get())
      AK::IAkStreamMgr::Get()->Destroy();

    AK::MemoryMgr::Term();
  }

  void SoundSystem::OnObjectInitialized(CompositionId comp_id)
  {
    Composition *comp = g_engine_core->SystemCall("ObjectManager", "GetObject", comp_id);
    ListenerComponent* objListener = comp->GetComponent<ListenerComponent>();
    
    if(objListener)
      m_soundListeners.push_back(objListener);

    SoundEmitter* sndEmitter = comp->GetComponent<SoundEmitter>();

    if(sndEmitter)
    {
      sndEmitter->SetEnvironment(m_environID);
      m_soundEmitters.push_back(sndEmitter);
    }
  }

  void SoundSystem::OnQueryAudioMute()
  {
    g_engine_core->DispatchEvent("OnRecieveAudioMute", sm_muted);
  }

  void SoundSystem::OnChangeAudioMute(bool newSetting)
  {
    sm_muted = newSetting;
    if(newSetting == true)
      sm_volume = 0.0f;
    else
      sm_volume = 1.0f;
  }

  void SoundSystem::PushGlobalAudioEvent(char const* audioEvent)
  {
    if(!audioEvent)
      return;

    AK::SoundEngine::PostEvent(AK::SoundEngine::GetIDFromString(audioEvent),AkGameObjectID(SoundEmitter::sm_emitterID));
    SoundSystem::ForceUpdate();
  }

  unsigned SoundSystem::sm_environmentNum = 2;

  bool SoundSystem::sm_muted = false;

  float SoundSystem::sm_volume = 1.0f;

  SoundSystem::SoundSystemStack SoundSystem::sm_soundSystems;
}