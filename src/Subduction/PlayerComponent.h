// Aaron Biddlecom
// 2011/12/9
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Composition/Component.h"

namespace Improbability
{

  class Serializer;
  class MetaComposition;

  class PlayerComponent : public Component
  {
  public:
    static void BindMeta(MetaComposition*);

    // Inherited methods
    PlayerComponent();
    void Initialize();
    void Uninitialize();
    Component* Clone();
    void Serialize(Serializer &serializer) { }
    void Deserialize(Serializer &serializer);
	void UpdateInstance(float dt);

    // Methods
    void GroundType(char tile_type);
    void OnGrounded();
    void OnNotGrounded();
    void OnHorizMoveStart();
    void OnHorizMoveFinish();
    void OnFallStart();
    void OnAxisChange(int old_axis, int new_axis);
    // Getters

  private:
    bool m_grounded;
    bool m_moving;
    float m_start_y;
    float m_apex_y;
  };

}