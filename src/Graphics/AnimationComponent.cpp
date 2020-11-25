// Nicholas Baldwin
// 2011/10/25
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "AnimationComponent.h"
#include "..\core\EngineCore.h"
#include "..\FileIO\Serializer.h"
#include "..\math\MatrixStack.h"
#include "..\math\matrix4.h"
#include "GraphicsDevice.h"
#include "..\Resource\ResourceManager.h"
#include "Skeleton.h"
#include "OgreMesh.h"
#include "..\composition\composition.h"
#include "Shader.h"
#include "OgreSkeleton.h"
#include "Renderable.h"

namespace Improbability
{

MakeMeta(AnimationComponent);

void AnimationComponent::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

void AnimationComponent::Initialize()
{
  m_skeleton = g_engine_core->GetResourceManager()->GetResource<OgreSkeleton>(m_skeleton_name)->GetSkeleton();
}

void AnimationComponent::Deserialize(Serializer &serializer)
{
  Read(serializer, m_skeleton_name, "Skeleton");
}

}