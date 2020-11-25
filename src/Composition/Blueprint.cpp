// David Evans
// 2011/07/19
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Blueprint.h"
#include "Composition.h"
#include "../FileIO/Serializer.h"
#include "../FileIO/FileIO.h"

namespace Improbability
{

MakeMeta(Blueprint);

void Blueprint::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

void Blueprint::Load(char const *filepath)
{
  Serializer loader(filepath);

  m_filename = GetFilenameFromPath(filepath);
  m_composition = new Composition();
  Read(loader, *m_composition);
}

void Blueprint::Unload()
{
  delete m_composition;
}

}