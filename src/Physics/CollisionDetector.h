// David Evans
// 2011/09/13
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "RigidBodyComponent.h"

namespace Improbability
{

class Manifold
{
public:
  float m_depth;
  Vector3 m_normal;
  Vector3 m_points[2];
};

bool BoxBoxCollision(RigidBodyComponent *a, RigidBodyComponent *b, Manifold *m);
bool BoxCircleCollision(RigidBodyComponent *a, RigidBodyComponent *b, Manifold *m);
bool CircleBoxCollision(RigidBodyComponent *a, RigidBodyComponent *b, Manifold *m);
bool CircleCircleCollision(RigidBodyComponent *a, RigidBodyComponent *b, Manifold *m);

}