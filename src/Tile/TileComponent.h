// David Evans
// 2011/07/23
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Composition/Component.h"
#include "../Math/Vector4.h"
#include "Tilemap.h"
#include "../Input/InputEvent.h"

namespace Improbability
{
class GameSystem;
class Serializer;
class MetaComposition;
class Physics;
struct collisionDirection;
class TilePos3;

struct MemoryLayer
{
  MemoryLayer(int _layer, float _max):layer(_layer), age(0), maxAge(_max){}
  int layer;
  float age;
  float maxAge;
  bool operator==(const MemoryLayer& rhs) const {return layer == rhs.layer;}
  bool operator==(const int& rhs) const {return layer == rhs;}
  bool operator<(const MemoryLayer& rhs) const {return layer < rhs.layer;}
};
enum PLAYERFACING
{
  RIGHT,
  LEFT,
  UP,
  DOWN
};
enum NARRATIVEMESSAGES
{
  MESSAGE_OVERRIDE,
  NO_NARRATIVE_MESSAGE,
  OPENING,
  COINS,
  COIN_COMPLETE,
  UNLOCK_ROTATION_BLOCKS,
  FIRST_ROTATION,
  UNLOCK_FREE_ROTATION,
  LEVEL_ONE,
  LEVEL_TWO,
  UNLOCK_ANCHOR_BLOCKS,
  FIRST_ANCHOR,
  UNLOCK_FREE_ANCHORING,
  FIRST_FREE_ANCHOR,
  FINAL_LEVEL,
  END_GAME,
  //Tutorial Messages
  MOVEMENT,
  JUMPING,
  ROTATING,
  ANCHOR_CLEARING,
  ANCHOR_SETTING,
  ANCHOR_COMMITTING,
  ANCHOR_DROPPING,
  POST_ANCHOR_DROPPING,
  HUB_RETURN,
  WALK_BEHIND
};

enum CONTROLCONFIG
{
  NORMAL_DIRECTIONAL,
  NORMAL_POSITIONAL,
  SOUTHPAW_DIRECTIONAL,
  SOUTHPAW_POSITIONAL
};

class TileComponent : public Component
{
  friend class Physics;

public:
  typedef std::vector<Vector3> HotspotArray;

  static void BindMeta(MetaComposition*);

  TileComponent();
  TileComponent(TileComponent const &rhs);

  // Inherited methods
  void Initialize();
  void Uninitialize();
  Component* Clone();
  void Serialize(Serializer &serializer) { }
  void Deserialize(Serializer &serializer);

  // Methods
  void Update(Tilemap *tilemap, float dt);
  void SnapHorizontal(int centerShift = 0);
  void SnapVertical(int centerShift = 0);
  void SnapDepth();
  void SetAnchorAxis(int axis);
  void SetAnchorBegin(int begin);
  void SetAnchorEnd(int end);
  void DebugDraw();

  // Messages
  void GetTileComponent();
  void OnHUDReady();
  void OnAxisChange(int old_axis, int new_axis);
  void OnAxisChangeComplete(int axis);
  void OnGrounded();
  void OnAnchorTileEnter();
  void OnRotateStart(int start_axis, int end_axis);
  void OnRotateComplete(int start_axis, int end_axis);
  void OnAxisInitalize(int axis);
  void OnPlayerDrop();
  void OnAnchorEvent(int event);
  void OnRecieveControllerConfig(unsigned config);
  void CoinCollected(TilePos3 position);
  void SetPosition(TilePos3 position);

  // Getters
  int GetHeight() {return m_height;}
  int GetLayer() {return m_layer;}
  int GetAxis() {return m_axis;}
  TilePos2 GetTilePos2();
  TilePos3 GetTilePos3();
  char GetCollisionTileAtOffset(Tilemap *tilemap, Vector3 offset);
  bool IsGrounded();
  void OnKeyPress(KeyboardEvent key);

private:
  friend class GameSystem;
  bool CheckGrounded(Tilemap *tilemap, Vector3 offset);
  void UpdateTilePos();
  void ResolveHorizontalCollision(Vector3 contact_offset);
  void ResolveVerticalCollision(Vector3 contact_offset);
  void ResolveDepthCollision(Vector3 contact_offset);
  void CheckCollisionInDirection(collisionDirection &data, Vector4 pos, Tilemap *tilemap);
  void Anchor(bool undo = false);
  void RotateTo(int axis);
  void EnableRotateTo(int axis, bool enable);
  void UpdateMemory(int axis);
  void HUDRotatePrompt(bool enable);
  void AnimatePlayer( float dt );
  void ReturnFromSpace();
  void SpaceWrapup();
  void HUDNarrativePrompt(NARRATIVEMESSAGES message = NO_NARRATIVE_MESSAGE, float time = 2.5f, bool freeze = false);
  void ChangeMemoryLimit(int newLimit);
  void AnimateTiles( float dt );
  void UnlockTiles(Tilemap* collectMap, TilePos3 pos, bool loadUnlock=false);
  // Keep track of frame time
  float m_dt;

  // Size of the collider, in tiles
  int m_width;
  int m_height;
  int m_depth;

  // Size of the collider as a percentage of a tile
  float m_fwidth;
  float m_fheight;
  float m_fdepth;

  // Whether or not tile collider is grounded
  bool m_is_grounded;

  // Amount to offset the origin
  Vector3 m_offset;

  // Which plane am I on in the stack?
  // 0 is the closest to the camera
  int m_layer;

  // Which axis plane am I on?
  int m_axis;
  int m_prev_axis;

  // Anchoring info
  int m_anchor_axis;
  int m_anchor_begin;
  int m_anchor_end;
  std::list<int> m_ommited_layers;

  // Monitor Tile position
  TilePos3 m_tile_pos;

  // Hotspots
  HotspotArray m_hs_hor[3];
  HotspotArray m_hs_ver[3];
  int m_anchor_range;
  bool m_drop;
  float m_drop_y;
  int m_obscured;
  bool m_rotating;
  bool m_grounded_tile_change;
  TilePos3 m_last_grounded_pos;
  bool m_anchor_enabled;

  // 'Memory' lists
  std::list<MemoryLayer> m_TopMemory;
  std::list<MemoryLayer> m_FrontMemory;
  std::list<MemoryLayer> m_SideMemory;
  unsigned m_currMemorySize;
  TilePos3 m_grounded_pos;
  bool m_tile_change;
  TilePos3 m_last_tile_pos;
  bool m_anchor_commit;
  float m_count;
  float m_maxAge;

  // AnimationData
  PLAYERFACING m_facing;
  float m_player_animation_count;
  float m_frame_delay;
  bool m_ground_clear;
  float m_apex;
  float m_fall_max;
  TilePos3 m_last_safe_pos;
  bool m_initialized;
  bool m_levitate;
  bool m_left_space;
  bool m_dead;
  float m_deadCount;
  bool m_silent_anchor;
  bool m_warping;
  bool m_leveling;
  Vector3 m_levitate_rate;
  int m_next_level;
  bool m_narrating;
  float m_narrate_count;
  bool m_tutorial;
  bool m_prompt_continue;
  String m_narration_message;
  bool m_skip_text;
  CONTROLCONFIG m_controller_config;
  bool m_hud_ready;
  float m_constant_animation_count;
  unsigned m_memorySize;
};

}