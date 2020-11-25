float4x4 u_world_matrix;
float4x4 u_model_view_matrix;
float4x4 u_projection_matrix;
float4 u_vertex_constants;

float4 Transform(float4 i)
{
  float4 transformed = mul(mul(u_projection_matrix, u_model_view_matrix), i);
  transformed.xy -= u_vertex_constants.xy * 0.5;
  return transformed;
}