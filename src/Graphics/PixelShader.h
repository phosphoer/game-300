// Nicholas Baldwin
// 2012/05/30
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "..\Resource\Resource.h"
#include "GraphicsHandleTypes.h"

namespace Improbability
{
  
class PixelShader : public Resource
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
  void SetSampler(String const &sampler_name, unsigned t);
  void UnsetSampler(String const &sampler_name);

  static PixelShader *GetActiveShader() { return m_active_pixel_shader; }

  static PixelShader *GetShader(String const &name);

private:
  String m_name;

  PixelShaderHandle m_shader;

  bool m_alpha_enabled;
  bool m_depth_enabled;

  HashMap<String, PixelShaderConstantHandle> m_constants;

  static HashMap<String, PixelShader*> m_shaders;
  static PixelShader *m_active_pixel_shader;
};

}