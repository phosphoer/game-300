// Nicholas Baldwin
// 2011/12/06
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "Texture.h"

namespace Improbability
{

class MetaComposition;

class TextureFromFile : public Resource
{
public:
  static void BindMeta(MetaComposition*);

  void Load(char const *filepath);

  void Unload() {delete m_texture;}

  Texture *GetTexture()
  {
    return m_texture;
  }

private:
  Texture *m_texture;
};

}