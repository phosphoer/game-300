// Nicholas Baldwin
// 2012/02/05
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "..\Logic\Action.h"
#include "..\Input\InputSystem.h"

namespace Improbability
{

class Serializer;
class MetaComposition;

class CameraFollow : public Action
{
public:
  static void BindMeta(MetaComposition*);

  CameraFollow();

  // Inherited methods
  void Initialize();
  void Start();
  ActionReturn::Type Update(float dt);
  void Uninitialize();
  void Deserialize(Serializer &serializer);
  Action* Clone() {return new CameraFollow(*this);}

  // Methods
  void SetDistance(float distance) {m_distance = distance;}
  void SetAxis(unsigned axis) {m_axis = axis;}
  void SetObjectName(String name) {m_object_name = name;}
  void OnPlayerNotGrounded();
  void OnPlayerGrounded();
  void OnHelpActivated();
  void OnKeyDown(KeyboardEvent event);

private:
  float m_distance;
  String m_object_name;
  bool m_grounded;
  int m_axis;
  int m_nearplane_default;
};

}