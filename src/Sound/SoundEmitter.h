// Jake Leonard
// 2011/12/08
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Composition/Component.h"
#include "../FileIO/Serializer.h"
#include "../Containers/String.h"
#include "../Math/Vector4.h"
#include <stack>

namespace Improbability
{
  class SoundEmitter : public Component
  {
  public:
    /* Component */
    static void BindMeta(MetaComposition *comp);
    SoundEmitter() { }
    ~SoundEmitter() { }
    void Initialize();
    void Uninitialize();
    Component* Clone();
    void Serialize(Serializer &serializer) { }
    void Deserialize(Serializer &serializer);

    void update(void);

    /* Message Handlers */
    void OnSpaceReady();
    void PushAudioEvent(char const* soundEvent);

    void SetEnvironment(unsigned envID);

    void RemoveEnvironment(unsigned envID);

  private:
    friend class SoundSystem;

    void SetEnvironmentTop();

    Vector4 m_pos;
    static unsigned sm_emitterID;
    unsigned m_id;

    std::stack<unsigned> m_environments;
  };

}