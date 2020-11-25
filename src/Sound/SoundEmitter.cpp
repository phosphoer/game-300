// Jake Leonard
// 2011/10/03
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "SoundEmitter.h"
#include "SoundSystem.h"
#include "../Core/EngineCore.h"

namespace Improbability
{
  unsigned SoundEmitter::sm_emitterID = 1;

  MakeMeta(SoundEmitter);

  void SoundEmitter::BindMeta(MetaComposition *comp)
  {
    comp->MetaAddComponent(MetaNewable);
    comp->MetaAddComponent(MetaCallable);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnSpaceReady);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(PushAudioEvent);
  }

  void SoundEmitter::Initialize()
  {
    m_id = ++sm_emitterID;
    CriticalErrorIf(AK::SoundEngine::RegisterGameObj(AkGameObjectID(m_id)) != AK_Success, "Could not make sound emitter");
    g_engine_core->AddEventListener(MessageBank::OnSpaceReady, this);
    g_engine_core->AddEventListener(MessageBank::PushAudioEvent, this);
  }

  void SoundEmitter::Deserialize(Serializer &serializer)
  {
    /* Create a Vector pos for the emitter */
    m_pos.m_x = m_pos.m_y = m_pos.m_z = 0.0f;
  }

  Component* SoundEmitter::Clone()
  {
    SoundEmitter* newEmitter = new SoundEmitter();
    *newEmitter = *this;
    return newEmitter;
  }

  void SoundEmitter::Uninitialize()
  {
    CriticalErrorIf(AK::SoundEngine::UnregisterGameObj(AkGameObjectID(m_id)) != AK_Success, "Could not unregister Game Object Emitter");
    g_engine_core->RemoveEventListener(MessageBank::OnSpaceReady, this);
    g_engine_core->RemoveEventListener(MessageBank::PushAudioEvent, this);
  }

  void SoundEmitter::update(void)
  {
    m_pos = this->GetParent()->GetPosition();
    AkSoundPosition soundPos;
    soundPos.Position.X = m_pos.m_x;
    soundPos.Position.Y = m_pos.m_y;
    soundPos.Position.Z = m_pos.m_z;
    soundPos.Orientation.X = 1;
    soundPos.Orientation.Y = soundPos.Orientation.Z = 0;
    AK::SoundEngine::SetPosition(AkGameObjectID(m_id), soundPos);
  }

  /* EVENTS */
  void SoundEmitter::OnSpaceReady()
  {
    GetParent()->Invoke("EmitterReady");
  }

  void SoundEmitter::PushAudioEvent( char const* soundEvent )
  { 
    if(!soundEvent)
      return;

    if(AK::SoundEngine::PostEvent(AK::SoundEngine::GetIDFromString(soundEvent), AkGameObjectID(m_id)) == AK_INVALID_PLAYING_ID)
      std::cerr << "Could not launch event " << soundEvent << std::endl;
  }

  void SoundEmitter::SetEnvironment( unsigned envID )
  {
    /* Update the queue */
    if(m_environments.empty() || m_environments.top() != SoundSystem::GetCurrentEnvironmentID() || m_environments.empty() == true)
    {
      m_environments.push(SoundSystem::GetCurrentEnvironmentID());
      SetEnvironmentTop();
    }
  }

  void SoundEmitter::RemoveEnvironment(unsigned envID)
  {
    if(m_environments.top() == envID)
    {
      AK::SoundEngine::SetGameObjectEnvironmentsValues(AkGameObjectID(m_id), NULL, 0);
      m_environments.pop();
      SetEnvironmentTop();
    }
  }

  void SoundEmitter::SetEnvironmentTop()
  {
    if(m_environments.empty() == false)
    {
      AkEnvironmentValue aEnv;
      aEnv.EnvID = m_environments.top();
      aEnv.fControlValue = 1.0f; 
      AK::SoundEngine::SetGameObjectEnvironmentsValues(AkGameObjectID(m_id), &aEnv, 1);
    }
  }
}
