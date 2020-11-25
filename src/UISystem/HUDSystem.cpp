// Jake Leonard
// 2011/09/27
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "precompiled.h"
#include "HUDSystem.h"
#include "../Core/EngineCore.h"
#include "../Composition/ObjectManager.h"
#include "../Input/InputEvent.h"
#include "../FileIO/Serializer.h"
#include "../Graphics/Texture.h"
#include "../Graphics/SpriteComponent.h"
#include "../Graphics/GraphicsDevice.h"

namespace Improbability
{
  class HUDItem
  {
  public:
    HUDItem() : m_visible(false) { }
    bool IsVisible() { return m_visible; }
  private:
    friend class HUDSystem;
    String m_name, m_css, m_text;
    Vector2 m_pos;
    float m_width, m_height;
    bool m_visible;
  };

  MakeMeta(HUDSystem);

  void HUDSystem::BindMeta(MetaComposition *comp)
  {
    comp->MetaAddComponent(MetaNewable);
    comp->MetaAddComponent(MetaCallable);

    /* UI System stuff */ 
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnToggleDevConsole);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnSpaceReady);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnDeviceReset);

    /* HUD specific funcs */
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(DisplayHudItem);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(IsHudItemDisplayed);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(UpdateHudText);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(HideHudItem);
    //comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(ChangeHudItemPos);
  }

  void HUDSystem::InitializeInstance()
  {
    m_system_interface = new UISystemInterface();
    m_devConsoleActive = false;
    m_reset_device_count = g_reset_device_count;

    // Register listeners
    g_engine_core->AddEventListener(MessageBank::OnToggleDevConsole, this);
    g_engine_core->AddEventListener(MessageBank::OnSpaceReady, this);
    g_engine_core->AddEventListener(MessageBank::OnDeviceReset, this);

    // Hud listeners
    g_engine_core->AddEventListener(MessageBank::DisplayHudItem, this);
    g_engine_core->AddEventListener(MessageBank::IsHudItemDisplayed, this);
    g_engine_core->AddEventListener(MessageBank::UpdateHudText, this);
    g_engine_core->AddEventListener(MessageBank::HideHudItem, this);
    //g_engine_core->AddEventListener(MessageBank::ChangeHudItemPos, this);
  }

  void HUDSystem::UpdateInstance(float dt)
  {
    if (m_reset_device_count != g_reset_device_count)
    {
      m_system_interface->ResizeView(GraphicsDevice::GetDeviceWidth(), GraphicsDevice::GetDeviceHeight());

      m_reset_device_count = g_reset_device_count;
    }

    m_system_interface->Update();

    if(!m_devConsoleActive)
      m_system_interface->Render();
  }

  void HUDSystem::UninitializeInstance()
  {
    // Remove event listeners
    g_engine_core->RemoveEventListener(MessageBank::OnToggleDevConsole, this);
    g_engine_core->RemoveEventListener(MessageBank::OnSpaceReady, this);
    g_engine_core->RemoveEventListener(MessageBank::OnDeviceReset, this);

    // Hud event listeners
    g_engine_core->RemoveEventListener(MessageBank::DisplayHudItem, this);
    g_engine_core->RemoveEventListener(MessageBank::IsHudItemDisplayed, this);
    g_engine_core->RemoveEventListener(MessageBank::UpdateHudText, this);
    g_engine_core->RemoveEventListener(MessageBank::HideHudItem, this);
    //g_engine_core->RemoveEventListener(MessageBank::ChangeHudItemPos, this);

    for(auto it = m_hints.Begin(); it != m_hints.End(); ++it)
      delete it->second;

    m_system_interface->DestroyView();
    delete m_system_interface;
  }

  void HUDSystem::Deserialize(Serializer &serializer)
  {
    for (serializer.Begin(); serializer.IsGood(); serializer.Iterate())
    {
      HUDItem* newItem = new HUDItem();
      Read(serializer, newItem->m_name, "Name");
      Read(serializer, newItem->m_pos, "Position");
      Read(serializer, newItem->m_css, "CSSAttr");
      Read(serializer, newItem->m_text, "Text");
      m_hints[newItem->m_name] = newItem;
    }
    serializer.LeaveElement();
  }

  void HUDSystem::OnDeviceReset()
  {
    m_system_interface->ResizeView(GraphicsDevice::GetDeviceWidth(), GraphicsDevice::GetDeviceHeight());
  }

  void HUDSystem::OnSpaceReady()
  {
    m_system_interface->CreateView(GraphicsDevice::GetDeviceWidth(), GraphicsDevice::GetDeviceHeight());
    m_system_interface->LoadHTML("hud.html");
    m_system_interface->SetVisible(true);
  }

  void HUDSystem::OnToggleDevConsole(bool showConsole)
  {
    #ifdef _DEBUG
    m_system_interface->SetVisible(!showConsole);
    m_devConsoleActive = showConsole;
    #endif
  }

  void HUDSystem::DisplayHudItem(String name)
  {
    ErrorIf(m_hints.Find(name) == m_hints.End(), "Hint does not exist");

    HUDItem* item = m_hints[name];
    /* Make the item displayed on the screen. */
    g_engine_core->DispatchEvent("HUDCreate", name, item->m_css, item->m_text);
    g_engine_core->DispatchEvent("HUDMove", name, item->m_pos.m_x, item->m_pos.m_y);
    //g_engine_core->DispatchEvent("HUDSize", name, item->m_width, item->m_height);
    item->m_visible = true;
  }

  bool HUDSystem::IsHudItemDisplayed(String name)
  {
    ErrorIf(m_hints.Find(name) == m_hints.End(), "Hint does not exist");
    return m_hints[name]->IsVisible();
  }

  void HUDSystem::HideHudItem(String name)
  {
    ErrorIf(m_hints.Find(name) == m_hints.End(), "Hint does not exist");

    HUDItem* item = m_hints[name];
    if(item->IsVisible() == false)
      return;

    g_engine_core->DispatchEvent("HUDClose", name);
    item->m_visible = false;
  }

  //void HUDSystem::ChangeHudItemPos(String name, Vector2 newPos)
  //{
  //  ErrorIf(m_hints.Find(name) == m_hints.End(), "Hint does not exist");

  //  HUDItem* item = m_hints[name];

  //  if(item->IsVisible() == false)
  //    return;

  //  g_engine_core->DispatchEvent("HUDMove", name, item->m_pos.m_x, item->m_pos.m_y);
  //}

  void HUDSystem::UpdateHudText(String name, String text)
  {
    ErrorIf(m_hints.Find(name) == m_hints.End(), "Hint does not exist");

    HUDItem* item = m_hints[name];

    if(item->IsVisible() == false)
      return;

     g_engine_core->DispatchEvent("HUDTextUpdate", name, text);
  }

}
