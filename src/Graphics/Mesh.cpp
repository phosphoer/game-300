// Nicholas Baldwin
// 2011/09/19
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Mesh.h"
#include "../FileIO/Serializer.h"
#include "GraphicsDevice.h"
#include <stdint.h>

namespace Improbability
{

void Mesh::Set()
{
  GraphicsDevice::SetVertexBuffer(m_mesh_handle);
}

void Mesh::Add(Matrix4 const &transform)
{
// I will actually create this function when I do instancing
//  GraphicsDevice::AddInstanceData(m_mesh_handle, transform);
}

void Mesh::Draw()
{
  GraphicsDevice::DrawIndexedTriList(m_num_verts, m_num_tris);
}

void * Mesh::Lock(unsigned offset, unsigned size)
{
  return GraphicsDevice::LockVertexBuffer(m_mesh_handle);
}

MakeMeta(TexturedMesh);

void TexturedMesh::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

void TexturedMesh::Load(char const *filename)
{
  GraphicsVertexTypes::VertexElement vertex_definition[3];

  vertex_definition[0].m_stream = 
    vertex_definition[1].m_stream = 
    vertex_definition[2].m_stream = 0;

  vertex_definition[0].m_type = 
    vertex_definition[1].m_type = GraphicsVertexTypes::VM_Vector3;
  vertex_definition[2].m_type = GraphicsVertexTypes::VM_Vector2;

  vertex_definition[0].m_usage = GraphicsVertexTypes::MU_Position;
  vertex_definition[1].m_usage = GraphicsVertexTypes::MU_Normal;
  vertex_definition[2].m_usage = GraphicsVertexTypes::MU_TexCoord;

  m_declaration_handle = GraphicsDevice::CreateVertexDeclaration(vertex_definition, 3);

  Serializer serializer(filename);

  int num_verts, num_indexes;

  serializer.EnterElement("mesh");
  serializer.EnterElement("index_buffer");
  serializer.ReadIntAttribute(num_indexes, "index_count");
  serializer.LeaveElement();

  m_num_tris = num_indexes / 3;

  serializer.EnterElement("vertex_buffer");
  serializer.ReadIntAttribute(num_verts, "vertex_count");

  m_num_verts = num_verts;

  m_mesh_handle = GraphicsDevice::CreateVertexBuffer(num_verts, num_indexes, 
                                                     false, m_declaration_handle);

  Vertex *buffer = reinterpret_cast<Vertex*>(GraphicsDevice::LockVertexBuffer(m_mesh_handle));

  serializer.Begin();
  int i = 0;
  for (; serializer.IsGood() && i < num_verts; serializer.Iterate(), ++i)
  {
    serializer.EnterElement("position");
    ReadAttribute(serializer, buffer[i].m_position.m_x, "x");
    ReadAttribute(serializer, buffer[i].m_position.m_y, "y");
    ReadAttribute(serializer, buffer[i].m_position.m_z, "z");
    serializer.LeaveElement();
    serializer.EnterElement("normal");
    ReadAttribute(serializer, buffer[i].m_normal.m_x, "x");
    ReadAttribute(serializer, buffer[i].m_normal.m_y, "y");
    ReadAttribute(serializer, buffer[i].m_normal.m_z, "z");
    serializer.LeaveElement();
    serializer.EnterElement("texcoord");
    ReadAttribute(serializer, buffer[i].m_texture_coordinates.m_x, "u");
    ReadAttribute(serializer, buffer[i].m_texture_coordinates.m_y, "v");
    serializer.LeaveElement();
  }

  GraphicsDevice::UnlockVertexBuffer(m_mesh_handle);

  serializer.LeaveElement();
  serializer.LeaveElement();

  serializer.EnterElement("index_buffer");

  int16_t *indexes = reinterpret_cast<int16_t*>(GraphicsDevice::LockIndexBuffer(m_mesh_handle));
  int temp;

  serializer.Begin();
  i = 0;
  for (; serializer.IsGood() && i < num_indexes; serializer.Iterate(), i += 3)
  {
    ReadAttribute(serializer, temp, "v1");
    indexes[i+0] = (int16_t)temp;
    ReadAttribute(serializer, temp, "v2");
    indexes[i+1] = (int16_t)temp;
    ReadAttribute(serializer, temp, "v3");
    indexes[i+2] = (int16_t)temp;
  }

  GraphicsDevice::UnlockIndexBuffer(m_mesh_handle);

  serializer.LeaveElement();
  serializer.LeaveElement();
  serializer.LeaveElement();
}

void TexturedMesh::Unload()
{
  GraphicsDevice::DeleteVertexBuffer(m_mesh_handle);
  GraphicsDevice::DeleteVertexDeclaration(m_declaration_handle);
}

TexturedMesh::Vertex* TexturedMesh::GetBuffer()
{
  return reinterpret_cast<Vertex*>(GraphicsDevice::LockVertexBuffer(m_mesh_handle));
}

}