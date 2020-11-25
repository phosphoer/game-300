// Jake Leonard
// 2011/10/03
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "precompiled.h"
#include "PlaySoundAction.h"
#include "SoundEmitter.h"

namespace Improbability
{
  MakeMeta(PlaySoundAction);

  void PlaySoundAction::BindMeta(MetaComposition *comp)
  {
    comp->MetaAddComponent(MetaNewable);
  }

  ActionReturn::Type PlaySoundAction::Update(float dt)
  {
    Composition *obj = GetParent();
    SoundEmitter *emitter = obj->GetComponent<SoundEmitter>();

    //emitter->PlaySound("BallRoll2.wav");

    return ActionReturn::Remove;
  }

}