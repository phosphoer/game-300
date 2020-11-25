// Nicholas Baldwin
// 2011/10/29
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "..\Resource\Resource.h"
#include "..\math\Vector3.h"
#include "..\math\Vector4.h"
#include "Texture.h"
#include "Drawable.h"

namespace Improbability
{

class Renderable : public Resource, public Drawable
{
public:
  static void BindMeta(MetaComposition *comp);

  void Load(const char *filename);
  void Unload() {}

  unsigned GetShaderID() {return m_shader_id; }
  unsigned GetMeshID() {return m_mesh_id; }
  static unsigned GetSetShaderID() {return m_set_shader_id; }
  static unsigned GetSetMeshID() {return m_set_mesh_id; }

  void SetConstant(String const &name, float f);
  void SetConstant(String const &name, int i);
  void SetConstant(String const &name, Vector3 v);
  void SetConstant(String const &name, Vector4 v);

  void SetShader();
  void SetConstants();
  void SetMesh();

  static void Clear();

  void Add(Matrix4 const &transform);
  void Draw();

  float GetDepth();

private:
  String m_mesh, m_vertex_shader, m_pixel_shader;
  HashMap<String, Variable> m_shader_constants;

  static HashMap<String, char> m_shader_ids;
  static char m_max_shader_id;
  static char m_set_shader_id;
  static HashMap<String, char> m_mesh_ids;
  static char m_max_mesh_id;
  static char m_set_mesh_id;

  char m_shader_id;
  char m_mesh_id;

  float m_transparency;
};

}