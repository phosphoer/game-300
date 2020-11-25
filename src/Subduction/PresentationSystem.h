// David Evans
// 2011/09/27
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Core/System.h"
#include "../Composition/Composition.h"

namespace Improbability
{

class KeyboardEvent;
class MouseEvent;
class MetaComposition;

class PresentationSystem : public System
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

  // Messages
  void OnObjectInitialized(CompositionId id);
  void OnObjectDeleted(CompositionId id);
  void OnSpaceReady();
  void OnKeyPress(KeyboardEvent);
  void OnMousePress(MouseEvent);
  void OnMouseRelease(MouseEvent);
  void OnDeviceReset();

private:
  void ResizeSlides();

  std::vector<String> m_slidenames;
  std::vector<CompositionId> m_slides;
};

}