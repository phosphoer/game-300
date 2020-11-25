// David Evans
// 2011/07/20
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "FilepathResource.h"

namespace Improbability
{

MakeMeta(FilepathResource);

void FilepathResource::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

void FilepathResource::Load(char const *filepath)
{
  m_filepath = filepath;
}

void FilepathResource::Unload()
{
}

}