// David Evans
// 2011/09/27
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Core/System.h"
#include "../Composition/Composition.h"

namespace GooeyUI
{
  class UISystem;
}

namespace Improbability
{

class UISystemInterface;
class KeyboardEvent;
class ActionEvent;

class UISystem : public System
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

  // messages to talk with cef
  void OnSpaceReady();
  void OnDeviceReset();
  void OnToggleDevConsole(bool showConsole);

private:
  UISystemInterface *m_system_interface;

  String m_start_string;
  bool m_start_string_is_url;
  int m_cursor_count;
  int m_reset_device_count;
};

}