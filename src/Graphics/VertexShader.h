// Nicholas Baldwin
// 2012/05/17
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "GraphicsHandleTypes.h"
#include "..\Resource\Resource.h"

namespace Improbability
{

class VertexShader : public Resource
{
public:
  static void BindMeta(MetaComposition*);

  void Load(char const *filename);

  void Unload();

  void Set();
  void Unset();

  void SetConstant(String const &constant_name, bool b);
  void SetConstant(String const &constant_name, int i);
  void SetConstant(String const &constant_name, float f);
  void SetConstant(String const &constant_name, Vector2 const &v);
  void SetConstant(String const &constant_name, Vector3 const &v);
  void SetConstant(String const &constant_name, Vector4 const &v);
  void SetConstant(String const &constant_name, Color const &c);
  void SetConstant(String const &constant_name, Matrix2 const &m);
  void SetConstant(String const &constant_name, Matrix3 const &m);
  void SetConstant(String const &constant_name, Matrix4 const &m);
  void SetConstant(String const &constant_name, Matrix3 const *m, unsigned count);
  void SetConstant(String const &constant_name, Matrix4 const *m, unsigned count);

  static VertexShader *GetActiveShader() { return m_active_vertex_shader; }

  static VertexShader *GetShader(String const &name);

private:
  String m_name;

  VertexShaderHandle m_shader;

  bool m_alpha_enabled;
  bool m_depth_enabled;

  HashMap<String, VertexShaderConstantHandle> m_constants;

  static HashMap<String, VertexShader*> m_shaders;
  static VertexShader *m_active_vertex_shader;
};

}