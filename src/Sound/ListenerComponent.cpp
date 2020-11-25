// Jake Leonard
// 2011/10/03
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "ListenerComponent.h"
#include "../Composition/composition.h"
#include "../Math/Vector4.h"

namespace Improbability
{
  MakeMeta(ListenerComponent);

  void ListenerComponent::BindMeta(MetaComposition *comp)
  {
    comp->MetaAddComponent(MetaNewable);
    comp->MetaAddComponent(MetaCallable);
  }

  Component* ListenerComponent::Clone()
  {
    ListenerComponent* newListener = new ListenerComponent();
    *newListener = *this;
    return newListener;
  }

  void ListenerComponent::update(void)
  {
    m_pos = this->GetParent()->GetPosition();

    AkListenerPosition soundPos;
    soundPos.OrientationFront.Z = soundPos.OrientationTop.Y = 1;
    soundPos.OrientationFront.Y = soundPos.OrientationFront.X = soundPos.OrientationTop.X = soundPos.OrientationTop.Z = 0;
    soundPos.Position.X = m_pos.m_x;
    soundPos.Position.Y = m_pos.m_y;
    soundPos.Position.Z = m_pos.m_z;

    AK::SoundEngine::SetListenerPosition(soundPos);
  }

  void ListenerComponent::Deserialize( Serializer &serializer )
  {
    m_pos.m_x = m_pos.m_y = m_pos.m_z = 0.0f;
  }

}

