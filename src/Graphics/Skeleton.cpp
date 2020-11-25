// Nicholas Baldwin
// 2011/10/17
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Skeleton.h"
#include "..\FileIO\Serializer.h"
#include "Shader.h"
#include "..\core\EngineCore.h"

namespace Improbability
{

enum InterpolationMethod
{
  IM_Lerp = 0,
  IM_Slerp,
  IM_Ilerp
};

static const InterpolationMethod k_interpolation_method = IM_Lerp;

MakeMeta(Skeleton);

void Skeleton::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

void Skeleton::Deserialize(Serializer &serializer)
{
  String name;
  int id, max_id = 0;
  float a, x, y, z;

  HashMap<String, int> bone_indeces;
  HashMap<int, Bone> bones;

  serializer.EnterElement("bones");

  for (serializer.Begin(); serializer.IsGood(); serializer.Iterate())
  {
    Bone new_bone;

    new_bone.m_parent_index = static_cast<unsigned>(-1);

    new_bone.m_inverse_bind_pose.Identity();
    new_bone.m_transform.Identity();

    ReadAttribute(serializer, name, "name");
    ReadAttribute(serializer, id, "id");

    bone_indeces[name] = id;

    if (max_id < id)
      max_id = id;

    serializer.EnterElement("position");
    ReadAttribute(serializer, new_bone.m_inverse_bind_pose.m_vector.m_x, "x");
    ReadAttribute(serializer, new_bone.m_inverse_bind_pose.m_vector.m_y, "y");
    ReadAttribute(serializer, new_bone.m_inverse_bind_pose.m_vector.m_z, "z");
    serializer.LeaveElement();

    serializer.EnterElement("rotation");
    ReadAttribute(serializer, a, "angle");
    serializer.EnterElement("axis");
    ReadAttribute(serializer, x, "x");
    ReadAttribute(serializer, y, "y");
    ReadAttribute(serializer, z, "z");
    serializer.LeaveElement();
    serializer.LeaveElement();

    new_bone.m_inverse_bind_pose.m_quaternion.FromAxisAngle(Vector3(x, y, z), a);

    bones[id] = new_bone;
  }
  serializer.LeaveElement();
  serializer.LeaveElement();

  String parent, child;

  serializer.EnterElement("bonehierarchy");

  for (serializer.Begin(); serializer.IsGood(); serializer.Iterate())
  {
    ReadAttribute(serializer, parent, "parent");
    ReadAttribute(serializer, child, "bone");

    bones[bone_indeces[child]].m_parent_index = bone_indeces[parent];
  }
  serializer.LeaveElement();
  serializer.LeaveElement();

  for (int i = 0; i <= max_id; ++i)
  {
    if (bones[i].m_parent_index != static_cast<unsigned>(-1))
    {
      bones[i].m_inverse_bind_pose = m_bones[bones[i].m_parent_index].m_inverse_bind_pose * 
                                     bones[i].m_inverse_bind_pose;
    }

    m_bones.push_back(bones[i]);
  }
}

void Skeleton::Set(String const &uniform_name)
{
  std::vector<Matrix4> matrices;
  FillTransforms(matrices);

  Shader::GetActiveVertexShader()->SetConstant(uniform_name, &(matrices[0]), matrices.size());
}

void Skeleton::FillTransforms(std::vector<Matrix4> &matrices)
{
  matrices.clear();

  for (unsigned i = 0; i < m_bones.size(); ++i)
  {
    if (m_bones[i].m_parent_index != static_cast<unsigned>(-1))
    {
      VQS new_transform;

      new_transform = m_bones[i].m_inverse_bind_pose;
      new_transform = (m_bones[m_bones[i].m_parent_index].m_transform * 
                       m_bones[i].m_transform) * new_transform;
      new_transform = m_bones[i].m_inverse_bind_pose.Inversed() * new_transform;

      matrices.push_back(new_transform.ToMatrix());
    }
    else
    {
      VQS new_transform;

      new_transform = m_bones[i].m_inverse_bind_pose;
      new_transform = (m_bones[i].m_transform) * new_transform;
      new_transform = m_bones[i].m_inverse_bind_pose.Inversed() * new_transform;

      matrices.push_back(new_transform.ToMatrix());
    }
  }
}

void Skeleton::UpdateBone(unsigned bone_index, VQS const &start, VQS const &end, float t)
{
  if (k_interpolation_method == IM_Lerp)
  {
    m_bones[bone_index].m_transform = start * (1.0f - t) + end * t;
    m_bones[bone_index].m_transform.m_quaternion.Normalize();
  }
  else if (k_interpolation_method == IM_Slerp)
  {
    m_bones[bone_index].m_transform.m_vector = start.m_vector * (1.0f - t) + end.m_vector * t;
    m_bones[bone_index].m_transform.m_scale = start.m_scale * (1.0f - t) + end.m_scale * t;
    
    float alpha = std::acos(start.m_quaternion.Dot(end.m_quaternion));
    float sin_alpha = std::sin(alpha);

    m_bones[bone_index].m_transform.m_quaternion = start.m_quaternion * (std::sin(alpha - t * alpha) / sin_alpha) +
                                                   end.m_quaternion * (std::sin(t * alpha) / sin_alpha);
  }
}

}