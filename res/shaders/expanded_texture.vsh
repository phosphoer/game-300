#include "vertex_header.vsh"

struct VertexIn
{
  float3 m_position      : POSITION;
  float2 m_tex_coords    : TEXCOORD0;
  float4 m_ex_tex_coords : TEXCOORD1;
};

struct VertexOut
{
  float4 m_position        : POSITION;
  float4 m_tex_coords      : TEXCOORD0;
  float4 m_ex_tex_coords   : TEXCOORD1;
  float4 m_camera_position : TEXCOORD2;
};

VertexOut main(VertexIn i)
{
  VertexOut o         = (VertexOut)0;
  o.m_position        = Transform(float4(i.m_position, 1.0f));
  o.m_tex_coords      = float4(i.m_tex_coords, 0.0f, 0.0f);
  o.m_ex_tex_coords   = i.m_ex_tex_coords;
  o.m_camera_position = mul(u_model_view_matrix, float4(i.m_position, 1.0f));
  return o;
}