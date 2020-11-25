// Jake Leonard
// 2012/09/13
// All content (c) 2012 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "EditorSystem.h"
#include "../Core/EngineCore.h"
#include "../Input/InputEvent.h"
#include "../Graphics/Camera.h"
#include "../Graphics/GraphicsDevice.h"
#include "../Resource/ResourceManager.h"

namespace Improbability
{

  MakeMeta(EditorSystem);

  void EditorSystem::BindMeta(MetaComposition *comp)
  {
    comp->MetaAddComponent(MetaNewable);
    comp->MetaAddComponent(MetaCallable);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnObjectInitialized);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnObjectDeleted);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnSpaceReady);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnKeyPress);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnMousePress);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnMouseDrag);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnEditorChangeTool);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnEditorObjClick);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnEditorReady);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnEditorRequestObjects);
  }

  void EditorSystem::InitializeInstance()
  {
    m_system_interface = new UISystemInterface();
    m_reset_device_count = g_reset_device_count;
    m_selObj = 0;

    // Register listeners
    g_engine_core->AddEventListener(MessageBank::OnObjectInitialized, this);
    g_engine_core->AddEventListener(MessageBank::OnObjectDeleted, this);
    g_engine_core->AddEventListener(MessageBank::OnSpaceReady, this);
    g_engine_core->AddEventListener(MessageBank::OnKeyPress, this);
    g_engine_core->AddEventListener(MessageBank::OnMousePress, this);
    g_engine_core->AddEventListener(MessageBank::OnMouseDrag, this);

    // JS Event Messages
    g_engine_core->AddEventListener(MessageBank::OnEditorChangeTool, this);
    g_engine_core->AddEventListener(MessageBank::OnEditorObjClick, this);
    g_engine_core->AddEventListener(MessageBank::OnEditorReady, this);
    g_engine_core->AddEventListener(MessageBank::OnEditorRequestObjects, this);
  }

  void EditorSystem::OnSpaceReady()
  {
    m_system_interface->CreateView(GraphicsDevice::GetDeviceWidth(), GraphicsDevice::GetDeviceHeight());
    m_system_interface->LoadHTML("editor.html");
    m_system_interface->SetVisible(true);

    g_engine_core->SystemCall("ObjectManager", "CreateObjectAtPosition", "editor-camera.bp", Vector3(2.0f, 2.0f, 8.0f));
  }

  void EditorSystem::OnEditorReady()
  {
    /* TODO: Send objects to javascript */
  }

  void EditorSystem::OnEditorRequestObjects()
  {
    Array<Resource*> const *resources = g_engine_core->GetResourceManager()->GetResourcesOfType(".bp");

    for (unsigned i = 0; i < resources->GetSize(); ++i)
      g_engine_core->DispatchEvent("OnEditorRecieveObject", (*resources)[i]->GetFileName());
  }

  void EditorSystem::UpdateInstance(float dt)
  {
    if (m_reset_device_count != g_reset_device_count)
    {
      m_system_interface->ResizeView(GraphicsDevice::GetDeviceWidth(), GraphicsDevice::GetDeviceHeight());
      m_reset_device_count = g_reset_device_count;
    }

    m_system_interface->Update();
    m_system_interface->Render();
  }

  void EditorSystem::UninitializeInstance()
  {
    g_engine_core->RemoveEventListener(MessageBank::OnObjectInitialized, this);
    g_engine_core->RemoveEventListener(MessageBank::OnObjectDeleted, this);
    g_engine_core->RemoveEventListener(MessageBank::OnSpaceReady, this);
    g_engine_core->RemoveEventListener(MessageBank::OnKeyPress, this);
    g_engine_core->RemoveEventListener(MessageBank::OnMousePress, this);
    g_engine_core->RemoveEventListener(MessageBank::OnMouseDrag, this);
    g_engine_core->RemoveEventListener(MessageBank::OnEditorChangeTool, this);
    g_engine_core->RemoveEventListener(MessageBank::OnEditorObjClick, this);
    g_engine_core->RemoveEventListener(MessageBank::OnEditorReady, this);

    m_system_interface->DestroyView();
    delete m_system_interface;
  }

  /************************************************************************/
  /* Messages                                                             */
  /************************************************************************/

  /** JS **/
  void EditorSystem::OnEditorChangeTool(String toolName) {  m_currentTool = toolName; }
  void EditorSystem::OnEditorObjClick(int selectedObject) { m_selObj = selectedObject; }

  /** Objects **/
  void EditorSystem::OnObjectInitialized(CompositionId id)
  {
    /* TODO: This */
  }

  void EditorSystem::OnObjectDeleted(CompositionId id)
  {
    /* TODO: This */
  }

  /** Input **/
  void EditorSystem::OnKeyPress(KeyboardEvent event)
  {
    /* TODO: Add more Hotkeys */
    switch(event.m_keycode)
    {
      case kKeyPlus:
      case kKeyNumPadAdd:
        /* TODO: Move forward through layers */
      break;
      case kKeyNumPadSubtract:
      case kKeyMinus:
        /* TODO: Move backward through layers */
      break;
      case kKeyP:
        /* TODO: Switch to Perspective */
      break;
      case kKeyO:
        /* TODO: Switch to Orthographic */
      break;
      case kKeyS:
        if(event.m_modifiers == kKeyShift)
        {
          /* TODO: Save */
        }
      break;

    }
  }

  void EditorSystem::OnMouseDrag(MouseEvent event)
  {
    /* This will give you the the start and end points of a mouse drag

      * m_x and m_y are the start points
      * m_x_move and m_y_move are the offsets from the start point
    */

    /* TODO: This */
  }

  void EditorSystem::OnMousePress(MouseEvent event)
  {
    /* Send the click to JS */
    if(m_system_interface->IsClickonHTML(event.m_x, event.m_y))
      return;

    /* TODO:
        * If in 2D:
          * Check the active tool
            * If eraser
              * Erase the block at this location
            * If selector
              * Paint block or place object at location
            * If Eyedropper
              * Set the selected object to what is at location
              * Send message back to JS on what the current object is now
        * If in 3D:
          *  If selected object, place it (dragging is handled by another event)
    */
  }

}
