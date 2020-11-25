// Nicholas Baldwin
// 2012/04/29
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "AnchorVeil.h"
#include "GraphicsVertexTypes.h"
#include "GraphicsDevice.h"
#include "..\Tile\Tilemap.h"
#include "..\Tile\TileComponent.h"
#include "..\Core\EngineCore.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "Camera.h"

namespace Improbability
{
  
AnchorVeil::AnchorVeil(TilePos3 start_position, int axis) : 
  m_axis(axis),
  m_committed(false)
{
  // create the vertex declaration if it isn't yet
  GraphicsVertexTypes::VertexElement vertex_elements[1];

  vertex_elements[0].m_stream = 0;
  vertex_elements[0].m_type = GraphicsVertexTypes::VM_Vector3;
  vertex_elements[0].m_usage = GraphicsVertexTypes::MU_Position; // position

  m_vertex_declaration = GraphicsDevice::CreateVertexDeclaration(vertex_elements, 1);

  m_vertex_buffer = GraphicsDevice::CreateVertexBuffer(12, 0, false, m_vertex_declaration);
  Vector3 *vertex = reinterpret_cast<Vector3*>(GraphicsDevice::LockVertexBuffer(m_vertex_buffer));

  float height;
  Tilemap *tilemap = g_engine_core->SystemCall("TileSystem", "GetTilemaps", "Graphics");

  if (axis == 0)
  {
    height = float(tilemap->GetHeight());

    vertex[0] = Vector3(1.0f, height, 0.0f);
    vertex[1] = Vector3(1.0f, 0.0f,   0.0f);
    vertex[2] = Vector3(1.0f, height, 1.0f);
    vertex[3] = Vector3(1.0f, 0.0f,   1.0f);

    vertex[4] = Vector3(1.0f, height, 1.0f);
    vertex[5] = Vector3(1.0f, 0.0f,   1.0f);
    vertex[6] = Vector3(0.0f,  height, 1.0f);
    vertex[7] = Vector3(0.0f,  0.0f,   1.0f);

    vertex[8] =  Vector3(0.0f, height, 0.0f);
    vertex[9] =  Vector3(0.0f, 0.0f,   0.0f);
    vertex[10] = Vector3(1.0f,  height, 0.0f);
    vertex[11] = Vector3(1.0f,  0.0f,   0.0f);

    m_x_axis = 2;
    m_z_axis = 0;
  }
  else
  {
    height = float(tilemap->GetHeight());

    vertex[0] = Vector3(1.0f, height, 1.0f);
    vertex[1] = Vector3(1.0f, 0.0f,   1.0f);
    vertex[2] = Vector3(0.0f, height, 1.0f);
    vertex[3] = Vector3(0.0f, 0.0f,   1.0f);

    vertex[4] = Vector3(1.0f, height, 0.0f);
    vertex[5] = Vector3(1.0f, 0.0f,   0.0f);
    vertex[6] = Vector3(1.0f, height, 1.0f);
    vertex[7] = Vector3(1.0f, 0.0f,   1.0f);

    vertex[8] =  Vector3(0.0f, height, 1.0f);
    vertex[9] =  Vector3(0.0f, 0.0f,   1.0f);
    vertex[10] = Vector3(0.0f, height, 0.0f);
    vertex[11] = Vector3(0.0f, 0.0f,   0.0f);

    m_x_axis = 0;
    m_z_axis = 2;
  }

  GraphicsDevice::UnlockVertexBuffer(m_vertex_buffer);

  m_start_x_layer = int(start_position.m_a[m_x_axis]);
  m_end_x_layer = m_start_x_layer;
}

AnchorVeil::~AnchorVeil()
{
  GraphicsDevice::DeleteVertexBuffer(m_vertex_buffer);
  GraphicsDevice::DeleteVertexDeclaration(m_vertex_declaration);
}

void AnchorVeil::Update(TilePos3 end_position, std::list<MemoryLayer> memory)
{
  if (!m_committed)
  {
    int max_anchor_range = g_engine_core->SystemCall("TileSystem", "GetAnchorRange");

    if (max_anchor_range >= abs(int(end_position.m_a[m_x_axis]) - m_start_x_layer))
      m_end_x_layer = int(end_position.m_a[m_x_axis] + 1.0f);
  }

  std::list<int> tempList;
  for(std::list<MemoryLayer>::iterator mIt = memory.begin(); mIt != memory.end(); ++mIt)
  {
    tempList.push_back(mIt->layer);
  }
  tempList.sort();
  m_start_z_layer = tempList.front();
  m_end_z_layer = tempList.back();
  //m_start_z_layer = memory.front().layer;
  //m_end_z_layer = memory.back().layer;
}

void AnchorVeil::Draw()
{
  VertexShader *vertex = VertexShader::GetShader("basic");
  vertex->Set();
  PixelShader *pixel = PixelShader::GetShader("anchor_veil");
  pixel->Set();

  MatrixStack<Matrix4> &world = GraphicsDevice::GetWorldStack();

  Vector4 translation = Vector4(0.0f, 0.0f, 0.0f, 1.0f), scale = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
  if (m_end_x_layer > m_start_x_layer)
  {
    translation.m_a[m_x_axis] = float(m_start_x_layer);
    scale.m_a[m_x_axis] = float(m_end_x_layer - m_start_x_layer);
    translation.m_a[m_z_axis] = float(m_start_z_layer);
    scale.m_a[m_z_axis] = float(m_end_z_layer - m_start_z_layer);
  }
  else
  {
    translation.m_a[m_x_axis] = float(m_end_x_layer - 1);
    scale.m_a[m_x_axis] = float(m_start_x_layer - (m_end_x_layer - 2));
    translation.m_a[m_z_axis] = float(m_start_z_layer);
    scale.m_a[m_z_axis] = float(m_end_z_layer - m_start_z_layer);
  }
  
  world.Push();
  world.LoadMatrix(Matrix4::CreateTranslation(translation));
  world.LoadMatrix(Matrix4::CreateScaling(scale));
  
  GraphicsDevice::CommitModelViewMatrix();

  GraphicsDevice::SetAlphaEnable(true);
  GraphicsDevice::SetDepthEnable(false);

  GraphicsDevice::SetVertexBuffer(m_vertex_buffer);
  GraphicsDevice::DrawTriStripBuffer(2, 0);
  GraphicsDevice::DrawTriStripBuffer(2, 4);
  GraphicsDevice::DrawTriStripBuffer(2, 8);

  GraphicsDevice::SetAlphaEnable(false);
  GraphicsDevice::SetDepthEnable(true);

  world.Pop();
}

unsigned AnchorVeil::GetDrawOrder(Camera const * cam)
{
  Matrix4 model_view = GraphicsDevice::GetViewStack().Top() * 
                       GraphicsDevice::GetWorldStack().Top();

  Vector4 origin = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
  origin.m_a[m_z_axis] = float(m_end_z_layer);
  origin = model_view * origin;

  float camera_depth = origin.m_z;

  float depth = (camera_depth - cam->GetNear()) / (cam->GetFar() - cam->GetNear());

  return unsigned(UINT_MAX * depth); // return the normalized depth
}

void AnchorVeil::Commit(TilePos3 end_position)
{
  m_committed = true;

  int max_anchor_range = g_engine_core->SystemCall("TileSystem", "GetAnchorRange");

  if (max_anchor_range > abs(int(end_position.m_a[m_x_axis] + 1.0f) - m_start_x_layer))
    m_end_x_layer = int(end_position.m_a[m_x_axis] + 1.0f);
}

}