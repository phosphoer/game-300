// Nicholas Baldwin
// 2011/10/29
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Renderable.h"
#include "..\FileIO\Serializer.h"
#include "Shader.h"
#include "Texture.h"
#include "..\core\EngineCore.h"
#include "..\Resource\ResourceManager.h"
#include "Mesh.h"
#include "GraphicsDevice.h"
#include "TextureFromFile.h"

namespace Improbability
{

MakeMeta(Renderable);

char Renderable::m_set_mesh_id = 0;
char Renderable::m_max_mesh_id = 1;
HashMap<String, char> Renderable::m_mesh_ids;
char Renderable::m_set_shader_id = 0;
char Renderable::m_max_shader_id = 1;
HashMap<String, char> Renderable::m_shader_ids;

void Renderable::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

void Renderable::Load(const char *filename)
{
  Serializer serializer(filename);

  Read(serializer, m_mesh, "Mesh");
  Read(serializer, m_vertex_shader, "VertexShader");
  Read(serializer, m_pixel_shader, "PixelShader");

  String uniform, type, s;
  int i; float f; Vector3 v3; Vector4 v4;
  if (serializer.EnterElement("Constants"))
  {
    for (serializer.Begin(); serializer.IsGood(); serializer.Iterate())
    {
      Read(serializer, uniform, "Name");
      serializer.EnterElement("Value");
      ReadAttribute(serializer, type, "type");

      if (type == "float")
      {
        Read(serializer, f);
        m_shader_constants[uniform] = Variable(f);

        if(uniform == "u_alpha_value")
          m_transparency = f;
      }
      else if (type == "int")
      {
        Read(serializer, i);
        m_shader_constants[uniform] = Variable(i);
      }
      else if (type == "Vector3")
      {
        Read(serializer, v3);
        m_shader_constants[uniform] = Variable(v3);
      }
      else if (type == "Vector4")
      {
        Read(serializer, v4);
        m_shader_constants[uniform] = Variable(v4);
      }
      else if (type == "Sampler")
      {
        Read(serializer, s);
        m_shader_constants[uniform] = Variable(s);
      }
      else
      {
        ErrorIf(true, "Uniform type not currently supported by Renderable: %s", type);
      } 
      serializer.LeaveElement();
    }
  }

  if (m_shader_ids.Find(m_vertex_shader + m_pixel_shader) == m_shader_ids.End())
  {
    m_shader_ids[m_vertex_shader + m_pixel_shader] = m_max_shader_id;
    ++m_max_shader_id;
  }
  if (m_mesh_ids.Find(m_mesh) == m_mesh_ids.End())
  {
    m_mesh_ids[m_mesh] = m_max_mesh_id;
    ++m_max_mesh_id;
  }

  m_shader_id = m_shader_ids[m_vertex_shader + m_pixel_shader];
  m_mesh_id = m_mesh_ids[m_mesh];
}

void Renderable::SetConstant( String const &name, float f )
{
  ErrorIf(m_shader_constants.Find(name) == m_shader_constants.End(), "Could not find uniform");
  ErrorIf(m_shader_constants[name].GetType() != "float", "Uniform of name %s is not a float", name.GetCString());

  m_shader_constants[name] = Variable(f);
}

void Renderable::SetConstant( String const &name, int i )
{
  ErrorIf(m_shader_constants.Find(name) == m_shader_constants.End(), "Could not find uniform");
  ErrorIf(m_shader_constants[name].GetType() != "int", "Uniform of name %s is not a int", name.GetCString());

  m_shader_constants[name] = Variable(i);
}

void Renderable::SetConstant( String const &name, Vector3 v )
{
  ErrorIf(m_shader_constants.Find(name) == m_shader_constants.End(), "Could not find uniform");
  ErrorIf(m_shader_constants[name].GetType() != "Vector3", "Uniform of name %s is not a Vector3", name.GetCString());

  m_shader_constants[name] = Variable(v);
}

void Renderable::SetConstant( String const &name, Vector4 v )
{
  ErrorIf(m_shader_constants.Find(name) == m_shader_constants.End(), "Could not find uniform");
  ErrorIf(m_shader_constants[name].GetType() != "Vector4", "Uniform of name %s is not a Vector4", name.GetCString());

  m_shader_constants[name] = Variable(v);
}

void Renderable::SetShader()
{
  if (m_set_shader_id == m_shader_id)
  {
    GraphicsDevice::CommitWorldMatrix();
    return;
  }

  // if there is an active renderable end it first
  if (m_set_shader_id != 0)
  {
    Shader::GetActiveVertexShader()->Unset();
    Shader::GetActivePixelShader()->Unset();
  }

  Shader *shader = Shader::GetShader(m_vertex_shader);
  shader->Set();

  shader = Shader::GetShader(m_pixel_shader);
  shader->Set();

  m_set_shader_id = m_shader_id;
}

void Renderable::SetConstants()
{
  Shader *vertex_shader = Shader::GetActiveVertexShader();
  Shader *pixel_shader  = Shader::GetActivePixelShader();

  ErrorIf(vertex_shader == 0 || pixel_shader == 0, "No active shader to set constants to");

  for (auto it = m_shader_constants.Begin(); it != m_shader_constants.End(); ++it)
  {
    String type = it->second.GetType();

    if (type == "float")
    {
      vertex_shader->SetConstant(it->first, static_cast<float>(it->second));
      pixel_shader->SetConstant(it->first, static_cast<float>(it->second));
    }
    else if (type == "int")
    {
      vertex_shader->SetConstant(it->first, static_cast<int>(it->second));
      pixel_shader->SetConstant(it->first, static_cast<int>(it->second));
    }
    else if (type == "Vector3")
    {
      vertex_shader->SetConstant(it->first, static_cast<Vector3>(it->second));
      pixel_shader->SetConstant(it->first, static_cast<Vector3>(it->second));
    }
    else if (type == "Vector4")
    {
      vertex_shader->SetConstant(it->first, static_cast<Vector4>(it->second));
      pixel_shader->SetConstant(it->first, static_cast<Vector4>(it->second));
    }
    else if (type == "String")
    {
      Texture *tex = g_engine_core->GetResourceManager()->GetResource<TextureFromFile>(static_cast<String>(it->second))->GetTexture();

      tex->Set(it->first);
    }
  }

  if (m_transparency < 1.0f)
  {
    GraphicsDevice::SetAlphaEnable(true);
  }
  else
  {
    GraphicsDevice::SetAlphaEnable(false);
  }
}

void Renderable::SetMesh()
{  
  if (m_set_mesh_id == m_mesh_id)
    return;

  Mesh *mesh = g_engine_core->GetResourceManager()->GetResource<Mesh>(m_mesh);
  mesh->Set();

  m_set_mesh_id = m_mesh_id;
}

void Renderable::Clear()
{
  // cleans up when all draw calls are finished
  if(m_set_shader_id)
  {
    Shader::GetActivePixelShader()->Unset();
    Shader::GetActiveVertexShader()->Unset();
  }

  m_set_shader_id = 0;
  m_set_mesh_id = 0;
}

void Renderable::Add(Matrix4 const &transform)
{
  Mesh *mesh = g_engine_core->GetResourceManager()->GetResource<Mesh>(m_mesh);

  mesh->Add(transform);
}

void Renderable::Draw()
{
  SetShader();
  SetConstants();
  SetMesh();

  Mesh *mesh = g_engine_core->GetResourceManager()->GetResource<Mesh>(m_mesh);

  mesh->Draw();
}

float Renderable::GetDepth()
{
  Matrix4 model_view = GraphicsDevice::GetViewStack().Top() * GraphicsDevice::GetWorldStack().Top();

  Vector4 origin = model_view * Vector4(0.0f, 0.0f, 0.0f, 1.0f);

  return origin.m_z;
}

}