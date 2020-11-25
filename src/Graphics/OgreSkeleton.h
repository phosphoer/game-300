// Nicholas Baldwin
// 2011/10/25
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "..\Resource\Resource.h"

namespace Improbability
{

class Skeleton;

class OgreSkeleton : public Resource
{
public:
  static void BindMeta(MetaComposition *comp);

  void Load(char const *filename);

  void Unload() {}

  Skeleton* GetSkeleton() { return m_skeleton; }

private:
  Skeleton *m_skeleton;
};

}