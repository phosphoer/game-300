// Nicholas Baldwin
// 2011/10/25
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "..\composition\component.h"

namespace Improbability
{

class Renderable;
class Skeleton;
class Serializer;
class MetaComposition;

class AnimationComponent : public Component
{
public:
  void Initialize();
  void Uninitialize() {}
  Component* Clone() { return new AnimationComponent(*this); }

  void Deserialize(Serializer &serializer);
  void Serialize(Serializer &serializer) { }

  static void BindMeta(MetaComposition*);

  Skeleton& GetSkeleton() {return *m_skeleton;}

private:
  String m_skeleton_name;

  Skeleton *m_skeleton;
};

}