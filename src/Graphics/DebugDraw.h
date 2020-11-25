// Nicholas Baldwin
// 2011/08/05
// All content (c) 2011 DigiPen (USA) Corporation, all rights reserved.

#pragma once
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Quaternion.h"
#include "../Math/Color.h"
#include "GraphicsHandleTypes.h"

namespace Improbability
{

class DebugDraw
{
public:
  // these are the types of shapes that can be drawn (exception DS_Max)
  enum DebugShape
  {
    DS_Arrow,
    DS_Coordinates,
    DS_Quad,
    DS_Cube,
    DS_CircleXY,
    DS_CircleYZ,
    DS_CircleXZ,

    DS_Max,

    DS_Sphere
  };

  // 
  struct DebugDrawObject
  {
    Vector3 *m_position;
    Matrix4 *m_orientation;
    Vector3 *m_scale;
    Color m_color;
    float m_lifetime;
  };

  // these calls add the various debug shapes to the draw queue which is cleared
  // after each frame
  // position - the start position for the arrow, origin for the coordinates
  //            and center for quad and cube
  // alignment_vector - the (normalized) vector you want to align the arrow to
  // rotation - the quaternion that describes the rotation you want
  // scale    - the scale you want to apply to the shape, later we will have
  //            non-uniform scaling
  static DebugDrawObject* AddArrow(Vector3 position, Vector3 alignment_vector, float scale);
  static DebugDrawObject* AddCoordinates(Vector3 position, Matrix4 const &rotation, float scale);
  static DebugDrawObject* AddQuad(Vector3 position, Matrix4 const &rotation, Vector2 scale);
  static DebugDrawObject* AddCube(Vector3 poistion, Matrix4 const &rotation, Vector3 scale);

  //static DebugDrawObject* RegisterObject(String const &name, DebugShape shape);

  static void ToggleDraw() { m_draw_enabled = !m_draw_enabled; }

public:
  // only used by the GraphicsSystem

  // must be called before anything else
  static void Initialize();
  // draws everything in the draw queue
  static void DrawQueue(float dt);
  // draws a specific shape immediately
  static void Draw(DebugShape shape_type);
  // must be called before closing to deallocate all buffers
  static void Uninitialize();

private:
  // is set to true when debug draw is initialized
  static bool m_is_initialized;

  static bool m_draw_enabled;

  // the buffer handles for each debug shape
  static BufferHandle m_debug_shapes[DS_Max];

  // the handle to the vertex declaration used by the shapes
  static DeclarationHandle m_declaration;

  // each shape uses this vertex structure
  struct Vertex
  {
    Vector3 m_position;
    X8R8G8B8 m_color;
  };

  // the draw queue for debug draw shapes
  static std::vector<DebugDrawObject*> m_static_queue[DS_Max];
  static HashMap<String, DebugDrawObject*> m_registered_queue[DS_Max];
};

}