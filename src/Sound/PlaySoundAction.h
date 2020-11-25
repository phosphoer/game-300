// Jake Leonard
// 2011/10/03
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Logic/Action.h"
#include "../Composition/Composition.h"

namespace Improbability
{
  class Serializer;
  class MetaComposition;

  class PlaySoundAction : public Action
  {
    public:
      static void BindMeta(MetaComposition* comp);

      PlaySoundAction() { }
      ~PlaySoundAction() { }

      ActionReturn::Type Update(float dt);
      void Deserialize(Serializer &serializer) { }
      Action* Clone() {return new PlaySoundAction(*this);}
  };
}