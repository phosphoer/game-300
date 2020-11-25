// Nicholas Baldwin
// 2011/08/21
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "precompiled.h"
#include "../Resource/Resource.h"
#include "../Containers/HashMap.h"
#include "GraphicsHandleTypes.h"

namespace Improbability
{

class Texture
{
public:
  static void BindMeta(MetaComposition* comp);
  // constructs a texture, either from a file or a blank texture of a certain size
  // filepath - the file to load the texture from
  // width - the width of the texture to create
  // height - the height of the texture to create
  Texture(String const &filepath);
  Texture(unsigned width, unsigned height);
  Texture() {}

  // copy constructor, does reference counting
  // original - the texture being copy constructed from
  Texture(Texture const &original);

  // assignment operator, does reference counting
  // rhs - the texture being assigned from
  Texture& operator=(Texture const &rhs);

  // destructs a texture, subtracts it's reference if no more references are held
  // it deletes the texture
  ~Texture();

  void Fill(unsigned char *color_buffer);

  unsigned GetWidth() { return m_width; }
  unsigned GetHeight() { return m_height; }

  void Set(String const &m_uniform_name);
  void Unset(String const &m_uniform_name);
private:
  TextureHandle m_texture_handle;
  unsigned m_width, m_height;
  unsigned m_mipmap_levels;


  static HashMap<unsigned, unsigned> m_references;
};

}
