// David Evans
// 2011/09/13
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "CollisionDetector.h"
#include "../Composition/Composition.h"
#include "../Graphics/DebugDraw.h"
#include "../Math/Vector3.h"
#include "../Math/Vector4.h"

namespace Improbability
{

bool BoxBoxCollision(RigidBodyComponent *a, RigidBodyComponent *b, Manifold *m)
{
  // Get info
  Vector3 rot_a = a->GetParent()->GetOrientation();
  Vector3 rot_b = b->GetParent()->GetOrientation();
  Vector3 pos_a = a->GetParent()->GetPosition().ToVector3();
  Vector3 pos_b = b->GetParent()->GetPosition().ToVector3();
  Vector3 size_a = a->GetSize() * 0.5f;
  Vector3 size_b = b->GetSize() * 0.5f;

  if (pos_a.m_x + size_a.m_x < pos_b.m_x - size_b.m_x)
    return false;
  if (pos_a.m_x - size_a.m_x > pos_b.m_x + size_b.m_x)
    return false;

  if (pos_a.m_y + size_a.m_y < pos_b.m_y - size_b.m_y)
    return false;
  if (pos_a.m_y - size_a.m_y > pos_b.m_y + size_b.m_y)
    return false;

  m->m_depth = 1.0f;
  m->m_normal = (pos_b - pos_a).Normalized();
  m->m_points[0] = pos_a;
  m->m_points[1] = pos_b;

  return true;
}

bool BoxCircleCollision(RigidBodyComponent *a, RigidBodyComponent *b, Manifold *m)
{
  return false;
}

bool CircleBoxCollision(RigidBodyComponent *a, RigidBodyComponent *b, Manifold *m)
{
  return false;
}

bool CircleCircleCollision(RigidBodyComponent *a, RigidBodyComponent *b, Manifold *m)
{
  return false;
}


}