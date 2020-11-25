// Jake Leonard
// 2011/10/03
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Composition/Component.h"
#include "../FileIO/Serializer.h"

namespace Improbability
{

  class ListenerComponent : public Component
  {
  public:
    static void BindMeta(MetaComposition *comp);

    ListenerComponent() { }
    void Initialize() { }
    void Uninitialize() { }
    void update(void);
    Component* Clone();
    void Serialize(Serializer &serializer) { }
    void Deserialize(Serializer &serializer);
  private:
    Vector4 m_pos;

  };

}