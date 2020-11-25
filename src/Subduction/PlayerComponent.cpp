// Aaron Biddlecom
// 2011/12/9
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "PlayerComponent.h"
#include "../Core/EngineCore.h"
#include "../Composition/Composition.h"
#include "../FileIO/Serializer.h"
#include "../Physics/DynamicComponent.h"
#include "../Tile/Tilemap.h"

namespace Improbability
{

  MakeMeta(PlayerComponent);

  void PlayerComponent::BindMeta(MetaComposition *comp)
  {
    comp->MetaAddComponent(MetaNewable);
    comp->MetaAddComponent(MetaCallable);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnGrounded);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GroundType);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnNotGrounded);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnHorizMoveStart);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnHorizMoveFinish);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnFallStart);
    comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAxisChange);
  }

  PlayerComponent::PlayerComponent():
    m_grounded(false),
    m_moving(false)
  {
  }


  void PlayerComponent::Initialize()
  {
    g_engine_core->AddEventListener(MessageBank::OnAxisChange, this);
  }

  void PlayerComponent::Uninitialize()
  {
    g_engine_core->RemoveEventListener(MessageBank::OnAxisChange, this);
  }

  Component* PlayerComponent::Clone()
  {
    Component *clone = new PlayerComponent(*this);
    return clone;
  }

  void PlayerComponent::Deserialize(Serializer &serializer)
  {
  }  

  // React to collision with specific ground types
  void PlayerComponent::GroundType(char tile_type)
  {
  }

  void PlayerComponent::OnGrounded()
  {
    float newY = GetParent()->GetPosition().m_y;
    /*if(m_start_y >= newY)
    GetParent()->Invoke("PlaySound", "Jump2.wav");*/

    /*if(m_moving)
      GetParent()->Invoke("PlaySound", "BallRoll2.wav");*/
    m_grounded = true;
    g_engine_core->DispatchEvent(MessageBank::OnPlayerGrounded);
  }
  void PlayerComponent::OnNotGrounded()
  {
    m_start_y = GetParent()->GetPosition().m_y;
    m_grounded = false;
   /* if(m_moving)
      GetParent()->Invoke("StopSound", "BallRoll2.wav");*/
    g_engine_core->DispatchEvent(MessageBank::OnPlayerNotGrounded);

  }
  void PlayerComponent::OnHorizMoveStart()
  {
    /*if(m_grounded)
      GetParent()->Invoke("PlaySound", "BallRoll2.wav");*/
    m_moving = true;
  }
  void PlayerComponent::OnHorizMoveFinish()
  {
    //GetParent()->Invoke("StopSound", "BallRoll2.wav");
    m_moving = false;
  }

  void PlayerComponent::OnFallStart()
  {
    m_apex_y = GetParent()->GetPosition().m_y;
  }

  void PlayerComponent::OnAxisChange(int old_axis, int new_axis)
  {
    GetSibling<DynamicComponent>()->SetHorizontalVelocity(Vector3(0,0,0));
    //GetParent()->Invoke("PlaySound", "rotate1.wav");
  }
}