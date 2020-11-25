// Nicholas Baldwin
// 2012/04/29
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "Drawable.h"
#include "..\Tile\Tilemap.h"
#include "GraphicsHandleTypes.h"

namespace Improbability
{
  
struct MemoryLayer;
class AnchorVeil : public Drawable
{
public:
  AnchorVeil(TilePos3 start_position, int axis);
  ~AnchorVeil();

  void Update(TilePos3 end_position, std::list<MemoryLayer> memory);

  void Draw();
  unsigned GetDrawOrder(Camera const *);
  Matrix4 GetWorldTransform() { Matrix4 m; m.Identity(); return m; }

  void Commit(TilePos3 end_position);

  int GetAxis() {return m_axis;}
  TilePos3 GetStartPos() {TilePos3 pos = TilePos3(0, 0, 0); pos.m_a[m_x_axis] = m_start_x_layer; return pos;}
  TilePos3 GetEndPos() {TilePos3 pos = TilePos3(0, 0, 0); pos.m_a[m_x_axis] = m_end_x_layer; return pos;}

private:
  DeclarationHandle m_vertex_declaration;
  BufferHandle m_vertex_buffer;

  int m_start_x_layer, m_end_x_layer, m_start_z_layer, m_end_z_layer, m_axis, m_x_axis, m_z_axis;
  bool m_committed;
};

}