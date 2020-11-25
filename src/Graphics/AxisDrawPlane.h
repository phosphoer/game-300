// Nicholas Baldwin
// 2012/02/17
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "GraphicsHandleTypes.h"

namespace Improbability
{

class AxisDrawPlane
{
public:
  AxisDrawPlane();
  ~AxisDrawPlane();

  void SetRenderTarget();
  void UnsetRenderTarget();

  void Draw(unsigned sub_quad);

  void SetTransform(Matrix4 const &transform) { m_world_transform = transform; }

private:
  RenderTargetHandle m_render_target;
  BufferHandle m_vertex_buffer;
  DeclarationHandle m_vertex_declaration;

  Matrix4 m_world_transform;

  struct Vertex
  {
    Vector3 m_position;
    Vector3 m_normal;
    Vector2 m_tex_coords;
  };
};

}