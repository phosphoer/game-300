// Nicholas Baldwin
// 2011/09/29
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "..\composition\component.h"

namespace Improbability
{

class Texture;
class TextureFromFile;

class UIComponent : public Component
{
public:
  void Initialize();
  void Uninitialize();
  Component* Clone() { return new UIComponent(*this); }

  void Serialize(Serializer &serializer) { }
  void Deserialize(Serializer &serializer);
  static void BindMeta(MetaComposition *comp);

public:
  void SetTexture(TextureFromFile *resource);
  void SetTexture(Texture *tex);
  Texture *GetTexture() { return m_texture; }

  static void StartDraw();

  void Draw(float width, float height);

  void SetStartPositionPixel(Vector2 start_pos);

  Vector2 GetSize() { return m_size; }
  void SetSize(Vector2 val) { m_size = val; }
  
private:
  Vector2 m_size;
  Texture *m_texture;
  static unsigned m_mesh;
  static unsigned m_declaration;
  static int m_references;

  String m_texture_name;
};

}