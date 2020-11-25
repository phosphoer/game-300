// David Evans
// 2011/09/20
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Action.h"
#include "../FileIO/Serializer.h"
#include "../Core/EngineCore.h"
#include "../Input/InputEvent.h"

namespace Improbability
{

MakeMeta(Action);

void Action::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnKeyPress);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnKeyRelease);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnActionPress);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnActionRelease);
}

Action::Action():
  m_has_run(false),
  m_time(0),
  m_lifetime(0),
  m_remove_on_update(false),
  m_is_blocking(false),
  m_is_paused(false),
  m_on_key_down(false),
  m_on_key_pressed(false),
  m_on_actionkey_down(false),
  m_on_actionkey_pressed(false),
  m_key_down(false),
  m_key_pressed(false),
  m_actionkey_down(false),
  m_actionkey_pressed(false),
  m_key_name(),
  m_actionkey_name(),
  m_key(0)
{
}

void Action::InitializeBase()
{
  if (m_on_key_down || m_on_key_pressed)
  {
    g_engine_core->AddEventListener(MessageBank::OnKeyPress, this);
    g_engine_core->AddEventListener(MessageBank::OnKeyRelease, this);

    if (m_on_key_down)
      m_key = *Meta::GetConstantValue<unsigned>(m_key_name);
    if (m_on_key_pressed)
      m_keypressed = *Meta::GetConstantValue<unsigned>(m_keypressed_name);
  }

  if (m_on_actionkey_down || m_on_actionkey_pressed)
  {
    g_engine_core->AddEventListener(MessageBank::OnActionPress, this);
    g_engine_core->AddEventListener(MessageBank::OnActionRelease, this);
  }

  Initialize();
}

ActionReturn::Type Action::UpdateBase(float dt)
{
  // Return early while paused (we don't want time to be incremented)
  if (m_is_paused)
    return ActionReturn::Continue;

  // Increment time
  m_time += dt;

  // Return early if while key mode is activated and key isn't down
  if (m_on_key_down && !m_key_down)
    //return ActionReturn::Continue;
    return InactiveUpdate(dt);

  // Return early if while key mode is activated and key isn't pressed
  if (m_on_key_pressed && !m_key_pressed)
    //return ActionReturn::Continue;
    return InactiveUpdate(dt);
  
  // Return early if while action mode is activated and action isn't down
  if (m_on_actionkey_down && !m_actionkey_down)
    //return ActionReturn::Continue;
    return InactiveUpdate(dt);
  
  // Return early if while key mode is activated and action isn't pressed
  if (m_on_actionkey_pressed && !m_actionkey_pressed)
    //return ActionReturn::Continue;
    return InactiveUpdate(dt);

  if (!m_has_run)
    Start();
  m_has_run = true;

  // Update the action and return the result
  m_key_pressed = false;
  m_actionkey_pressed = false;
  ActionReturn::Type ret = Update(dt);
  if (m_remove_on_update)
    ret = ActionReturn::Remove;

  return ret;
}

void Action::UninitializeBase()
{
  if (m_on_key_down || m_on_key_pressed)
  {
    g_engine_core->RemoveEventListener(MessageBank::OnKeyPress, this);
    g_engine_core->RemoveEventListener(MessageBank::OnKeyRelease, this);
  }

  if (m_on_actionkey_down || m_on_actionkey_pressed)
  {
    g_engine_core->RemoveEventListener(MessageBank::OnActionPress, this);
    g_engine_core->RemoveEventListener(MessageBank::OnActionRelease, this);
  }

  Uninitialize();
}

void Action::DeserializeBase(Serializer &serializer)
{
  m_action_type = serializer.GetName();
  
  Read(serializer, m_lifetime, "LifeTime");
  Read(serializer, m_remove_on_update, "RemoveOnUpdate");
  Read(serializer, m_is_blocking, "IsBlocking");
  Read(serializer, m_key_name, "OnKeyDown");
  Read(serializer, m_keypressed_name, "OnKeyPressed");
  Read(serializer, m_actionkey_name, "OnActionDown");
  Read(serializer, m_actionkeypressed_name, "OnActionPressed");

  // We aren't using the on key conditional if the string was null
  m_on_key_down = m_key_name.GetLength() > 0;
  m_on_key_pressed = m_keypressed_name.GetLength() > 0;
  m_on_actionkey_down = m_actionkey_name.GetLength() > 0;
  m_on_actionkey_pressed = m_actionkeypressed_name.GetLength() > 0;
}

void Action::OnKeyPress(KeyboardEvent event)
{
  if (event.m_keycode == m_keypressed)
    m_key_pressed = true;

  if (event.m_keycode == m_key)
    m_key_down = true;
}

void Action::OnKeyRelease(KeyboardEvent event)
{
  if (event.m_keycode == m_key)
    m_key_down = false;
}

void Action::OnActionPress(ActionEvent event)
{
  if (event.m_name == m_actionkeypressed_name)
    m_actionkey_pressed = true;

  if (event.m_name == m_actionkey_name)
    m_actionkey_down = true;
}

void Action::OnActionRelease(ActionEvent event)
{
  if (event.m_name == m_actionkey_name)
    m_actionkey_down = false;   
}

}