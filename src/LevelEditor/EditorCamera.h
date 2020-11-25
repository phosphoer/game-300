// David Evans
// 2011/09/13
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Composition/Component.h"

namespace Improbability
{

class Serializer;
class MouseEvent;
class MetaComposition;

class EditorCamera : public Component
{
public:
  static void BindMeta(MetaComposition*);

  EditorCamera();
  void Initialize();
  void Uninitialize();
  Component* Clone();
  void Serialize(Serializer &serializer) { }
  void Deserialize(Serializer &serializer);

  void OnMouseDown(MouseEvent);
  void OnMouseScroll(MouseEvent);

private:
  void Pan(float x, float y);
  void Orbit(float x, float y);
};

}