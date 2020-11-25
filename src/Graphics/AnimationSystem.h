// Nicholas Baldwin
// 2011/10/23
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "../core/system.h"
#include "Skeleton.h"
#include "Animation.h"

namespace Improbability
{

class AnimationSystem : public System
{
public:
  static void BindMeta(MetaComposition *comp);

  void InitializeGlobal();
  void UninitializeGlobal();

  void InitializeInstance();
  void UninitializeInstance();

  void UpdateInstance(float dt);

  void Copy(System *dest, bool deep);
  void Deserialize(Serializer &serializer);

  void OnStartAnimation(String const &animation_name, Skeleton *skeleton);
  void OnAnimationInitialize(String const &filename);
private:
  HashMap<String, Animation*> m_animations;

  struct AnimationInstance
  {
    Skeleton *m_skeleton;
    Animation m_animation;
    float m_curr_time;
  };

  std::vector<AnimationInstance> m_instances;
};

}