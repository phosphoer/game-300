// Nicholas Baldwin
// 2012/06/05
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "..\Composition\Component.h"
#include "GraphicsHandleTypes.h"
#include "Drawable.h"

namespace Improbability
{

class Camera;

class LightComponent : public Component, public Drawable
{
public:
  static void BindMeta(MetaComposition *comp);

  void Initialize();
  void Uninitialize();
  Component* Clone();
  void Serialize(Serializer &serializer) { }
  void Deserialize(Serializer &serializer);

  void Update(float dt, unsigned axis);

  void Draw();
  unsigned GetDrawOrder(Camera const * cam);
  Matrix4 GetWorldTransform();

private:
  DeclarationHandle m_declaration;
  BufferHandle m_mesh;
  unsigned m_axis;
  bool m_draw;

  Vector4 m_diffuse_color;
  float m_radius;
};

}