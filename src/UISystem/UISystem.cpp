// David Evans
// 2011/09/27
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "UISystem.h"
#include "../Core/EngineCore.h"
#include "../Composition/ObjectManager.h"
#include "../Input/InputEvent.h"
#include "../FileIO/Serializer.h"
#include "../Graphics/Texture.h"
#include "../Graphics/SpriteComponent.h"
#include "../Graphics/GraphicsDevice.h"

namespace Improbability
{

MakeMeta(UISystem);

void UISystem::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnToggleDevConsole);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnSpaceReady);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnDeviceReset);
}

// Inherited methods
void UISystem::InitializeGlobal()
{
}

void UISystem::InitializeInstance()
{
  m_system_interface = new UISystemInterface();

  // Register listeners
  g_engine_core->AddEventListener(MessageBank::OnToggleDevConsole, this);
  g_engine_core->AddEventListener(MessageBank::OnSpaceReady, this);
  g_engine_core->AddEventListener(MessageBank::OnDeviceReset, this);

  m_cursor_count = 0;
  m_reset_device_count = g_reset_device_count;

  while (ShowCursor(true) < 0)
  {
    m_cursor_count++;
  }
}

void UISystem::UpdateInstance(float dt)
{
  if (m_reset_device_count != g_reset_device_count)
  {
    m_system_interface->ResizeView(GraphicsDevice::GetDeviceWidth(), GraphicsDevice::GetDeviceHeight());

    m_reset_device_count = g_reset_device_count;
  }

  m_system_interface->Update();
  m_system_interface->Render();
}

void UISystem::UninitializeInstance()
{
  // Remove event listeners
  g_engine_core->RemoveEventListener(MessageBank::OnToggleDevConsole, this);
  g_engine_core->RemoveEventListener(MessageBank::OnSpaceReady, this);
  g_engine_core->RemoveEventListener(MessageBank::OnDeviceReset, this);

  while (m_cursor_count--)
  {
    ShowCursor(false);
  }

  m_system_interface->DestroyView();
  delete m_system_interface;
}

void UISystem::UninitializeGlobal()
{
}

void UISystem::Copy(System *dest, bool deep)
{
}

void UISystem::Deserialize(Serializer &serializer)
{
  String url;
  if (Read(serializer, url, "StartURL"))
  {
    m_start_string = url;
    m_start_string_is_url = true;
  }
  else if (Read(serializer, url, "StartHTML"))
  {
    m_start_string = url;
    m_start_string_is_url = false;
  }
}

void UISystem::OnDeviceReset()
{
  m_system_interface->ResizeView(GraphicsDevice::GetDeviceWidth(), GraphicsDevice::GetDeviceHeight());
}

void UISystem::OnSpaceReady()
{
  m_system_interface->CreateView(GraphicsDevice::GetDeviceWidth(), GraphicsDevice::GetDeviceHeight());

  if (m_start_string.GetSize() != 0)
  {
    if (m_start_string_is_url)
      m_system_interface->LoadURL(m_start_string);
    else
      m_system_interface->LoadHTML(m_start_string);
  }

  m_system_interface->SetVisible(true);
}

void UISystem::OnToggleDevConsole(bool showConsole)
{
#ifdef _DEBUG
  if(showConsole)
  {
    m_system_interface->LoadHTML("devconsole.html");
  }
  else
    m_system_interface->LoadHTML(m_start_string);
#endif
}

}