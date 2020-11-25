// Nicholas Baldwin
// 2012/04/02
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "..\Tile\Tilemap.h"
#include "..\math\Vector3.h"
#include "Drawable.h"
#include "GraphicsHandleTypes.h"

namespace Improbability
{

class Texture;

namespace SliceConstants
{
  const float k_total_fade_time = 10.f;//1.5f;
  const float k_min_fade_time = 1.f;//0.15f;
}

class Slice : public Drawable
{
public:
  Slice(int axis, int depth, float age, float maxLifetime, bool cubes = false);
  ~Slice();

  void Rebuild(bool cubes = false);

  void Update(float dt);

  void Draw();
  unsigned GetDrawOrder(Camera const *cam);
  Matrix4 GetWorldTransform();

  static void Initialize();
  static void Uninitialize();

  int GetLayer() { return m_id.m_layer; }

private:
  // graphics handles
  static bool m_initialized;
  static DeclarationHandle m_vertex_declaration;
  BufferHandle m_vertex_buffer;
  static DeclarationHandle m_render_declaration;
  BufferHandle m_render_buffer;

  // memory fading values
  float m_fade_time;
  float m_value;
  float m_a_value;

  bool m_cubes;

  // size in world space
  int m_width;
  int m_height;

  bool m_no_tiles;

  // helpers to build up the slice
  void FindActiveTiles(Tilemap *tilemap, std::vector<Vector4> &tiles_out, std::vector< std::pair<int, int> > &edges_out);
  int GetEdgeType(Tilemap *tilemap, TilePos3 pos);
  int GetCornerType(Tilemap *tilemap, TilePos3 pos, int edge_type);

  // helper for setting up the shaders
  void SetUpShaders();

  // struct used to identify the slice
  struct SliceIdentifier
  {
    int m_axis;
    int m_layer;
    float m_age;
    float m_maxLifetime;
    bool m_is_anchored;

    unsigned Hash(unsigned table_size) const { return ((m_is_anchored ? -1 : 1 ) * (m_axis << 16) + (m_layer)) % table_size; }

    bool operator==(SliceIdentifier const &rhs) const { return m_axis == rhs.m_axis && m_layer == rhs.m_layer && m_is_anchored == rhs.m_is_anchored; }
  };

  SliceIdentifier m_id;

  struct Vertex
  {
    Vector3 m_position;
    Vector2 m_tex_coords;
    Vector4 m_edge_tex_coords;

  };

  unsigned m_triangle_count;

  static Texture *m_anchor_maps[3];
  static Texture *m_anchor_edge_maps[3];
  static Texture *m_slice_maps[3];
  static Texture *m_slice_edge_maps[3];

  static const unsigned k_num_sub_textures_per_row = 16;
  static const unsigned k_num_sub_texture_rows = 1;
  static const unsigned k_edge_tile_type_max = 12;
};

}