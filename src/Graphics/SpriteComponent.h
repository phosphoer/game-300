// Nicholas Baldwin
// 2012/02/19
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "../Composition/component.h"
#include "Drawable.h"

namespace Improbability
{

namespace SpriteConstants
{
  const float k_total_fade_time = 10.f;//1.5f;
  const float k_min_fade_time = 1.f;//0.15f;
}

class SpriteComponent : public Component, public Drawable
{
public:
  static void BindMeta(MetaComposition *comp);

  void Initialize();
  void Uninitialize();
  Component* Clone();
  void Serialize(Serializer &serializer) { }
  void Deserialize(Serializer &serializer);

  void Update(float dt, unsigned axis);

  void Draw();
  unsigned GetDrawOrder(Camera const *cam);
  Matrix4 GetWorldTransform();

  void Reset(unsigned axis) { m_fade_time[axis] = 0.0f; m_value[axis] = 0.0f; m_a_value[axis] = 0.0f;}

  void SetAnimation(unsigned i);
  void SetFrame(unsigned i);
  void IncrementFrame();

  int GetCurrentAnimation();
  int GetCurrentFrame();

private:
  void SetUpShaders();

  String m_animation_name;
  unsigned m_mesh, m_declaration;
  unsigned m_frame, m_track;
  unsigned m_axis;

  float m_fade_time[3];
  float m_value[3];
  float m_a_value[3];

  static const unsigned k_num_sub_textures_per_row = 1;
  static const unsigned k_edge_tile_type_max = 4;
};

}