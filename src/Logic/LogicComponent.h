// David Evans
// 2011/09/17
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Composition/Component.h"
#include "Action.h"
#include "ActionBank.h"

namespace Improbability
{

class Serializer;
class ActionList;
class MetaComposition;

class LogicComponent : public Component
{
public:
  static void BindMeta(MetaComposition*);

  LogicComponent() {}
  LogicComponent(LogicComponent const &rhs);

  // Inherited methods
  void Initialize();
  void Uninitialize();
  Component* Clone();
  void Serialize(Serializer &serializer) { }
  void Deserialize(Serializer &serializer);

  // Methods
  void Update(float dt);
  void AddActionList(char const *file);
  void AddActionList(ActionList *list);
  
  Action* PushFront(int action_id, float lifetime);
  Action* PushBack(int action_id, float lifetime);

  // This function is dangerous
  std::list<Action*> const& GetActionList() {return m_actions;}

private:
  String m_list_name;
  std::list<Action*> m_actions;
};

}