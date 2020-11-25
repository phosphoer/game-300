// David Evans
// 2011/09/17
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Core/System.h"
#include "../Composition/Composition.h"
#include "../Containers/HashMap.h"

namespace Improbability
{

class LogicComponent;
class MetaComposition;

class LogicSystem : public System
{
public:
  typedef HashMap<CompositionId, LogicComponent*> ComponentMap;

  static void BindMeta(MetaComposition*);

  // Inherited methods
  void InitializeGlobal();
  void InitializeInstance();
  void UpdateInstance(float dt);
  void UninitializeInstance();
  void UninitializeGlobal();
  void Copy(System *dest, bool deep);
  void Deserialize(Serializer &serializer);

  // Messages
  void OnObjectInitialized(CompositionId id);
  void OnObjectDeleted(CompositionId id);

private:
  ComponentMap m_components;
};

}