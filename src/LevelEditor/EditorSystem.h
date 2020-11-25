// David Evans
// 2011/09/27
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Core/System.h"
#include "../Composition/Composition.h"

namespace Improbability
{

  class UISystemInterface;
  class KeyboardEvent;
  class MouseEvent;
  class MetaComposition;

  class EditorSystem : public System
  {
  public:
    static void BindMeta(MetaComposition*);

    // Inherited methods
    void InitializeGlobal() {}
    void InitializeInstance();
    void UpdateInstance(float dt);
    void UninitializeInstance();
    void UninitializeGlobal() {}
    void Copy(System *dest, bool deep) {}
    void Deserialize(Serializer &serializer) {}

    // Messages
    void OnSpaceReady();

    // JS Messages
    void OnEditorChangeTool(String toolName);
    void OnEditorObjClick(int selectedObject);
    void OnEditorReady();
    void OnEditorRequestObjects();

    // Object Messages
    void OnObjectInitialized(CompositionId id);
    void OnObjectDeleted(CompositionId id);

    // Input Messages
    void OnKeyPress(KeyboardEvent);
    void OnMousePress(MouseEvent);
    void OnMouseDrag(MouseEvent);

  private:
    String m_currentTool;
    UISystemInterface *m_system_interface;
    bool m_rotating;
    int m_reset_device_count;
    int m_selObj;
  };

}
