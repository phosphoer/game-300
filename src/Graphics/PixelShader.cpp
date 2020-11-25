// Nicholas Baldwin
// 2012/05/30
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "PixelShader.h"
#include "GraphicsDevice.h"
#include "..\FileIO\Serializer.h"

namespace Improbability
{

HashMap<String, PixelShader*> PixelShader::m_shaders;
PixelShader *PixelShader::m_active_pixel_shader = 0;

MakeMeta(PixelShader);

void PixelShader::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

void PixelShader::Load(char const *filename)
{
  Serializer serializer(filename);

  String shader_file, constant_name;

  Read(serializer, m_name, "Name");

  m_shaders[m_name] = this;

  Read(serializer, shader_file, "File");

  String full_path = filename;
  int index = full_path.FindLastOf("\\");

  ErrorIf(index == full_path.k_end, "something is wrong with the directory...");

  String dir(full_path.GetCString(), 0, index+1);

  m_shader = GraphicsDevice::CreatePixelShader(dir + shader_file);

  serializer.EnterElement("Constants");
  for (serializer.Begin(); serializer.IsGood(); serializer.Iterate())
  {
    Read(serializer, constant_name);
    m_constants[constant_name] = GraphicsDevice::InitializePixelConstant(m_shader, constant_name);
  }
  serializer.LeaveElement();
  serializer.LeaveElement();

  // check render state values
  if (!Read(serializer, m_alpha_enabled, "AlphaEnabled"))
  {
    m_alpha_enabled = false;
  }
  if (!Read(serializer, m_depth_enabled, "DepthEnabled"))
  {
    m_depth_enabled = true;
  }
}

void PixelShader::Unload()
{
  GraphicsDevice::DeletePixelShader(m_shader);
}

void PixelShader::Set()
{
  // do nothing if this shader is already active
  if (m_active_pixel_shader == this)
    return;

  GraphicsDevice::SetAlphaEnable(m_alpha_enabled);
  GraphicsDevice::SetDepthEnable(m_depth_enabled);

  // unset any already active shader
  GraphicsDevice::UnsetPixelShader();

  GraphicsDevice::SetPixelShader(m_shader);
  m_active_pixel_shader = this;
}

void PixelShader::Unset()
{
  GraphicsDevice::UnsetPixelShader();
  m_active_pixel_shader = 0;
}

void PixelShader::SetConstant(String const &constant_name, bool b)
{
  if (m_active_pixel_shader == this)
    GraphicsDevice::SetPixelShaderConstant(m_constants[constant_name], b);
}

void PixelShader::SetConstant(String const &constant_name, int i)
{
  if (m_active_pixel_shader == this)
    GraphicsDevice::SetPixelShaderConstant(m_constants[constant_name], i);
}

void PixelShader::SetConstant(String const &constant_name, float f)
{
  if (m_active_pixel_shader == this)
    GraphicsDevice::SetPixelShaderConstant(m_constants[constant_name], f);
}

void PixelShader::SetConstant(String const &constant_name, Vector2 const &v)
{
  if (m_active_pixel_shader == this)
    GraphicsDevice::SetPixelShaderConstant(m_constants[constant_name], v);
}

void PixelShader::SetConstant(String const &constant_name, Vector3 const &v)
{
  if (m_active_pixel_shader == this)
    GraphicsDevice::SetPixelShaderConstant(m_constants[constant_name], v);
}

void PixelShader::SetConstant(String const &constant_name, Vector4 const &v)
{
  if (m_active_pixel_shader == this)
    GraphicsDevice::SetPixelShaderConstant(m_constants[constant_name], v);
}

void PixelShader::SetConstant(String const &constant_name, Color const &c)
{
  if (m_active_pixel_shader == this)
    GraphicsDevice::SetPixelShaderConstant(m_constants[constant_name], c);
}

void PixelShader::SetConstant(String const &constant_name, Matrix2 const &m)
{
  if (m_active_pixel_shader == this)
    GraphicsDevice::SetPixelShaderConstant(m_constants[constant_name], m);
}

void PixelShader::SetConstant(String const &constant_name, Matrix3 const &m)
{
  if (m_active_pixel_shader == this)
    GraphicsDevice::SetPixelShaderConstant(m_constants[constant_name], m);
}

void PixelShader::SetConstant(String const &constant_name, Matrix4 const &m)
{
  if (m_active_pixel_shader == this)
    GraphicsDevice::SetPixelShaderConstant(m_constants[constant_name], m);
}

void PixelShader::SetConstant(String const &constant_name, Matrix3 const *m, unsigned count)
{
  if (m_active_pixel_shader == this)
    GraphicsDevice::SetPixelShaderConstant(m_constants[constant_name], m, count);
}

void PixelShader::SetConstant(String const &constant_name, Matrix4 const *m, unsigned count)
{
  if (m_active_pixel_shader == this)
    GraphicsDevice::SetPixelShaderConstant(m_constants[constant_name], m, count);
}

void PixelShader::SetSampler(String const &sampler_name, unsigned t)
{
  if (m_active_pixel_shader == this)
    GraphicsDevice::SetPixelShaderSampler(m_constants[sampler_name], t);
}

void PixelShader::UnsetSampler(String const &sampler_name)
{
  if (m_active_pixel_shader == this)
    GraphicsDevice::UnsetPixelShaderSampler(m_constants[sampler_name]);
}

PixelShader* PixelShader::GetShader(String const &name)
{
  ErrorIf(m_shaders.Find(name) == m_shaders.End(), "Invalid Pixel Shader Name");
  return m_shaders[name];
}

}