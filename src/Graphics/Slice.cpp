// Nicholas Baldwin
// 2012/04/02
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Slice.h"
#include "..\Core\EngineCore.h"
#include "GraphicsDevice.h"
#include "Texture.h"
#include "TextureFromFile.h"
#include "..\Resource\ResourceManager.h"
#include "Renderable.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "..\Tile\TileGameplayComponent.h"
#include "Camera.h"

namespace Improbability
{
  
bool Slice::m_initialized = false;
DeclarationHandle Slice::m_vertex_declaration;
DeclarationHandle Slice::m_render_declaration;
Texture *Slice::m_anchor_edge_maps[3];
Texture *Slice::m_anchor_maps[3];
Texture *Slice::m_slice_maps[3];
Texture *Slice::m_slice_edge_maps[3];

Slice::Slice(int axis, int depth, float age, float maxLifetime, bool cubes) : 
  m_vertex_buffer(0),
  m_triangle_count(0), 
  m_fade_time(0.0f), 
  m_value(0.0f), 
  m_width(0), 
  m_height(0), 
  m_no_tiles(false)
{
  ErrorIf(!m_initialized, "Slices not initialized");

  //fill in the id
  m_id.m_axis  = axis;
  m_id.m_layer = depth;
  m_id.m_is_anchored = false;
  m_id.m_age = age;
  m_id.m_maxLifetime = maxLifetime;

  int anchor_axis = g_engine_core->SystemCall("TileSystem", "GetAnchorAxis");

  // check if it is actually anchored
  if (axis == anchor_axis)
  {
    int anchor_begin = g_engine_core->SystemCall("TileSystem", "GetAnchorBegin");
    int anchor_end = g_engine_core->SystemCall("TileSystem", "GetAnchorEnd");

    if (anchor_begin < anchor_end)
      std::swap(anchor_begin, anchor_end);

    if (anchor_end >= 0 && depth >= anchor_end && depth <= anchor_begin)
    {
      m_id.m_is_anchored = true;
    }
  }

  Rebuild(cubes);
}

Slice::~Slice()
{
  if(m_no_tiles)
    return;
  GraphicsDevice::DeleteVertexBuffer(m_vertex_buffer);
}

void Slice::Rebuild(bool cubes)
{
  if (m_vertex_buffer)
  {
    GraphicsDevice::DeleteVertexBuffer(m_vertex_buffer);
    m_vertex_buffer = 0;
  }

  m_cubes = cubes;

  Tilemap *graphics_tilemap = g_engine_core->SystemCall("TileSystem", "GetTilemaps", "Graphics");
  Tilemap *special_tilemap = g_engine_core->SystemCall("TileSystem", "GetTilemaps", "Special");
  std::vector<Vector4> tiles;
  std::vector< std::pair<int, int> > edges;

  FindActiveTiles(graphics_tilemap, tiles, edges);
  if(tiles.empty())
  {
    m_no_tiles = true;
    return;
  }
  float sub_texture_width  = 1.0f / float(k_num_sub_textures_per_row),
    sub_texture_height = 1.0f / float(k_num_sub_texture_rows);

  float sub_edge_texture_width  = 1.0f / 16.0f, 
    sub_edge_texture_height = 1.0f / 2.0f;

  // this gets us the actual size of the slice
  ++m_width; ++m_height;

  m_vertex_buffer = GraphicsDevice::CreateVertexBuffer(tiles.size() * 6, 0, false, m_vertex_declaration);
  //m_render_buffer = GraphicsDevice::CreateVertexBuffer(4, 0, false, m_render_declaration);
  Vertex *vertexes = reinterpret_cast<Vertex*>(GraphicsDevice::LockVertexBuffer(m_vertex_buffer));

  unsigned index = 0;

  int x = (m_id.m_axis + 1) % 3;
  int y = (m_id.m_axis + 2) % 3;
  int z = m_id.m_axis;

  Vector3 offsets[4];       
  offsets[0].m_a[x] = 1.0f;
  offsets[0].m_a[y] = 1.0f; 
  offsets[0].m_a[z] = 0.0f;

  offsets[1].m_a[x] = 1.0f; 
  offsets[1].m_a[y] = 0.0f; 
  offsets[1].m_a[z] = 0.0f;

  offsets[2].m_a[x] = 0.0f; 
  offsets[2].m_a[y] = 1.0f; 
  offsets[2].m_a[z] = 0.0f;

  offsets[3].m_a[x] = 0.0f; 
  offsets[3].m_a[y] = 0.0f; 
  offsets[3].m_a[z] = 0.0f;

  switch(m_id.m_axis)
  {
  case 0:
    offsets[0] = Vector3(0.0f, 1.0f, 0.0f);
    offsets[1] = Vector3(0.0f, 0.0f, 0.0f);
    offsets[2] = Vector3(0.0f, 1.0f, 1.0f);
    offsets[3] = Vector3(0.0f, 0.0f, 1.0f);
    break;
  case 1:
    offsets[0] = Vector3(1.0f, 0.0f, 0.0f);
    offsets[1] = Vector3(1.0f, 0.0f, 1.0f);
    offsets[2] = Vector3(0.0f, 0.0f, 0.0f);
    offsets[3] = Vector3(0.0f, 0.0f, 1.0f);
    break;
  case 2:
    offsets[0] = Vector3(1.0f, 1.0f, 0.0f);
    offsets[1] = Vector3(1.0f, 0.0f, 0.0f);
    offsets[2] = Vector3(0.0f, 1.0f, 0.0f);
    offsets[3] = Vector3(0.0f, 0.0f, 0.0f);
    break;
  }

  Vector2 tex_coords[4];
  tex_coords[0] = Vector2(sub_texture_width, 0.0f);
  tex_coords[1] = Vector2(sub_texture_width, sub_texture_height);
  tex_coords[2] = Vector2(0.0f,              0.0f);
  tex_coords[3] = Vector2(0.0f,              sub_texture_height);

  Vector4 edge_coords[4];
  edge_coords[0] = Vector4(sub_edge_texture_width, 0.0f,                    
    sub_edge_texture_width, 0.0f);
  edge_coords[1] = Vector4(sub_edge_texture_width, sub_edge_texture_height, 
    sub_edge_texture_width, sub_edge_texture_height);
  edge_coords[2] = Vector4(0.0f,                   0.0f,                    
    0.0f,                   0.0f);
  edge_coords[3] = Vector4(0.0f,                   sub_edge_texture_height,                    
    0.0f,                   sub_edge_texture_height);


  std::vector<Vector4>::iterator tile_pos = tiles.begin();
  std::vector< std::pair<int, int> >::iterator edge_type = edges.begin();

  for (; tile_pos != tiles.end() && edge_type != edges.end(); ++tile_pos, ++edge_type)
  {
    char sub_texture_index = graphics_tilemap->GetTile(*tile_pos) - 1;

    // Texture is a wall: determine surface status
    if(sub_texture_index == 0)
    {
      // Check tile in front of current to see if its collision
      Tile check_tile;
      Vector4 depth_tile = *tile_pos;
      depth_tile.m_a[m_id.m_axis] += 1;
      check_tile = graphics_tilemap->GetTile(depth_tile); // right tile

      if(!check_tile)
      {
        sub_texture_index = 7;
      }
    }

    float tex_x  = float(sub_texture_index % k_num_sub_textures_per_row) * sub_texture_width;
    float tex_y  = float(sub_texture_index / k_num_sub_textures_per_row) * sub_texture_height;
    float edge_x = float(edge_type->first) * sub_edge_texture_width;
    float edge_y = 0.0f;
    float corner_x = float(edge_type->second) * sub_edge_texture_width;
    float corner_y = sub_edge_texture_height;

    vertexes[index].m_position        = tile_pos->ToVector3() + offsets[0];
    vertexes[index].m_tex_coords      = Vector2(tex_x, tex_y) + tex_coords[0];
    vertexes[index].m_edge_tex_coords = Vector4(edge_x, edge_y, corner_x, corner_y) + edge_coords[0];

    ++index;

    vertexes[index].m_position   = tile_pos->ToVector3() + offsets[1];
    vertexes[index].m_tex_coords = Vector2(tex_x, tex_y) + tex_coords[1];
    vertexes[index].m_edge_tex_coords = Vector4(edge_x, edge_y, corner_x, corner_y) + edge_coords[1];

    ++index;

    vertexes[index].m_position   = tile_pos->ToVector3() + offsets[2];
    vertexes[index].m_tex_coords = Vector2(tex_x, tex_y) + tex_coords[2];
    vertexes[index].m_edge_tex_coords = Vector4(edge_x, edge_y, corner_x, corner_y) + edge_coords[2];

    ++index;
    ++m_triangle_count;

    vertexes[index].m_position   = tile_pos->ToVector3() + offsets[2];
    vertexes[index].m_tex_coords = Vector2(tex_x, tex_y) + tex_coords[2];
    vertexes[index].m_edge_tex_coords = Vector4(edge_x, edge_y, corner_x, corner_y) + edge_coords[2];

    ++index;

    vertexes[index].m_position   = tile_pos->ToVector3() + offsets[1];
    vertexes[index].m_tex_coords = Vector2(tex_x, tex_y) + tex_coords[1];
    vertexes[index].m_edge_tex_coords = Vector4(edge_x, edge_y, corner_x, corner_y) + edge_coords[1];

    ++index;

    vertexes[index].m_position   = tile_pos->ToVector3() + offsets[3];
    vertexes[index].m_tex_coords = Vector2(tex_x, tex_y) + tex_coords[3];
    vertexes[index].m_edge_tex_coords = Vector4(edge_x, edge_y, corner_x, corner_y) + edge_coords[3];

    ++index;
    ++m_triangle_count;
  }

  GraphicsDevice::UnlockVertexBuffer(m_vertex_buffer);
}

void Slice::Initialize()
{
  // create the vertex declaration if it isn't yet
  GraphicsVertexTypes::VertexElement vertex_elements[3];

  vertex_elements[0].m_stream = 0;
  vertex_elements[1].m_stream = 0;
  vertex_elements[2].m_stream = 0;

  vertex_elements[0].m_type = GraphicsVertexTypes::VM_Vector3;
  vertex_elements[1].m_type = GraphicsVertexTypes::VM_Vector2;
  vertex_elements[2].m_type = GraphicsVertexTypes::VM_Vector4;

  vertex_elements[0].m_usage = GraphicsVertexTypes::MU_Position; // position
  vertex_elements[1].m_usage = GraphicsVertexTypes::MU_TexCoord; // main texture coordinates
  vertex_elements[2].m_usage = GraphicsVertexTypes::MU_TexCoord; // tex coords for edge (x,y) and corner (z,w) compination

  m_vertex_declaration = GraphicsDevice::CreateVertexDeclaration(vertex_elements, 3);
  m_render_declaration = GraphicsDevice::CreateVertexDeclaration(vertex_elements, 1);

  m_initialized = true;

  m_anchor_maps[0] = g_engine_core->GetResourceManager()->GetResource<TextureFromFile>("side-anchor.png")->GetTexture();
  m_anchor_maps[1] = g_engine_core->GetResourceManager()->GetResource<TextureFromFile>("top-anchor.png")->GetTexture();
  m_anchor_maps[2] = g_engine_core->GetResourceManager()->GetResource<TextureFromFile>("front-anchor.png")->GetTexture();
 
  m_anchor_edge_maps[0] = g_engine_core->GetResourceManager()->GetResource<TextureFromFile>("side-anchor-edges.png")->GetTexture();
  m_anchor_edge_maps[1] = g_engine_core->GetResourceManager()->GetResource<TextureFromFile>("top-anchor-edges.png")->GetTexture();
  m_anchor_edge_maps[2] = g_engine_core->GetResourceManager()->GetResource<TextureFromFile>("front-anchor-edges.png")->GetTexture();

  m_slice_maps[0] = g_engine_core->GetResourceManager()->GetResource<TextureFromFile>("side-slice.png")->GetTexture();
  m_slice_maps[1] = g_engine_core->GetResourceManager()->GetResource<TextureFromFile>("top-slice.png")->GetTexture();
  m_slice_maps[2] = g_engine_core->GetResourceManager()->GetResource<TextureFromFile>("front-slice.png")->GetTexture();

  m_slice_edge_maps[0] = g_engine_core->GetResourceManager()->GetResource<TextureFromFile>("side-slice-edges.png")->GetTexture();
  m_slice_edge_maps[1] = g_engine_core->GetResourceManager()->GetResource<TextureFromFile>("top-slice-edges.png")->GetTexture();
  m_slice_edge_maps[2] = g_engine_core->GetResourceManager()->GetResource<TextureFromFile>("front-slice-edges.png")->GetTexture();
}

void Slice::Uninitialize()
{
  m_initialized = false;

  GraphicsDevice::DeleteVertexDeclaration(m_vertex_declaration);
}

void Slice::Update(float dt)
{
  using namespace SliceConstants;

  //if (m_fade_time < k_total_fade_time)
  //{
    m_id.m_age += dt;
    m_fade_time += dt;

    int focus_layer = static_cast<TilePos3>(g_engine_core->SystemCall("TileSystem", "GetFocusPosition")).m_a[m_id.m_axis];

    int anchor_axis = g_engine_core->SystemCall("TileSystem", "GetAnchorAxis");
    int dl;

    if (anchor_axis == m_id.m_axis)
    {
      int anchor_begin = g_engine_core->SystemCall("TileSystem", "GetAnchorBegin");
      int anchor_end = g_engine_core->SystemCall("TileSystem", "GetAnchorEnd");

      if (anchor_begin < anchor_end)
        std::swap(anchor_begin, anchor_end);

      // Player inside the anchor range
      if (anchor_end >= 0 && focus_layer >= anchor_end && focus_layer <= anchor_begin)
      {
        if (m_id.m_layer > anchor_begin)
        {
          // memory layer, outside of anchor layers
          dl = abs(m_id.m_layer - anchor_begin);
        }
        else if (m_id.m_layer < anchor_end)
        {
          // memory layer, outside of anchor layers
          dl = abs(m_id.m_layer - anchor_end);
        }
        // layer is an anchor layer
        else
        {
          m_id.m_is_anchored = true;
          m_value = 1.0f;
          m_a_value = 1.0f;

          return;
        }
      }
      else
      {
        // memory layer
        dl = abs(m_id.m_layer - focus_layer);
        m_id.m_is_anchored = false;
      }
    }
    else
    {
      // memory layer
      dl = abs(m_id.m_layer - focus_layer);
    }

    int layerCount = g_engine_core->SystemCall("TileSystem", "GetMemoryRange");
    float percentageOfMaxDistance = float(dl) / float(layerCount);
    float percentageOfLifetime = m_id.m_maxLifetime ? m_id.m_age / m_id.m_maxLifetime : 0;
    float adjustedFadeDurationRange = k_total_fade_time - k_min_fade_time;
    float actualTotalFadeTime = adjustedFadeDurationRange * percentageOfMaxDistance + k_min_fade_time;
    float currentProgress = min(1.0f, m_fade_time / actualTotalFadeTime);

    m_value = 1 - percentageOfMaxDistance;// * percentageOfLifetime;
    m_a_value = currentProgress * m_value;
  //}
}

void Slice::Draw()
{
  if(m_no_tiles)
    return;
  SetUpShaders();

  if (m_id.m_is_anchored)
  {
    m_anchor_maps[m_id.m_axis]->Set("s_texture");
    m_anchor_edge_maps[m_id.m_axis]->Set("s_edges");
  }
  else
  {
    m_slice_maps[m_id.m_axis]->Set("s_texture");
    m_slice_edge_maps[m_id.m_axis]->Set("s_edges");
  }

  GraphicsDevice::SetAlphaEnable(true);

  GraphicsDevice::SetVertexBuffer(m_vertex_buffer);
  GraphicsDevice::DrawTriListBuffer(m_triangle_count);

  if (m_id.m_is_anchored)
  {
    m_anchor_maps[m_id.m_axis]->Unset("s_texture");
    m_anchor_edge_maps[m_id.m_axis]->Unset("s_edges");
  }
  else
  {
    m_slice_maps[m_id.m_axis]->Unset("s_texture");
    m_slice_edge_maps[m_id.m_axis]->Unset("s_edges");
  }
}

unsigned Slice::GetDrawOrder(Camera const *cam)
{
  Matrix4 model_view = GraphicsDevice::GetViewStack().Top() * 
                       GraphicsDevice::GetWorldStack().Top();

  Vector4 origin = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
  origin.m_a[m_id.m_axis] = float(m_id.m_layer);

  origin = model_view * origin;

  float camera_depth = origin.m_z;

  float depth = (camera_depth - cam->GetNear()) / (cam->GetFar() - cam->GetNear());

  return unsigned(UINT_MAX * depth);
}

Matrix4 Slice::GetWorldTransform()
{
  Matrix4 m;
  m.Identity();
  return m;
}

void Slice::FindActiveTiles(Tilemap *tilemap, std::vector<Vector4> &tiles_out, std::vector< std::pair<int, int> > &edges_out)
{
  int x_axis = (m_id.m_axis + 1) % 3;
  int y_axis = (m_id.m_axis + 2) % 3;
  int z_axis = m_id.m_axis;

  // get the start and end of the slice
  TilePos2 begin(0, 0);
  TilePos2 end;

  if (m_id.m_axis == 0)
  {
    end = TilePos2(tilemap->GetHeight(), tilemap->GetDepth());
  }
  else if (m_id.m_axis == 1)
  {
    end = TilePos2(tilemap->GetDepth(), tilemap->GetWidth());
  }
  else
  {
    end = TilePos2(tilemap->GetWidth(), tilemap->GetHeight());
  }

  for (int x = begin.m_x; x <= end.m_x; ++x)
  {
    for (int y = begin.m_y; y <= end.m_y; ++y)
    {
      TilePos3 pos;
      pos.m_a[x_axis] = x;
      pos.m_a[y_axis] = y;
      pos.m_a[z_axis] = m_id.m_layer;

      Tile tile_type = tilemap->GetTile(pos);

      if (tile_type > 0)
      {
        if (x > m_width)
        {
          m_width = x;
        }
        if (y > m_height)
        {
          m_height = y;
        }

        Vector4 tile_position;
        tile_position.m_a[x_axis] = float(x);
        tile_position.m_a[y_axis] = float(y);
        tile_position.m_a[z_axis] = float(m_id.m_layer);
        tile_position.m_w = 1.0f;

        tiles_out.push_back(tile_position);

        int edge_type = GetEdgeType(tilemap, pos);
        int corner_type = GetCornerType(tilemap, pos, edge_type);

        edges_out.push_back(std::make_pair(edge_type, corner_type));
      }
    }
  }
}

int Slice::GetEdgeType(Tilemap *tilemap, TilePos3 pos)
{
  TilePos3 offset_x, offset_y;
  
  switch(m_id.m_axis)
  {
  case 0:
    offset_x = TilePos3(0, 0, -1);
    offset_y = TilePos3(0, 1,  0);
    break;
  case 1:
    offset_x = TilePos3(1, 0,  0);
    offset_y = TilePos3(0, 0, -1);
    break;
  case 2:
    offset_x = TilePos3(1, 0, 0);
    offset_y = TilePos3(0, 1, 0);
    break;
  }

  int edge_type = 0;

  Tile check_tile;
  check_tile = tilemap->GetTile(pos + offset_x); // right tile
  if (check_tile > 0 && check_tile < k_edge_tile_type_max)
  {
    edge_type |= 1 << 0;
  }

  check_tile = tilemap->GetTile(pos + offset_y); // top tile
  if (check_tile > 0 && check_tile < k_edge_tile_type_max)
  {
    edge_type |= 1 << 1;
  }

  check_tile = tilemap->GetTile(pos - offset_x); // left tile
  if (check_tile > 0 && check_tile < k_edge_tile_type_max)
  {
    edge_type |= 1 << 2;
  }

  check_tile = tilemap->GetTile(pos - offset_y); // bottom tile
  if (check_tile > 0 && check_tile < k_edge_tile_type_max)
  {
    edge_type |= 1 << 3;
  }

  return edge_type;
}

int Slice::GetCornerType(Tilemap *tilemap, TilePos3 pos, int edge_type)
{
  TilePos3 offset_x, offset_y;

  switch(m_id.m_axis)
  {
  case 0:
    offset_x = TilePos3(0, 0, -1);
    offset_y = TilePos3(0, 1,  0);
    break;
  case 1:
    offset_x = TilePos3(1, 0,  0);
    offset_y = TilePos3(0, 0, -1);
    break;
  case 2:
    offset_x = TilePos3(1, 0, 0);
    offset_y = TilePos3(0, 1, 0);
    break;
  }

  int corner_type = 0;

  Tile check_tile;
  if (!(edge_type & 1 << 0) || !(edge_type & 1 << 1)) // right or top tile
  {
    corner_type |= 1 << 0;
  }
  else if (edge_type & 1 << 0 && edge_type & 1 << 1) // no right and no top tile
  {
    check_tile = tilemap->GetTile(pos + offset_x + offset_y); // top-right tile
    if (check_tile > 0 && check_tile < k_edge_tile_type_max)
    {
      corner_type |= 1 << 0;
    }
  }

  if (!(edge_type & 1 << 1) || !(edge_type & 1 << 2)) // top or left tile
  {
    corner_type |= 1 << 1;
  }
  else if (edge_type & 1 << 1 && edge_type & 1 << 2) // no top and no left tile
  {
    check_tile = tilemap->GetTile(pos - offset_x + offset_y); // top-left tile
    if (check_tile > 0 && check_tile < k_edge_tile_type_max)
    {
      corner_type |= 1 << 1;
    }
  }

  if (!(edge_type & 1 << 2) || !(edge_type & 1 << 3)) // left or bottom tile
  {
    corner_type |= 1 << 2;
  }
  else if (edge_type & 1 << 2 && edge_type & 1 << 3) // no left and no bottom tile
  {
    check_tile = tilemap->GetTile(pos - offset_x - offset_y); // bottom-left tile
    if (check_tile > 0 && check_tile < k_edge_tile_type_max)
    {
      corner_type |= 1 << 2;
    }
  }

  if (!(edge_type & 1 << 3) || !(edge_type & 1 << 0)) // bottom or right tile
  {
    corner_type |= 1 << 3;
  }
  else if (edge_type & 1 << 3 && edge_type & 1 << 0) // no bottom and no right tile
  {
    check_tile = tilemap->GetTile(pos + offset_x - offset_y); // bottom-right tile
    if (check_tile > 0 && check_tile < k_edge_tile_type_max)
    {
      corner_type |= 1 << 3;
    }
  }

  return corner_type;
}

void Slice::SetUpShaders()
{
  VertexShader *tessellated_vertex = VertexShader::GetShader("expanded_texture");

  tessellated_vertex->Set();

  int focus_layer = static_cast<TilePos3>(g_engine_core->SystemCall("TileSystem", "GetFocusPosition")).m_a[m_id.m_axis];

  if (m_id.m_is_anchored)
  {
    int obscure_layer = g_engine_core->SystemCall("TileSystem", "GetObscured");

    if (obscure_layer > -1 && m_id.m_layer - obscure_layer >= 0)
    {
      PixelShader *obscured_pixel = PixelShader::GetShader("anchor_obscured");
      obscured_pixel->Set();

      obscured_pixel->SetConstant("u_alpha", m_a_value);
    }
    else
    {
      PixelShader *tessellated_pixel = PixelShader::GetShader("tessellated_texture");
      tessellated_pixel->Set();
    }
    
    return;
  }

  int anchor_axis = g_engine_core->SystemCall("TileSystem", "GetAnchorAxis");

  if (m_id.m_axis == anchor_axis)
  {
    int anchor_begin = g_engine_core->SystemCall("TileSystem", "GetAnchorBegin");
    int anchor_end = g_engine_core->SystemCall("TileSystem", "GetAnchorEnd");

    if (anchor_begin < anchor_end)
      std::swap(anchor_begin, anchor_end);

    if (anchor_end >= 0 && focus_layer >= anchor_end && focus_layer <= anchor_begin)
    {
      if (m_id.m_layer > anchor_begin)
      {
        PixelShader *front_layer = PixelShader::GetShader("front_layer");
        front_layer->Set();

        front_layer->SetConstant("u_lightness", m_value);
        front_layer->SetConstant("u_alpha", m_a_value);
      }
      else if (m_id.m_layer < anchor_end)
      {
        PixelShader *back_layer = PixelShader::GetShader("back_layer");
        back_layer->Set();

        back_layer->SetConstant("u_lightness", m_value);
        back_layer->SetConstant("u_alpha", m_a_value);
      }
    }
    else
    {
      if (m_id.m_layer - focus_layer < 0)
      {
        PixelShader *back_layer = PixelShader::GetShader("back_layer");
        back_layer->Set();

        back_layer->SetConstant("u_lightness", m_value);
        back_layer->SetConstant("u_alpha", m_a_value);
      }
      else if (m_id.m_layer - focus_layer > 0)
      {
        PixelShader *front_layer = PixelShader::GetShader("front_layer");
        front_layer->Set();

        front_layer->SetConstant("u_lightness", m_value);
        front_layer->SetConstant("u_alpha", m_a_value);
      }
      else
      {
        PixelShader *tessellated_pixel = PixelShader::GetShader("tessellated_texture");
        tessellated_pixel->Set();
      }
    }
  }
  else
  {
    if (m_id.m_layer - focus_layer < 0)
    {
      PixelShader *back_layer = PixelShader::GetShader("back_layer");
      back_layer->Set();

      back_layer->SetConstant("u_lightness", m_value);
      back_layer->SetConstant("u_alpha", m_a_value);
    }
    else if (m_id.m_layer - focus_layer > 0)
    {
      PixelShader *front_layer = PixelShader::GetShader("front_layer");
      front_layer->Set();

      front_layer->SetConstant("u_lightness", m_value);
      front_layer->SetConstant("u_alpha", m_a_value);
    }
    else
    {
      PixelShader *tessellated_pixel = PixelShader::GetShader("tessellated_texture");
      tessellated_pixel->Set();
    }
  }
}

}