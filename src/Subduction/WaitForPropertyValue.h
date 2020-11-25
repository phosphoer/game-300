// David Evans
// 2011/09/18
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Logic/Action.h"

namespace Improbability
{

class Serializer;
class MetaComposition;

class WaitForPropertyValue : public Action
{
public:
  static void BindMeta(MetaComposition*);

  // Inherited methods
  ActionReturn::Type Update(float dt);
  void Deserialize(Serializer &serializer);
  Action* Clone() {return new WaitForPropertyValue(*this);}

  // Messages
  void OnAxisChange(int old_axis, int new_axis);

  // Methods
  void SetField(String field) {m_field = field;}

private:
  String m_field;
  String m_component;
  String m_class_type;
  String m_type;
  union
  {
    int m_value_i;
    float m_value_f;
  };
};

}