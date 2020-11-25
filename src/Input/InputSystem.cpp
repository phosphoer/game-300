// David Evans
// 2011/09/07
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "InputSystem.h"
#include "InputEvent.h"
#include "InputActions.h"
#include "../Core/EngineCore.h"
#include "../Core/WindowSystem.h"
#include "../Resource/ResourceManager.h"
#include "../FileIO/Serializer.h"

namespace Improbability
{

unsigned InputSystem::m_controller_config = 0;

MakeMeta(InputSystem);

void InputSystem::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnKeyDownWin);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnCharDownWin);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnKeyUpWin);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnMouseMoveWin);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnMouseScrollWin);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnMouseLeftButtonDownWin);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnMouseMiddleButtonDownWin);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnMouseRightButtonDownWin);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnMouseLeftButtonUpWin);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnMouseMiddleButtonUpWin);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnMouseRightButtonUpWin);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnChangeControllerConfig);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnQueryControllerConfig);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnSpacePushed);

}

void InputSystem::InitializeGlobal()
{
  WindowSystem *window = g_engine_core->GetWindow();
  window->RegisterMessage(WM_CHAR, MessageBank::OnCharDownWin);
  window->RegisterMessage(WM_CLOSE, MessageBank::OnWindowClose);
  window->RegisterMessage(WM_KEYDOWN, MessageBank::OnKeyDownWin);
  window->RegisterMessage(WM_KEYUP, MessageBank::OnKeyUpWin);
  window->RegisterMessage(WM_MOUSEMOVE, MessageBank::OnMouseMoveWin);
  window->RegisterMessage(WM_MOUSEWHEEL, MessageBank::OnMouseScrollWin);
  window->RegisterMessage(WM_LBUTTONDOWN, MessageBank::OnMouseLeftButtonDownWin);
  window->RegisterMessage(WM_MBUTTONDOWN, MessageBank::OnMouseMiddleButtonDownWin);
  window->RegisterMessage(WM_RBUTTONDOWN, MessageBank::OnMouseRightButtonDownWin);
  window->RegisterMessage(WM_LBUTTONUP, MessageBank::OnMouseLeftButtonUpWin);
  window->RegisterMessage(WM_MBUTTONUP, MessageBank::OnMouseMiddleButtonUpWin);
  window->RegisterMessage(WM_RBUTTONUP, MessageBank::OnMouseRightButtonUpWin);
}

void InputSystem::InitializeInstance()
{
  // Zero out key states
  std::memset(m_keystates, 0, sizeof(int) * k_num_keys);
  std::memset(m_mousemove_states, 0, sizeof(int) * 4);

  m_mouse_x = 0;
  m_mouse_y = 0;
  m_mouse_drag = false;
  m_mouse_move_x = 0;
  m_mouse_move_y = 0;
  m_mouse_scroll = 0;

  for (unsigned i = 0; i < m_actions_files.size(); ++i)
  {
    m_actions_vector.push_back(g_engine_core->GetResourceManager()->GetResource<InputActions>(m_actions_files[i]));
  }

  g_engine_core->AddEventListener(MessageBank::OnCharDownWin, this);
  g_engine_core->AddEventListener(MessageBank::OnKeyDownWin, this);
  g_engine_core->AddEventListener(MessageBank::OnKeyUpWin, this);
  g_engine_core->AddEventListener(MessageBank::OnMouseMoveWin, this);
  g_engine_core->AddEventListener(MessageBank::OnMouseScrollWin, this);
  g_engine_core->AddEventListener(MessageBank::OnMouseLeftButtonDownWin, this);
  g_engine_core->AddEventListener(MessageBank::OnMouseMiddleButtonDownWin, this);
  g_engine_core->AddEventListener(MessageBank::OnMouseRightButtonDownWin, this);
  g_engine_core->AddEventListener(MessageBank::OnMouseLeftButtonUpWin, this);
  g_engine_core->AddEventListener(MessageBank::OnMouseMiddleButtonUpWin, this);
  g_engine_core->AddEventListener(MessageBank::OnMouseRightButtonUpWin, this);
  g_engine_core->AddEventListener(MessageBank::OnSpacePushed, this);
  g_engine_core->AddEventListener(MessageBank::OnChangeControllerConfig, this);
  g_engine_core->AddEventListener(MessageBank::OnQueryControllerConfig, this);
}

void InputSystem::UpdateInstance(float dt)
{
  // Update input states
  for (int i = 0; i < k_num_keys; ++i)
  {
    // Check if this key is registered to an action and 
    // dispatch an action event
    if (m_keystates[i] && m_actions_vector.size() > m_controller_config)
    {
      String action = m_actions_vector[m_controller_config]->GetAction(i);
      if (action.GetLength() > 0)
      {
        ActionEvent event;
        event.m_name = action;
        event.m_keystates = m_keystates;
        
        g_engine_core->DispatchEvent(MessageBank::OnActionDown, event);
      }
    }
    
    // Dispatch mouse event
    if (m_keystates[i] && i <= k_num_mousekeys)
    {
      MouseEvent event;
      event.m_buttoncode = i;
      event.m_x = m_mouse_x;
      event.m_y = m_mouse_y;
      event.m_x_move = m_mouse_move_x;
      event.m_y_move = m_mouse_move_y;
      event.m_scroll = m_mouse_scroll;
      event.m_scroll_move = m_mouse_scroll_move;
      event.m_keystates = m_keystates;

      g_engine_core->DispatchEvent(MessageBank::OnMouseDown, event);
    }
    // Dispatch keyboard event
    else if (m_keystates[i] && i > k_num_mousekeys)
    {
      KeyboardEvent event;
      event.m_keycode = i;
      event.m_modifiers = m_keystates[i] - 1;
      event.m_keystates = m_keystates;

      g_engine_core->DispatchEvent(MessageBank::OnKeyDown, event);
    }
  }

  // Reset states
  m_mouse_move_x = 0;
  m_mouse_move_y = 0;
  m_mouse_scroll_move = 0;
}

void InputSystem::UninitializeInstance()
{
  g_engine_core->RemoveEventListener(MessageBank::OnCharDownWin, this);
  g_engine_core->RemoveEventListener(MessageBank::OnKeyDownWin, this);
  g_engine_core->RemoveEventListener(MessageBank::OnKeyUpWin, this);
  g_engine_core->RemoveEventListener(MessageBank::OnMouseMoveWin, this);
  g_engine_core->RemoveEventListener(MessageBank::OnMouseScrollWin, this);
  g_engine_core->RemoveEventListener(MessageBank::OnMouseLeftButtonDownWin, this);
  g_engine_core->RemoveEventListener(MessageBank::OnMouseMiddleButtonDownWin, this);
  g_engine_core->RemoveEventListener(MessageBank::OnMouseRightButtonDownWin, this);
  g_engine_core->RemoveEventListener(MessageBank::OnMouseLeftButtonUpWin, this);
  g_engine_core->RemoveEventListener(MessageBank::OnMouseMiddleButtonUpWin, this);
  g_engine_core->RemoveEventListener(MessageBank::OnMouseRightButtonUpWin, this);
  g_engine_core->RemoveEventListener(MessageBank::OnSpacePushed, this);
  g_engine_core->RemoveEventListener(MessageBank::OnChangeControllerConfig, this);
  g_engine_core->RemoveEventListener(MessageBank::OnQueryControllerConfig, this);
}

void InputSystem::UninitializeGlobal()
{
}

void InputSystem::Copy(System *dest, bool deep)
{
  InputSystem *other = reinterpret_cast<InputSystem*>(dest);

//  for (int i = 0; i < k_num_keys; ++i)
//    other->m_keystates[i] = m_keystates[i];
//
//  for (int i = 0; i < 4; ++i)
//    other->m_mousemove_states[i] = m_mousemove_states[i];

  other->m_mouse_x = m_mouse_x;
  other->m_mouse_y = m_mouse_y;
  other->m_mouse_move_x = m_mouse_move_x;
  other->m_mouse_move_y = m_mouse_move_y;
  other->m_mouse_scroll = m_mouse_scroll;
  other->m_mouse_scroll_move = m_mouse_scroll_move;
}

void InputSystem::Deserialize(Serializer &serializer)
{
  String action_file;
  for (serializer.Begin(); serializer.IsGood(); serializer.Iterate())
  {
    Read (serializer, action_file);
    m_actions_files.push_back(action_file);
  }
  serializer.LeaveElement();
}

void InputSystem::OnCharDownWin(unsigned wparam, unsigned lparam)
{
  if (~lparam & (1 << 31))
    g_engine_core->DispatchEvent(MessageBank::OnCharPress, wparam, lparam);
}

void InputSystem::OnKeyDownWin(unsigned wparam, unsigned lparam)
{
  // Check if this key is registered to an action and 
  // dispatch an action event
  if (m_actions_vector.size() > m_controller_config)
  {
    String action = m_actions_vector[m_controller_config]->GetAction(wparam);
    if (action.GetLength() > 0)
    {
      ActionEvent event;
      event.m_name = action;
      event.m_keystates = m_keystates;
      
      if (m_keystates[wparam] == 0)
        g_engine_core->DispatchEvent(MessageBank::OnActionPress, event);
    }
  }

  // If this was a keyboard key, send an event
  if (wparam > k_num_mousekeys && m_keystates[wparam] == 0)
  {
    KeyboardEvent event;
    event.m_keycode = wparam;
    event.m_modifiers = lparam;
    event.m_keystates = m_keystates;

    g_engine_core->DispatchEvent(MessageBank::OnKeyPress, event);

    m_keystates[wparam] = lparam + 1;
  }
  // Otherwise it was a mouse event
  else if (m_keystates[wparam] == 0)
  {
    MouseEvent event;
    event.m_buttoncode = wparam;
    event.m_x = m_mouse_x;
    event.m_y = m_mouse_y;
    event.m_x_move = m_mouse_move_x;
    event.m_y_move = m_mouse_move_y;
    event.m_scroll = m_mouse_scroll;
    event.m_scroll_move = m_mouse_scroll_move;
    event.m_keystates = m_keystates;

    g_engine_core->DispatchEvent(MessageBank::OnMousePress, event);

    m_keystates[wparam] = 1;
  }
}

void InputSystem::OnKeyUpWin(unsigned wparam, unsigned lparam)
{
  // Check if this key is registered to an action and 
  // dispatch an action event
  if (m_actions_vector.size() > m_controller_config && m_keystates[wparam])
  {
    String action = m_actions_vector[m_controller_config]->GetAction(wparam);
    if (action.GetLength() > 0)
    {
      ActionEvent event;
      event.m_name = action;
      event.m_keystates = m_keystates;
      g_engine_core->DispatchEvent(MessageBank::OnActionRelease, event);
    }
  }

  // If this was a keyboard key, send an event
  if (wparam > k_num_mousekeys && m_keystates[wparam])
  {
    KeyboardEvent event;
    event.m_keycode = wparam;
    event.m_modifiers = lparam;
    event.m_keystates = m_keystates;

    g_engine_core->DispatchEvent(MessageBank::OnKeyRelease, event);
  }
  // Otherwise it was a mouse event
  else if (m_keystates[wparam] == 1)
  {
    MouseEvent event;
    event.m_buttoncode = wparam;
    event.m_x = m_mouse_x;
    event.m_y = m_mouse_y;
    event.m_x_move = m_mouse_move_x;
    event.m_y_move = m_mouse_move_y;
    event.m_scroll = m_mouse_scroll;
    event.m_scroll_move = m_mouse_scroll_move;
    event.m_keystates = m_keystates;
    if(m_mouse_drag)
    {
      g_engine_core->DispatchEvent(MessageBank::OnMouseDrag, event);
      m_mouse_drag = false;
    }
    else // Not sure if we should fire this as well as the mouse drag.
      g_engine_core->DispatchEvent(MessageBank::OnMouseRelease, event);
  }
  m_keystates[wparam] = 0;
}

void InputSystem::OnMouseMoveWin(unsigned wparam, unsigned lparam)
{
  if(m_keystates[wparam] == 1)
    m_mouse_drag = true;

  m_mouse_move_x = GET_X_LPARAM(lparam) - m_mouse_x;
  m_mouse_move_y = GET_Y_LPARAM(lparam) - m_mouse_y;

  if(m_mouse_drag == true)
    return;
  
  // Get mouse position
  MouseEvent event;
  event.m_x = GET_X_LPARAM(lparam);
  event.m_y = GET_Y_LPARAM(lparam);
  event.m_x_move = m_mouse_move_x;
  event.m_y_move = m_mouse_move_y;
  event.m_scroll = 0;
  event.m_scroll_move = m_mouse_scroll_move;
  event.m_buttoncode = 0;
  event.m_keystates = m_keystates;
  m_mouse_x = event.m_x;
  m_mouse_y = event.m_y;

  g_engine_core->DispatchEvent(MessageBank::OnMouseMove, event);
}

void InputSystem::OnMouseScrollWin(unsigned wparam, unsigned lparam)
{
  // Get scroll move
  m_mouse_scroll += GET_WHEEL_DELTA_WPARAM(wparam);

  // Setup event
  MouseEvent event;
  event.m_x = m_mouse_x;
  event.m_y = m_mouse_y;
  event.m_x_move = m_mouse_move_x;
  event.m_y_move = m_mouse_move_y;
  event.m_scroll_move = GET_WHEEL_DELTA_WPARAM(wparam);
  event.m_scroll = m_mouse_scroll;
  event.m_keystates = m_keystates;

  g_engine_core->DispatchEvent(MessageBank::OnMouseScroll, event);
}

void InputSystem::OnMouseLeftButtonDownWin(unsigned wparam, unsigned lparam)
{
  m_mouse_x = GET_X_LPARAM(lparam);
  m_mouse_y = GET_Y_LPARAM(lparam);
  OnKeyDownWin(kMouseLeftButton, lparam);
}

void InputSystem::OnMouseMiddleButtonDownWin(unsigned wparam, unsigned lparam)
{
  m_mouse_x = GET_X_LPARAM(lparam);
  m_mouse_y = GET_Y_LPARAM(lparam);
  OnKeyDownWin(kMouseMiddleButton, lparam);
}

void InputSystem::OnMouseRightButtonDownWin(unsigned wparam, unsigned lparam)
{
  m_mouse_x = GET_X_LPARAM(lparam);
  m_mouse_y = GET_Y_LPARAM(lparam);
  OnKeyDownWin(kMouseRightButton, lparam);
}

void InputSystem::OnMouseLeftButtonUpWin(unsigned wparam, unsigned lparam)
{
  if(m_mouse_drag)
  {
    m_mouse_move_x = GET_X_LPARAM(lparam) - m_mouse_x;
    m_mouse_move_y = GET_Y_LPARAM(lparam) - m_mouse_y;
  }
  else
  {
    m_mouse_x = GET_X_LPARAM(lparam);
    m_mouse_y = GET_Y_LPARAM(lparam);
  }
  OnKeyUpWin(kMouseLeftButton, lparam);
}

void InputSystem::OnMouseMiddleButtonUpWin(unsigned wparam, unsigned lparam)
{
  m_mouse_x = GET_X_LPARAM(lparam);
  m_mouse_y = GET_Y_LPARAM(lparam);
  OnKeyUpWin(kMouseMiddleButton, lparam);
}

void InputSystem::OnMouseRightButtonUpWin(unsigned wparam, unsigned lparam)
{
  m_mouse_x = GET_X_LPARAM(lparam);
  m_mouse_y = GET_Y_LPARAM(lparam);
  OnKeyUpWin(kMouseRightButton, lparam);
}

void InputSystem::OnSpacePushed()
{
  std::memset(m_keystates, 0, k_num_keys);
  std::memset(m_mousemove_states, 0, 4);
}
void InputSystem::OnChangeControllerConfig(int index)
{
  m_controller_config = index;
}

void InputSystem::OnQueryControllerConfig()
{
  g_engine_core->DispatchEvent(MessageBank::OnRecieveControllerConfig, m_controller_config);
}

}