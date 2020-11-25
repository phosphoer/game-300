// Nicholas Baldwin
// 2011/10/25
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "OgreMesh.h"
#include "GraphicsVertexTypes.h"
#include "GraphicsDevice.h"
#include "..\FileIO\Serializer.h"
#include <stdint.h>

namespace Improbability
{

MakeMeta(OgreMesh);

void OgreMesh::BindMeta(MetaComposition* comp)
{
  comp->MetaAddComponent(MetaNewable);
}

void OgreMesh::Load(char const *filename)
{
  GraphicsVertexTypes::VertexElement vertex_definition[4];

  vertex_definition[0].m_stream = 
    vertex_definition[1].m_stream = 
    vertex_definition[2].m_stream = 
    vertex_definition[3].m_stream = 0;

  vertex_definition[0].m_type = 
    vertex_definition[1].m_type = GraphicsVertexTypes::VM_Vector3;
  vertex_definition[2].m_type = GraphicsVertexTypes::VM_Vector2;
  vertex_definition[3].m_type = GraphicsVertexTypes::VM_Float;

  vertex_definition[0].m_usage = GraphicsVertexTypes::MU_Position;
  vertex_definition[1].m_usage = GraphicsVertexTypes::MU_Normal;
  vertex_definition[2].m_usage = GraphicsVertexTypes::MU_TexCoord;
  vertex_definition[3].m_usage = GraphicsVertexTypes::MU_BlendIndex;

  m_declaration_handle = GraphicsDevice::CreateVertexDeclaration(vertex_definition, 4);

  Serializer serializer(filename);
  int num_indexes, num_tris, num_verts;

  serializer.EnterElement("submeshes");
  serializer.EnterElement("submesh");
  serializer.EnterElement("faces");
  ReadAttribute(serializer, num_tris, "count");
  serializer.LeaveElement();
  serializer.LeaveElement();
  serializer.LeaveElement();

  m_num_tris = num_tris;
  num_indexes = m_num_tris * 3;

  serializer.EnterElement("sharedgeometry");
  ReadAttribute(serializer, num_verts, "vertexcount");
  serializer.EnterElement("vertexbuffer");

  m_num_verts = num_verts;

  m_mesh_handle = GraphicsDevice::CreateVertexBuffer(m_num_verts, num_indexes, false,
                                                     m_declaration_handle);

  Vertex *buffer = reinterpret_cast<Vertex*>(GraphicsDevice::LockVertexBuffer(m_mesh_handle));

  serializer.Begin();
  unsigned i = 0;
  for (; serializer.IsGood() && i < m_num_verts; serializer.Iterate(), ++i)
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

  serializer.LeaveElement();
  serializer.LeaveElement();
  serializer.LeaveElement();
  serializer.EnterElement("boneassignments");

  int index, bone;
  for (serializer.Begin(); serializer.IsGood(); serializer.Iterate())
  {
    ReadAttribute(serializer, index, "vertexindex");
    ReadAttribute(serializer, bone, "boneindex");
    buffer[index].m_bone = static_cast<float>(bone);
  }

  serializer.LeaveElement();
  serializer.LeaveElement();

  GraphicsDevice::UnlockVertexBuffer(m_mesh_handle);

  serializer.EnterElement("submeshes");
  serializer.EnterElement("submesh");
  serializer.EnterElement("faces");

  int16_t *indexes = reinterpret_cast<int16_t*>(GraphicsDevice::LockIndexBuffer(m_mesh_handle));
  int temp;

  serializer.Begin();
  i = 0;
  for (; serializer.IsGood() && i < static_cast<unsigned>(num_indexes); serializer.Iterate(), i += 3)
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
  serializer.LeaveElement();
}

void OgreMesh::Unload()
{
  GraphicsDevice::DeleteVertexBuffer(m_mesh_handle);
  GraphicsDevice::DeleteVertexDeclaration(m_declaration_handle);
}

OgreMesh::Vertex* OgreMesh::GetBuffer()
{
  return reinterpret_cast<Vertex*>(GraphicsDevice::LockVertexBuffer(m_mesh_handle));
}

}