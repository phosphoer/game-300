// David Evans
// 2011/09/18
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "Action.h"
#include "../Math/Vector3.h"

namespace Improbability
{

class Serializer;
class MetaComposition;

class AnimatePropertyAction : public Action
{
public:
  static void BindMeta(MetaComposition*);

  AnimatePropertyAction();

  // Inherited methods
  ActionReturn::Type Update(float dt);
  void Deserialize(Serializer &serializer);
  Action* Clone() {return new AnimatePropertyAction(*this);}

  // Methods
  void SetField(char const *name) {m_field = name;}
  void SetComponent(char const *name) {m_component = name;}
  void SetTime(float time) {m_time = time;}

  void SetBegin(int begin) {m_begin_int = begin;}
  void SetBegin(float begin) {m_begin_float = begin;}

  void SetEnd(int end) {m_end_int = end;}
  void SetEnd(float end) {m_end_float = end;}

  int GetEndInt() {return m_end_int;}
  float GetEndFloat() {return m_end_float;}

private:
  String m_field;
  String m_component;
  bool m_relative;

  float m_time;

  int m_begin_int;
  float m_begin_float;

  int m_end_int;
  float m_end_float;
};

}