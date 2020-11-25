// Jake Leonard
// 2011/10/03
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "precompiled.h"
#include "Sound.h"
#include "SoundEmitter.h"
#include "SoundSystem.h"
#include "../Core/EngineCore.h"

/* Sets a sound to not playing when FMOD reports the sound has finished */
FMOD_RESULT F_CALLBACK soundFinished(FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, void *commanddata1, void *commanddata2)
{
  FMOD::Channel *channelInfo = (FMOD::Channel *)channel;
  void* soundData;
  channelInfo->getUserData(&soundData);
  Improbability::Sound* theSound = reinterpret_cast<Improbability::Sound*>(soundData);
  theSound->ResetSound();

  std::cout << "Sound " <<  theSound->GetName().GetCString() << " finished" << std::endl;
  if(theSound->GetStopped())
    return FMOD_OK;

  if(theSound->GetLoop())
    theSound->Play();

  return FMOD_OK;
}

namespace Improbability
{
  MakeMeta(Sound);

  void Sound::BindMeta(MetaComposition *comp)
  {
    comp->MetaAddComponent(MetaNewable);
  }

  Sound::Sound() : m_volume(1.0f), m_minDist(1.0f), 
    m_maxDist(10000.0f), m_posPtr(NULL), m_playing(false), m_paused(false), 
    m_3d(true), m_stream(false), m_loop(false), m_stop(false)
  {
  }

  Component* Sound::Clone() { return this; }

  void Sound::update(void)
  {
    if(m_stream && !m_playing && !m_stop && m_loop)
    {
      std::cout << "BGM started" << std::endl;
      m_loop = true;
      Play();
    }
    updatePosition();
  }

  void Sound::updatePosition(void)
  {
    if(m_playing && m_3d)
    {
      FMOD_VECTOR sndPos;
      sndPos.x = m_posPtr->m_x;
      sndPos.y = m_posPtr->m_y;
      sndPos.z = m_posPtr->m_z;
      m_channel->set3DAttributes(&sndPos,NULL);
    }
  }

  void Sound::Play(void)
  {
    std::cout << "Sound " << m_name.GetCString() << " started" << std::endl;

    FMOD::Sound* curSnd = SoundSystem::GetSound(m_name)->m_snd;

    /* Set the sound to the current instance group */
    curSnd->setSoundGroup(SoundSystem::GetTopSoundGroup());
    curSnd->set3DMinMaxDistance(m_minDist, m_maxDist);

    SoundSystem::sm_fmodSys->playSound(FMOD_CHANNEL_FREE, curSnd, true, &m_channel);
    m_playing = true;

    updatePosition();

    m_channel->setVolume(m_volume);
    m_paused = m_stop = false;
    m_channel->setCallback(soundFinished);
    m_channel->setUserData(reinterpret_cast<void*>(this));
    m_channel->setPaused(false);
  }

  void Sound::Pause(void)
  {
    if(m_playing)
    {
      m_channel->getPaused(&m_paused);
      if(m_paused)
        m_channel->setPaused(false);
      else
        m_channel->setPaused(true);
    }
  }
  void Sound::Stop(void)
  {
    m_stop = true;
    if(m_playing)
      m_channel->stop();
    m_paused = m_playing = false;
  }

}


