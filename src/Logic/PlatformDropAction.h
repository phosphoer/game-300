// Aaron Biddlecom
// 2011/09/22
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "Action.h"
#include "../Math/Vector3.h"

namespace Improbability
{

class Serializer;
class MetaComposition;

class PlatformDropAction : public Action
{
public:
  static void BindMeta(MetaComposition*);

  // Inherited methods
  void Start();
  ActionReturn::Type Update(float dt);
  ActionReturn::Type InactiveUpdate(float dt);
  void Deserialize(Serializer &serializer);
  void Initialize();
  void Uninitialize();
  Action* Clone() {return new PlatformDropAction(*this);}
  PlatformDropAction():
    m_axis(0),
    m_ignore_zeroes(false),
    m_collide(false)
  {}

  //Messages
  void OnAxisChange(int old_axis, int new_axis);

  // Methods
  void SetVector(Vector3 vector) {m_vector = vector;}
  void SetIgnoreZeroes(bool flag) {m_ignore_zeroes = flag;}
private:
  Vector3 m_vector;
  int m_axis;
  bool m_ignore_zeroes;
  bool m_collide;
};

}