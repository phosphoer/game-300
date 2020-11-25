// Nicholas Baldwin
// 2011/10/24
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "AnimationSystem.h"
#include "..\core\EngineCore.h"
#include "Animation.h"
#include "Skeleton.h"
#include "..\FileIO\Serializer.h"

namespace Improbability
{

MakeMeta(AnimationSystem);

void AnimationSystem::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnStartAnimation);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAnimationInitialize);
}

void AnimationSystem::InitializeGlobal()
{

}

void AnimationSystem::UninitializeGlobal()
{

}

void AnimationSystem::InitializeInstance()
{
  g_engine_core->AddEventListener(MessageBank::OnStartAnimation, this);
  g_engine_core->AddEventListener(MessageBank::OnAnimationInitialize, this);
}

void AnimationSystem::UninitializeInstance()
{
  g_engine_core->RemoveEventListener(MessageBank::OnStartAnimation, this);
  g_engine_core->RemoveEventListener(MessageBank::OnAnimationInitialize, this);
}

void AnimationSystem::UpdateInstance(float dt)
{
  for (auto it = m_instances.begin(); it != m_instances.end(); ++it)
  {
    it->m_curr_time += dt;
    float run_time = it->m_animation.Update(it->m_curr_time, *(it->m_skeleton));
  }
}

void AnimationSystem::Copy(System *dest, bool deep)
{

}

void AnimationSystem::Deserialize(Serializer &serializer)
{

}

void AnimationSystem::OnStartAnimation(String const &animation_name, Skeleton *skeleton)
{
  AnimationInstance new_instance;
  new_instance.m_animation = *(m_animations[animation_name]);
  new_instance.m_skeleton = skeleton;
  new_instance.m_curr_time = 0.0f;
  m_instances.push_back(new_instance);
}

void AnimationSystem::OnAnimationInitialize(String const &filename)
{
  Serializer animation_file(filename);

  animation_file.EnterElement("animations");

  String animation_name;

  ReadAttribute(animation_file, animation_name, "name");
  m_animations[animation_name] = new Animation;

  m_animations[animation_name]->Deserialize(animation_file);
}

}