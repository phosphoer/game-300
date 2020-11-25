// David Evans
// 2011/07/21
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Core/System.h"
#include "../Composition/Composition.h"
#include "../Containers/HashMap.h"
#include "Tilemap.h"

namespace Improbability
{

class MetaComposition;
class World;

class WorldAxis
{
public:
  enum Axis
  {
    Right,
    Top,
    Front,
    Count
  };
};

class TileSystem : public System
{
public:
  typedef std::vector<Tilemap*> TilemapArray;
  typedef HashMap<String, Tilemap*> TilemapsMap;

  static void BindMeta(MetaComposition*);

  TileSystem();

  // Inherited methods
  void InitializeGlobal();
  void InitializeInstance();
  void UpdateInstance(float dt);
  void UninitializeInstance();
  void UninitializeGlobal();
  void Copy(System *dest, bool deep);
  void Deserialize(Serializer &serializer);

  // Messages
  void OnSpaceReady();

  // Methods
  void SetActiveWorld(char const *filename);
  void SetWorldAxis(int axis);
  void SetAnchorAxis(int axis);
  void SetAnchorBegin(int begin);
  void SetAnchorEnd(int end);
  void SetAnchorRange(int range);
  void SetObscured(int obscured);
  void SetMemoryRange(int range);

  // Getters
  Tilemap* GetTilemaps(char const *type);
  World* GetWorld() {return m_world;}
  int GetLevelNumber() {return m_level_number;}
  int GetWorldAxis() {return m_axis;}
  int GetBorderSize() {return m_border_size;}
  int GetAnchorAxis() {return m_anchor_axis;}
  int GetAnchorBegin() {return m_anchor_begin;}
  int GetAnchorEnd() {return m_anchor_end;}
  int GetAnchorRange() {return m_anchor_range;}
  int GetObscured() {return m_obscured;}
  int GetMemoryRange() {return m_memory_limit;}

  // Get the beginning and end of the 3d focus region for the tilemap
  TilePos3 GetFocusBegin() {return m_focus_begin;}
  TilePos3 GetFocusEnd() {return m_focus_end;}

  TilePos3 GetFocusBeginWithArea(TilePos3 begin, int axis, TilePos3 focus_pos);
  TilePos3 GetFocusEndWithArea(TilePos3 end, int axis, TilePos3 focus_pos);

  TilePos3 GetFocusPosition();
private:
  World *m_world;
  TilemapsMap m_tilemaps;
  String m_initial_world;
  int m_axis;
  int m_level_number;

  TilePos3 m_focus_begin;
  TilePos3 m_focus_end;
  int m_border_size;

  // Anchoring and Memory info
  int m_anchor_axis;
  int m_anchor_begin;
  int m_anchor_end;
  int m_anchor_range;
  int m_obscured;
  int m_memory_limit;
};

}