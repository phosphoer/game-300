// David Evans
// 2011/07/21
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Core/System.h"
#include "../Composition/Composition.h"

namespace Improbability
{

class MetaComposition;
class DynamicComponent;
class TileComponent;
class Tilemap;

class Physics : public System
{
public:
  typedef HashMap<CompositionId, DynamicComponent*> DynamicComponentMap;
  typedef HashMap<CompositionId, TileComponent*> TileComponentMap;

  static void BindMeta(MetaComposition*);

  Physics();

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
  void OnAxisChange(int old_axis, int new_axis);
  void OnAxisChangeComplete(int axis);
  void OnAnchorSmushBegin();
  void OnAnchorSmushComplete();
  void OnAnchorEvent(int anchor_event_type);
  void OnPlayerDeathToggle(bool toggle);

  Vector3 GetGravity() {return m_gravity;}
  
private:
  void UpdateDynamicComponents(float dt);
  void UpdateTileComponents(float dt);
  void DebugDrawTilemaps();
  void DebugDrawSlicedTilemaps();
  void DebugDrawDynamicComponents();
  void DebugDrawTileComponents();
  void OnHelpActivated();
  String m_collision_layer_name;
  Vector3 m_gravity;
  int m_tile_draw_dist;
  float m_max_velocity;
  float m_dt;

  DynamicComponentMap m_dynamic_components;
  TileComponentMap m_tile_components;
  bool m_helping;
  bool m_rotating;
  bool m_smushing;
  bool m_frame_delay;
  bool m_dead;
};

}