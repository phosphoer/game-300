// Nicholas Baldwin
// 2011/10/25
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "OgreSkeleton.h"
#include "..\FileIO\Serializer.h"
#include "..\core\EngineCore.h"
#include "Skeleton.h"

namespace Improbability
{

MakeMeta(OgreSkeleton)

void OgreSkeleton::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

void OgreSkeleton::Load(char const *filename)
{
  Serializer skeleton_file(filename);

  m_skeleton = new Skeleton;

  m_skeleton->Deserialize(skeleton_file);

  g_engine_core->DispatchEvent(MessageBank::OnAnimationInitialize, String(filename));
}

}