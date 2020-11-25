// David Evans
// 2011/09/13
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Composition/Component.h"
#include "../Math/Vector4.h"
#include "../Input/InputEvent.h"

#include "TileSystem.h"

namespace Improbability
{

class Serializer;
class MetaComposition;

class TileController : public Component
{
public:
  static void BindMeta(MetaComposition*);

  TileController();
  void Initialize();
  void Uninitialize();
  Component* Clone();
  void Serialize(Serializer &serializer) { }
  void Deserialize(Serializer &serializer);

  void OnActionPress(ActionEvent);
  void OnActionRelease(ActionEvent);

private:
  float m_speed;
  float m_jump;
};

}