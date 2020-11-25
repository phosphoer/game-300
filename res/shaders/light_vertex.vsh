#include "vertex_header.vsh"

struct VertexIn
{
  float3 m_position      : POSITION;
};

struct VertexOut
{
  float4 m_position        : POSITION;
  float4 m_camera_position : TEXCOORD;
};

VertexOut main(VertexIn i)
{
  VertexOut o         = (VertexOut)0;
  o.m_position        = Transform(float4(i.m_position, 1.0f));
  o.m_camera_position = mul(u_model_view_matrix, float4(i.m_position, 1.0f));
  return o;
}