// Jake Leonard
// 2011/10/02
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Core/System.h"
#include "../Containers/HashMap.h"
#include "../Math/Vector4.h"
#include "../Composition/composition.h"
#include "ListenerComponent.h"
#include "SoundEmitter.h"

namespace Improbability
{
  class SoundEmitter;
  class SoundFile;
  class SoundSystem : public System
  {
  public:
    typedef std::stack<class SoundSystem*> SoundSystemStack;
    typedef std::vector<class SoundEmitter*> SoundEmitterList;
    typedef std::vector<class ListenerComponent*> SoundListenerList;

  public:
    static void BindMeta(MetaComposition *comp);

    void InitializeGlobal();
    void PostInitializeGlobal() {}
    void InitializeInstance();
    void PostInitializeInstance() {}
    void UpdateInstance(float dt);
    void UninitializeInstance();
    void UninitializeGlobal();
    void Copy(System *dest, bool deep) {}
    void Deserialize(Serializer &serializer) {}

    /* Keeping track of listeners and emitters */
    void OnQueryAudioMute();
    void OnChangeAudioMute(bool newSetting);
    void PushGlobalAudioEvent(char const* audioEvent);
    void OnObjectInitialized(CompositionId comp_id);

    /* Sound management */
    static void LoadSoundBank(char const* bnkFile);
    
    static float GetVolume() { return sm_volume; }
    static void SetVolume(float vol);

    /* Pauses and stops the sound for this instance */
    void StopSounds();
    void PauseSounds(bool paused);

    unsigned GetEnvironmentID() { return m_environID; }

    static unsigned GetCurrentEnvironmentID() { return sm_soundSystems.top()->m_environID; }
    static void ForceUpdate();
  private:
    /* Lets us push sounds to the top most sound system */
    static SoundSystemStack sm_soundSystems;

    /* List of emitters */
    SoundEmitterList m_soundEmitters;

    /* List of listeners */
    SoundListenerList m_soundListeners;

    static float sm_volume;

    static bool sm_muted;

    /* ID */
    unsigned m_environID;

    static unsigned sm_environmentNum;

  };

}
