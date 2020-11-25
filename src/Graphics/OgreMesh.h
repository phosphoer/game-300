// Nicholas Baldwin
// 2011/10/25
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "Mesh.h"
#include "..\math\Vector3.h"

namespace Improbability
{

class OgreMesh : public Mesh
{
public:
  static void BindMeta(MetaComposition* comp);

  void Load(char const *filename);

  void Unload();

  struct Vertex
  {
    Vector3 m_position;
    Vector3 m_normal;
    Vector2 m_texture_coordinates;
    float m_bone;
  };

  Vertex* GetBuffer();
};

}