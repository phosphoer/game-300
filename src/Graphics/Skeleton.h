// Nicholas Baldwin
// 2011/10/16
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "..\math\VQS.h"
#include "..\Resource\Resource.h"

namespace Improbability
{

class Serializer;

class Skeleton
{
public:
  static void BindMeta(MetaComposition*);

  void Deserialize(Serializer &serializer);
  void Set(String const &uniform_name);

  void FillTransforms(std::vector<Matrix4> &matrices);

  void UpdateBone(unsigned bone_index, VQS const &start, VQS const &end, float t);

private:
  struct Bone
  {
    unsigned m_parent_index;
    VQS m_inverse_bind_pose;
    VQS m_transform;
  };

  std::vector<Bone> m_bones;
};

}