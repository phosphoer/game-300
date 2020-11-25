// Nicholas Baldwin
// 2011/12/07
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{
class Texture;
class TextureFromFile;

// effectively the same as the sprite component but not a component
class UIFrame
{
public:
  void Initialize();
  void Uninitialize();

public:
  void SetTexture(Texture *tex) { m_texture = tex; }
  Texture *GetTexture() { return m_texture; }

  void Draw(float width, float height);

  void SetPosition(Vector2 start_pos) { m_position = start_pos; }

  Vector2 GetSize() { return m_size; }
  void SetSize(Vector2 val) { m_size = val; }

private:
  Vector2 m_size;
  Vector2 m_position;
  Texture *m_texture;
  static unsigned m_mesh;
  static unsigned m_declaration;
  static int m_references;
};

}