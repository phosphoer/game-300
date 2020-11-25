// David Evans
// 2011/06/30
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "TextResource.h"
#include "../FileIO/FileIO.h"

namespace Improbability
{

MakeMeta(TextResource);

void TextResource::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

void TextResource::Load(char const *filepath)
{
  m_buffer = ReadFromFile(filepath, m_size);
}

void TextResource::Unload()
{
  delete[] m_buffer;
}

}