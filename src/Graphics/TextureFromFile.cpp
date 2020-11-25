// Nicholas Baldwin
// 2011/12/06
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "TextureFromFile.h"

namespace Improbability
{

MakeMeta(TextureFromFile);

void TextureFromFile::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

void TextureFromFile::Load(char const *filepath)
{
  m_texture = new Texture(filepath);
}

}