// David Evans
// 2011/09/10
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Composition/Component.h"
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"

namespace Improbability
{

class Serializer;
class MetaComposition;

class CollisionType
{
public:
  enum Type
  {
    Box,
    Circle,
    Count
  };
};

class RigidBodyComponent : public Component
{
public:
  static void BindMeta(MetaComposition*);

  RigidBodyComponent();
  void Initialize();
  void Uninitialize();
  Component* Clone();
  void Deserialize(Serializer &serializer);

  CollisionType::Type GetType() {return m_type;}
  Vector3 GetSize() {return m_size;}

private:
  CollisionType::Type m_type;
  Vector3 m_size;
};

}