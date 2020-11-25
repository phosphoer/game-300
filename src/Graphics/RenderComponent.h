// Nicholas Baldwin
// 2011/06/27
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../composition/component.h"

namespace Improbability
{

class MetaComposition;
class Renderable;

class RenderComponent : public Component
{
public:
  static void BindMeta(MetaComposition*);

  void Initialize();
  void Uninitialize();
  void Serialize(Serializer &serializer) { }
  void Deserialize(Serializer &serializer);
  Component* Clone() {return new RenderComponent(*this);}

  void Draw();

  float GetDepth();
  Renderable* GetRenderable() {return m_renderable;}

private:
  String m_renderable_name;

  Renderable *m_renderable;
};

}