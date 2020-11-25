// Aaron Biddlecom
// 2011/11/29
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Logic/Action.h"

namespace Improbability
{

  class Serializer;
  class MetaComposition;

  class PlayerFreezeAction : public Action
  {
  public:
    static void BindMeta(MetaComposition*);
    PlayerFreezeAction();

    // Inherited methods
    void Initialize();
    void Uninitialize();
    ActionReturn::Type Update(float dt);
    void Deserialize(Serializer &serializer);
    Action* Clone() {return new PlayerFreezeAction(*this);}
    void OnAxisChangeComplete(int axis);
  private:
    bool m_action_complete;
  };

}