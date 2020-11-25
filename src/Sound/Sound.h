// Jake Leonard
// 2011/12/08
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Composition/Component.h"
#include "../FileIO/Serializer.h"
#include "../Containers/String.h"
#include "../Math/Vector4.h"

namespace Improbability
{
  class Sound : public Component
  {
    public:
      /* Stuffs */
      static void BindMeta(MetaComposition *comp);
      void Deserialize(Serializer &serializer) { }
      void Initialize() { }
      void Uninitialize() { }
      Component* Clone();
      Sound();

      /* Getters */
      float GetVolume(void) { return m_volume; }
      bool GetPlaying(void) { return m_playing; }
      bool GetPaused(void) { return m_paused; }
      bool GetLoop(void) { return m_loop; }
      bool GetStopped(void) { return m_stop; }
      bool GetBGM(void) { return m_stream; }
      String GetName(void) {return m_name;}

      /* Setters */
      void SetVolume(float vol) { m_volume = vol; }

      /* Function called by FMod when a sound finishes */
      void ResetSound(void) {m_paused = m_playing = false;}

      /* Controls */
      void Play(void);
      void Pause(void);
      void Stop(void);

      void update(void);
      void updatePosition(void);

    private:
      friend class SoundEmitter;
      friend class SoundSystem;

      String m_name;
      float m_volume, m_minDist, m_maxDist;
      bool m_playing, m_paused, m_3d, m_stream, m_loop, m_stop;

      Vector4* m_posPtr;
      FMOD::Channel* m_channel;
  };
}
