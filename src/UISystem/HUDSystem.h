// Jake Leonard
// 2011/09/27
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Core/System.h"
#include "../Composition/Composition.h"

namespace Improbability
{
  class UISystemInterface;
  class KeyboardEvent;
  class ActionEvent;
  class HUDItem;

  class HUDSystem : public System
  {
  public:
    static void BindMeta(MetaComposition*);

    // Inherited methods
    void InitializeGlobal() { }
    void InitializeInstance();
    void UpdateInstance(float dt);
    void UninitializeInstance();
    void UninitializeGlobal() { }
    void Copy(System *dest, bool deep) { }
    void Deserialize(Serializer &serializer);

    // messages to talk with cef
    void OnSpaceReady();
    void OnDeviceReset();
    void OnToggleDevConsole(bool showConsole);

    void DisplayHudItem(String name);
    bool IsHudItemDisplayed(String name);
    void UpdateHudText(String name, String text);
    void HideHudItem(String name);
    //void ChangeHudItemPos(String name, Vector2 newPos);

  private:
    UISystemInterface *m_system_interface;
    HashMap<String, HUDItem*> m_hints;
    bool m_devConsoleActive;
    int m_reset_device_count;
  };

}