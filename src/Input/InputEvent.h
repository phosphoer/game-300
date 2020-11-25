// David Evans
// 2011/09/07
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "InputKeys.h"

namespace Improbability
{

class ActionEvent
{
public:
  static void BindMeta(MetaComposition *comp);
  String m_name;
  int const *m_keystates;
};

class MouseEvent
{
public:
  unsigned m_buttoncode;
  int m_x;
  int m_y;
  int m_x_move;
  int m_y_move;
  int m_scroll;
  int m_scroll_move;
  int const *m_keystates;
};

class KeyboardEvent
{
public:
  unsigned int m_keycode;
  unsigned int m_modifiers;
  int const *m_keystates;
};

}