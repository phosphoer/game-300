// Nicholas Baldwin
// 2011/10/25
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "..\core\system.h"
#include "..\composition\composition.h"
#include "..\Input\InputEvent.h"

namespace Improbability
{

class ModelViewerSystem : public System
{
public:
  static void BindMeta(MetaComposition *comp);

  void InitializeGlobal();
  void InitializeInstance();
  void UninitializeGlobal();
  void UninitializeInstance();

  void UpdateInstance(float dt);

  void Copy(System *dest, bool deep);

  void Deserialize(Serializer &serializer);

  void OnObjectInitialized(CompositionId id);
  void OnObjectDeleted(CompositionId id);
  void OnSpaceReady();
  void OnKeyPress(KeyboardEvent);
  void OnMousePress(MouseEvent);
  void OnMouseRelease(MouseEvent);

private:
  String m_model_name;
};

}