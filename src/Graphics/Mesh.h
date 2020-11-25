// Nicholas Baldwin
// 2011/09/19
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "../Resource/Resource.h"

namespace Improbability
{

class Mesh : public Resource
{
public:
  virtual void Load(char const *filename) = 0;

  virtual void Unload() = 0;

  void Set();
  void Add(Matrix4 const &transform);
  void Draw();

  void* Lock(unsigned offset = 0, unsigned size = 0);

protected:
  unsigned m_mesh_handle;
  unsigned m_declaration_handle;
  unsigned m_num_tris;
  unsigned m_num_verts;
};

class MetaComposition;

class TexturedMesh : public Mesh
{
public:
  static void BindMeta(MetaComposition*);

  void Load(char const *filename);

  void Unload();

  struct Vertex
  {
    Vector3 m_position;
    Vector3 m_normal;
    Vector2 m_texture_coordinates;
  };

  Vertex* GetBuffer();
};

}