// Nicholas Baldwin
// 2011/08/21
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Texture.h"
#include "GraphicsDevice.h"
#include "PixelShader.h"

namespace Improbability
{
HashMap<unsigned, unsigned> Texture::m_references;

MakeMeta(Texture);

void Texture::BindMeta(MetaComposition* comp)
{
  comp->MetaAddComponent(MetaNewable);
}

Texture::Texture(String const &filepath) : m_mipmap_levels(0)
{
  m_texture_handle = GraphicsDevice::CreateTexture(filepath);

  m_width = GraphicsDevice::GetTextureWidth(m_texture_handle);
  m_height = GraphicsDevice::GetTextureHeight(m_texture_handle);
  m_references[m_texture_handle] = 1;
}

Texture::Texture(unsigned width, unsigned height):
  m_width(width),
  m_height(height)
{
  m_texture_handle = GraphicsDevice::CreateTexture(width, height, 1, 
                       GraphicsOptionTypes::Usage(GraphicsOptionTypes::USG_Dynamic),
                       GraphicsInfoTypes::FMT_A8R8G8B8);

  m_references[m_texture_handle] = 1;
}

Texture::Texture(Texture const &original)
{
  m_width = original.m_width;
  m_height = original.m_height;
  m_mipmap_levels = original.m_mipmap_levels;
  m_texture_handle = original.m_texture_handle;

  ++m_references[m_texture_handle];
}

Texture& Texture::operator=(Texture const &rhs)
{
  --m_references[m_texture_handle];

  if (m_references[m_texture_handle] == 0)
    GraphicsDevice::DeleteTexture(m_texture_handle);

  m_width = rhs.m_width;
  m_height = rhs.m_height;
  m_mipmap_levels = rhs.m_mipmap_levels;
  m_texture_handle = rhs.m_texture_handle;

  ++m_references[m_texture_handle];

  return *this;
}

Texture::~Texture()
{
  --m_references[m_texture_handle];

  if (m_references[m_texture_handle] == 0)
    GraphicsDevice::DeleteTexture(m_texture_handle);
}

void Texture::Fill(unsigned char *color_buffer)
{
  unsigned pitch;
  char *texture_buffer = reinterpret_cast<char*>(GraphicsDevice::LockTexture(m_texture_handle, 0, pitch));

  for (unsigned i = 0; i < m_height; ++i)
  {
    memcpy(texture_buffer + (i * pitch), color_buffer + (i * m_width * 4), m_width * 4);
  }

  GraphicsDevice::UnlockTexture(m_texture_handle, 0);
}

void Texture::Set(String const &m_uniform_name)
{
  PixelShader *pixel_shader = PixelShader::GetActiveShader();

  if (pixel_shader)
  {
    pixel_shader->SetSampler(m_uniform_name, m_texture_handle);
  }
}

void Texture::Unset(String const &m_uniform_name)
{
  PixelShader *pixel_shader = PixelShader::GetActiveShader();

  if (pixel_shader)
  {
    pixel_shader->UnsetSampler(m_uniform_name);
  }
}

}
