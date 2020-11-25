// David Evans
// 2011/09/17
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "Precompiled.h"
#include "LogicComponent.h"
#include "ActionList.h"
#include "../FileIO/Serializer.h"
#include "../Core/EngineCore.h"
#include "../Resource/ResourceManager.h"

namespace Improbability
{

MakeMeta(LogicComponent);

void LogicComponent::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

LogicComponent::LogicComponent(LogicComponent const &rhs)
{
  m_list_name = rhs.m_list_name;
}

void LogicComponent::Initialize()
{
  // Add actions
  AddActionList(m_list_name);

  // Initialize actions
  auto it = m_actions.begin();
  auto end = m_actions.end();
  for (; it != end; ++it)
  {
    Action *action = *it;
    action->InitializeBase();
  }
}

void LogicComponent::Uninitialize()
{
  // Uninitialize actions
  auto it = m_actions.begin();
  auto end = m_actions.end();
  for (; it != end; ++it)
  {
    Action *action = *it;
    action->UninitializeBase();
    delete action;
  }
}

Component* LogicComponent::Clone()
{
  return new LogicComponent(*this);
}

void LogicComponent::Deserialize(Serializer &serializer)
{
  Read(serializer, m_list_name, "ActionList");
}

void LogicComponent::Update(float dt)
{
  // Iterate over the action list
  auto it = m_actions.begin();
  auto end = m_actions.end();
  for (; it != end;)
  {
    Action *action = *it;

    // Update the action
    ActionReturn::Type ret = action->UpdateBase(dt);

    // Check the lifetime of the action and remove it if it has died
    float lifetime = action->GetLifeTime();
    if (lifetime > 0.0f && action->GetTime() >= lifetime)
      ret = ActionReturn::Remove;

    // Save the next iterator
    auto old_it = it++;

    // If the action returns Remove, we should remove it and delete it
    if (ret == ActionReturn::Remove)
    {
      action->End();
      action->UninitializeBase();
      m_actions.erase(old_it);
      delete action;
    }
    // If the action returns Block, it means no more actions should be executed
    // We also block if the action is set as a blocking element
    else if (ret == ActionReturn::Block || action->IsBlocking())
    {
      break;
    }
    // Not really anything to do otherwise, but might as well have a case for it
    // for clarity
    else if (ret == ActionReturn::Continue)
    {
    }
  }
}

void LogicComponent::AddActionList(char const *file)
{
  // Get the list from the resource manager and add it
  ActionList *actions = g_engine_core->GetResourceManager()->GetResource<ActionList>(file);
  AddActionList(actions);
}

void LogicComponent::AddActionList(ActionList *list)
{
  // Iterate over the list and clone its actions into our list
  auto it = list->m_actions.begin();
  auto end = list->m_actions.end();
  for (; it != end; ++it)
  {
    m_actions.push_back((*it)->Clone());
    m_actions.back()->m_parent = GetParent();
    m_actions.back()->m_logic = this;
    m_actions.back()->InitializeBase();
  }
}


Action* LogicComponent::PushFront(int action_id, float lifetime)
{
  // Create the desired action
  Action *action = Meta::Create<Action>(ActionBank::GetAction(action_id));
  action->SetLifeTime(lifetime);
  action->m_parent = GetParent();
  action->InitializeBase();

  // Add it to the list
  m_actions.push_front(action);

  // Return a reference to the action
  return action;
}

Action* LogicComponent::PushBack(int action_id, float lifetime)
{
  // Create the desired action
  Action *action = Meta::Create<Action>(ActionBank::GetAction(action_id));
  action->SetLifeTime(lifetime);
  action->m_parent = GetParent();
  action->InitializeBase();

  // Add it to the list
  m_actions.push_back(action);

  // Return a reference to the action
  return action;
}

}