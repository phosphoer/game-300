// David Evans
// 2011/07/21
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Resource/Resource.h"
#include "../containers/HashMap.h"
#include "../Math/Vector4.h"
#include <vector>

namespace Improbability
{

class MetaComposition;

typedef char Tile;
class TileGameplayComponent;
class Serializer;

class TilePos2
{
public:
  TilePos2(int x = 0, int y = 0) : m_x(x), m_y(y) {}
  TilePos2(float x, float y) : m_x(static_cast<int>(x)), m_y(static_cast<int>(y)) {}
  bool operator==(TilePos2 const &rhs) {return (m_x == rhs.m_x && m_y == rhs.m_y);}
  bool operator!=(TilePos2 const &rhs) {return !(*this == rhs);}

  union
  {
    struct {int m_x; int m_y;};
    int m_a[2];
  };
};

class TilePos3
{
public:
  TilePos3(int x = 0, int y = 0, int z = 0) : m_x(x), m_y(y), m_z(z) {}
  TilePos3(float x, float y, float z) : m_x(static_cast<int>(x)), m_y(static_cast<int>(y)), m_z(static_cast<int>(z)) {}
  bool operator==(TilePos3 const &rhs) const {return (m_x == rhs.m_x && m_y == rhs.m_y && m_z == rhs.m_z);}
  bool operator!=(TilePos3 const &rhs) {return !(*this == rhs);}
  static void BindMeta(MetaComposition*);
  void Deserialize(Serializer &serializer);
  void Serialize(Serializer &serializer);
  TilePos3 operator+(TilePos3 const &rhs) { return TilePos3(m_x + rhs.m_x, m_y + rhs.m_y, m_z + rhs.m_z); }
  TilePos3 operator-(TilePos3 const &rhs) { return TilePos3(m_x - rhs.m_x, m_y - rhs.m_y, m_z - rhs.m_z); }
  
  union
  {
    struct {int m_x; int m_y; int m_z;};
    int m_a[3];
  };
};

class TileSystem;
class String;

class Tilemap : public Resource
{
  friend class TileSystem;
  
public:

  static void BindMeta(MetaComposition*);

  Tilemap();
  Tilemap(Tilemap& rhs);
  ~Tilemap();

  void Load(char const *filepath);
  void ResetTilemap();
  void Unload();

  void SetTile(int x, int y, int z, Tile value);
  void SetTile(Vector4 pos, Tile value);

  Tile GetTile(int x, int y, int z);
  Tile GetTile(TilePos2 pos, int axis, int layer);
  Tile GetTile(TilePos3 pos);
  Tile GetTile(Vector4 pos);
  bool IsOutside(int x, int y, int z);
  TileGameplayComponent* GetTileObject(int x, int y, int z);

  int GetWidth() {return m_width;}
  int GetHeight() {return m_height;}
  int GetDepth() {return m_depth;}
  Tilemap* GetNext() {return m_next;}

  static TilePos2 GetTilePos(Vector4 pos, int axis);
  static TilePos3 GetTilePos(Vector4 pos);
  static float GetPenetration(float pos) {return pos - std::floor(pos);}

  void BuildObjectMap(int mapType);
  void RepairMap(int mapType);
  void ToggleRotationTiles( bool toggle );
  void ToggleAnchorTiles( bool toggle );
  void ToggleAnchorTilesActive( bool toggle, int anchor_axis );
  void SetRotationAbilityBlockActive();
  void SetRotationBlocksBlockActive();
  void SetAnchorBlocksBlockActive();
  void SetAnchorAbilityBlockActive();
  void SetCollectiblesActive();
  void SetArtifactsActive();
  void AnimateWarpTiles();

private:
  void Allocate(int x, int y, int z);
  Tilemap *m_next;
  Tile *m_grid;
  Tile *m_origGrid;
  int m_width;
  int m_height;
  int m_depth;

  HashMap<String, TileGameplayComponent*> m_tile_objects;
  std::list<TileGameplayComponent*> m_tile_list;
};

}