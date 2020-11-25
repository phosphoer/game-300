// Nicholas Baldwin
// 2011/06/29
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "Precompiled.h"

namespace Improbability
{

class GraphicsVertexTypes
{
public:
  // describes the type of the member
  enum MemberType
  {
    VM_Float,
    VM_Vector2,
    VM_Vector3,
    VM_Vector4,
    VM_Color,

    VM_Max
  };

  // describes the usage of the member for the shader
  enum MemberUsage
  {
    MU_Position,
    MU_Normal,
    MU_TexCoord,
    MU_Color,
    MU_Tangent,
    MU_Bitangent,
    MU_BlendIndex,

    MU_Max
  };

  // describes one element of a vertex
  struct VertexElement
  {
    unsigned m_stream;
    MemberType m_type;
    MemberUsage m_usage;

    bool operator!=(VertexElement const & rhs)
    {
      return (m_stream != rhs.m_stream) || (m_type != rhs.m_type) || (m_usage != rhs.m_usage);
    }
  };

}; // class GraphicsVertexTypes

} // namespace Improbability