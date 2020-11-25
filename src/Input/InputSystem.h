// David Evans
// 2011/09/07
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Core/System.h"
#include "../Core/Space.h"

namespace Improbability
{

class InputActions;
class MetaComposition;

class InputSystem : public System
{
public:
  static void BindMeta(MetaComposition*);

  // Inherited methods
  void InitializeGlobal();
  void InitializeInstance();
  void UpdateInstance(float dt);
  void UninitializeInstance();
  void UninitializeGlobal();
  void Copy(System *dest, bool deep);
  void Deserialize(Serializer &serializer);

  // Listeners for messages from the window system
  // to human-ify input data
  void OnCharDownWin(unsigned wparam, unsigned lparam);
  void OnKeyDownWin(unsigned wparam, unsigned lparam);
  void OnKeyUpWin(unsigned wparam, unsigned lparam);
  void OnMouseMoveWin(unsigned wparam, unsigned lparam);
  void OnMouseScrollWin(unsigned wparam, unsigned lparam);
  void OnMouseLeftButtonDownWin(unsigned wparam, unsigned lparam);
  void OnMouseMiddleButtonDownWin(unsigned wparam, unsigned lparam);
  void OnMouseRightButtonDownWin(unsigned wparam, unsigned lparam);
  void OnMouseLeftButtonUpWin(unsigned wparam, unsigned lparam);
  void OnMouseMiddleButtonUpWin(unsigned wparam, unsigned lparam);
  void OnMouseRightButtonUpWin(unsigned wparam, unsigned lparam);
  void OnSpacePushed();
  void OnChangeControllerConfig(int index);
  void OnQueryControllerConfig();
  static int const k_num_mousekeys = 7;
  static int const k_num_keys = 256;

private:
  int m_keystates[k_num_keys];
  int m_mousemove_states[4];
  int m_mouse_x;
  int m_mouse_y;
  int m_mouse_move_x;
  int m_mouse_move_y;
  int m_mouse_scroll;
  int m_mouse_scroll_move;
  bool m_mouse_drag;

  std::vector<String> m_actions_files;
  std::vector<InputActions*> m_actions_vector;
  static unsigned m_controller_config;
};

}