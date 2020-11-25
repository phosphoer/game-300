#include "vertex_header.vsh"

struct VertexIn
{
  float3 m_position   : POSITION;
};

struct VertexOut
{
  float4 m_position   : POSITION;
};

VertexOut main(VertexIn i)
{
  VertexOut o    = (VertexOut)0;
  o.m_position   = Transform(float4(i.m_position, 1.0f));
  return o;
}