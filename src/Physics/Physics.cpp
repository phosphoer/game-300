// David Evans
// 2011/07/21
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Physics.h"
#include "DynamicComponent.h"
#include "../Tile/TileComponent.h"
#include "../Tile/Tilemap.h"
#include "../Composition/ObjectManager.h"
#include "../Core/EngineCore.h"
#include "../Graphics/DebugDraw.h"
#include "../Graphics/Camera.h"
#include "../Math/Matrix4.h"
#include "../FileIO/Serializer.h"

namespace Improbability
{

MakeMeta(Physics);

void Physics::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnObjectInitialized);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnObjectDeleted);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnHelpActivated);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAxisChange);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAxisChangeComplete);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnPlayerDeathToggle);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAnchorSmushBegin);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAnchorSmushComplete);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAnchorEvent);
}

Physics::Physics():
  m_collision_layer_name("Collision"),
  m_gravity(0, 0, 0),
  m_tile_draw_dist(5),
  m_max_velocity(0),
  m_dt(0),
  m_helping(false),
  m_frame_delay(false),
  m_dead(false),
  m_rotating(false),
  m_smushing(false)
{
}

void Physics::InitializeGlobal()
{
}

void Physics::InitializeInstance()
{
  g_engine_core->AddEventListener(MessageBank::OnObjectInitialized, this);
  g_engine_core->AddEventListener(MessageBank::OnObjectDeleted, this);
  g_engine_core->AddEventListener(MessageBank::OnHelpActivated, this);
  g_engine_core->AddEventListener(MessageBank::OnAxisChange, this);
  g_engine_core->AddEventListener(MessageBank::OnAxisChangeComplete, this);
  g_engine_core->AddEventListener(MessageBank::OnPlayerDeathToggle, this);
  g_engine_core->AddEventListener(MessageBank::OnAnchorSmushBegin, this);
  g_engine_core->AddEventListener(MessageBank::OnAnchorSmushComplete, this);
  g_engine_core->AddEventListener(MessageBank::OnAnchorEvent, this);

  m_dt = 0.0f;
}

void Physics::UpdateInstance(float dt)
{
  // Only update when we have accumulated enough dt
  m_dt += dt;
  while (m_dt >= 0.016f)
  {
    // Remove the 16 ms that we are about to update
    m_dt -= 0.016f;

    // Update physics with a fixed timestep
    UpdateTileComponents(0.016f);
    UpdateDynamicComponents(0.016f);

    //m_dt = 0.0f;
  }

  // Draw axis
  Matrix4 matrix;
  matrix.Identity();
  DebugDraw::AddCoordinates(Vector3(0, 0, 0), matrix, 2.0f);

  // Debug draw everything
  //DebugDrawTilemaps();
  DebugDrawSlicedTilemaps();
  DebugDrawDynamicComponents();
  DebugDrawTileComponents();
}

void Physics::UninitializeInstance()
{
  g_engine_core->RemoveEventListener(MessageBank::OnObjectInitialized, this);
  g_engine_core->RemoveEventListener(MessageBank::OnObjectDeleted, this);
  g_engine_core->RemoveEventListener(MessageBank::OnHelpActivated, this);
  g_engine_core->RemoveEventListener(MessageBank::OnAxisChange, this);
  g_engine_core->RemoveEventListener(MessageBank::OnAxisChangeComplete, this);
  g_engine_core->RemoveEventListener(MessageBank::OnPlayerDeathToggle, this);
  g_engine_core->RemoveEventListener(MessageBank::OnAnchorSmushBegin, this);
  g_engine_core->RemoveEventListener(MessageBank::OnAnchorSmushComplete, this);
  g_engine_core->RemoveEventListener(MessageBank::OnAnchorEvent, this);
}

void Physics::UninitializeGlobal()
{
}

void Physics::Copy(System *dest, bool deep)
{
  if (deep)
  {
    Physics *other = reinterpret_cast<Physics*>(dest);
  }
}

void Physics::Deserialize(Serializer &serializer)
{
  Read(serializer, m_gravity, "Gravity");
  Read(serializer, m_tile_draw_dist, "TileDebugDrawDistance");
  Read(serializer, m_max_velocity, "MaxVelocity");
  Read(serializer, m_collision_layer_name, "CollisionLayerName");
}

void Physics::OnObjectInitialized(CompositionId id)
{
  ErrorIf(m_dynamic_components.Find(id) != m_dynamic_components.End(), "Physics::OnObjectInitialized: Given a duplicate CompositionId");
  ErrorIf(m_tile_components.Find(id) != m_tile_components.End(), "Physics::OnObjectInitialized: Given a duplicate CompositionId");

  Composition *obj = g_engine_core->SystemCall("ObjectManager", "GetObject", id);
  DynamicComponent *dyn = obj->GetComponent<DynamicComponent>();
  TileComponent *tile = obj->GetComponent<TileComponent>();

  if (dyn)
    m_dynamic_components[id] = dyn;

  if (tile)
    m_tile_components[id] = tile;
}

void Physics::OnObjectDeleted(CompositionId id)
{
  m_dynamic_components.Erase(id);
  m_tile_components.Erase(id);
}

void Physics::UpdateDynamicComponents(float dt)
{
  if(m_rotating || m_dead || m_smushing)
    return;

  // Update dynamic components, integrate position / orientation
  auto it = m_dynamic_components.Begin();
  auto end = m_dynamic_components.End();
  for (; it != end; ++it)
  {
    DynamicComponent *dyn = it->second;
    Composition *obj = dyn->GetParent();

    // Apply gravity
    if(!dyn->IsJumping())
    {
      dyn->m_velocity += m_gravity * dt;

    // The component has to know what the force of gravity is
    // in order to counteract it when it comes to rest
      dyn->m_gravity = m_gravity;
    }

    // Accelerate by acceleration vector
    bool grounded = dyn->GetSibling<TileComponent>()->IsGrounded();
    float horizDampener = grounded ? 1.f : .25f;
    /*dyn->m_velocity += dyn->GetAccelerationVector() * dyn->GetAcceleration() * horizDampener * dt;
    dyn->SetAccelerationVector(Vector3(0,0,0));*/
    Vector3 currHoriz = dyn->GetVelocity();
    Vector3 horizVel = dyn->GetHorizontalVelocity();
    float jumpDir = dyn->GetJumpDirection();
    //check x-axis
    if (horizVel.m_x)
    {
      if (horizVel.m_x > 0)
      {
        // accelerate
        if (currHoriz.m_x < horizVel.m_x)
        {
          dyn->m_velocity.m_x += dyn->GetAcceleration() * horizDampener * dt;
        }
        else if(currHoriz.m_x > horizVel.m_x)
          dyn->m_velocity.m_x = horizVel.m_x;
      }
      else
      {
        // accelerate
        if (currHoriz.m_x > horizVel.m_x)
        {
          dyn->m_velocity.m_x -= dyn->GetAcceleration() * horizDampener * dt;
        }
        else if(currHoriz.m_x < horizVel.m_x)
          dyn->m_velocity.m_x = horizVel.m_x;
      }
    }
    else if(!grounded)
    {
      if (currHoriz.m_x < 0)
      {
        dyn->m_velocity.m_x += dyn->GetAcceleration() * horizDampener/4 * dt;
      }
      else if (currHoriz.m_x > 0)
      {
        dyn->m_velocity.m_x -= dyn->GetAcceleration() * horizDampener/4 * dt;
      }
    }

    if(horizVel.m_z)
    {
      if (horizVel.m_z > 0)
      {
        // accelerate
        if (currHoriz.m_z < horizVel.m_z)
        {
          dyn->m_velocity.m_z += dyn->GetAcceleration() * horizDampener * dt;
        }
        else if(currHoriz.m_z > horizVel.m_z)
          dyn->m_velocity.m_z = horizVel.m_z;
      }
      else
      {
        // accelerate
        if (currHoriz.m_z > horizVel.m_z)
        {
          dyn->m_velocity.m_z -= dyn->GetAcceleration() * horizDampener * dt;
        }
        else if(currHoriz.m_z < horizVel.m_z)
          dyn->m_velocity.m_z = horizVel.m_z;
      }
    }
    else if(!grounded)
    {
      if (currHoriz.m_z < 0)
      {
        dyn->m_velocity.m_z += dyn->GetAcceleration() * horizDampener/4 * dt;
      }
      else if (currHoriz.m_z > 0)
      {
        dyn->m_velocity.m_z -= dyn->GetAcceleration() * horizDampener/4 * dt;
      }
    }
    dyn->SetHorizontalVelocity(Vector3(0,0,0));
    // Cap velocity
    Vector3 v = dyn->GetVelocity();
    //float curr_max_velocity = grounded ? m_max_velocity : dyn->GetJumpVelocity();
    if (v.m_y <= 0 && v.Magnitude() > m_max_velocity)
    {
      v.Normalize();
      v *= m_max_velocity;
      dyn->SetVelocity(v);
    }

    // Integrate position and orientation;
    obj->AddPosition(dyn->GetVelocity() * dt); 
  }
}

void Physics::UpdateTileComponents(float dt)
{
  //if(m_rotating)
  //  return;
  // Get tilemap info

  Tilemap *tilemap = g_engine_core->SystemCall("TileSystem", "GetTilemaps", "Collision");
  Tilemap *head = tilemap;

  // Iterate over tile components
  auto it = m_tile_components.Begin();
  auto end = m_tile_components.End();
  for (; it != end; ++it)
  {
    // Set the axis to our global axis
    TileComponent *tc = it->second;

    //If In help mode, override tilemap
    /*if(m_helping)
    {
      tilemap = g_engine_core->SystemCall("TileSystem", "GetTilemaps", "Helping");
    }*/

    // Iterate over every collision tilemap
    for (tilemap = head; tilemap; tilemap = tilemap->GetNext())
    {
      tc->Update(tilemap, dt);
    }
  }
}

void Physics::DebugDrawSlicedTilemaps()
{
  // Get tilemap
  Tilemap *tilemap = g_engine_core->SystemCall("TileSystem", "GetTilemaps", m_collision_layer_name);

  // Get area to draw
  TilePos3 begin = g_engine_core->SystemCall("TileSystem", "GetFocusBegin");
  TilePos3 end = g_engine_core->SystemCall("TileSystem", "GetFocusEnd");

  // Draw the tiles
  Matrix4 matrix;
  matrix.Identity();
  for (; tilemap; tilemap = tilemap->GetNext())
    for (int z = begin.m_z; z < end.m_z; ++z)
      for (int y = begin.m_y; y < end.m_y; ++y)
        for (int x = begin.m_x; x < end.m_x; ++x)
          if (tilemap->GetTile(x, y, z) == 1)
            DebugDraw::AddCube(Vector3(x + 0.5f, y + 0.5f, z + 0.5f), matrix, Vector3(1.0f, 1.0f, 1.0f));
}

void Physics::DebugDrawTilemaps()
{
  // Get tilemap
  Tilemap *tilemap = g_engine_core->SystemCall("ObjectManager", "GetTilemaps", m_collision_layer_name);

  // Get area to draw
  Composition *camera = g_engine_core->SystemCall("ObjectManager", "GetObjectByName", "Camera");
  Vector3 cam_pos = camera->GetComponent<Camera>()->GetTarget();
  int xbegin = static_cast<int>(cam_pos.m_x) - m_tile_draw_dist;
  int ybegin = static_cast<int>(cam_pos.m_y) - m_tile_draw_dist;
  int zbegin = static_cast<int>(cam_pos.m_z) - m_tile_draw_dist;
  int xend = xbegin + m_tile_draw_dist * 2;
  int yend = ybegin + m_tile_draw_dist * 2;
  int zend = zbegin + m_tile_draw_dist * 2;
  if (xbegin < 0) xbegin = 0; if (xend > tilemap->GetWidth()) xend = tilemap->GetWidth();
  if (ybegin < 0) ybegin = 0; if (yend > tilemap->GetHeight()) yend = tilemap->GetHeight();
  if (zbegin < 0) zbegin = 0; if (zend > tilemap->GetDepth()) zend = tilemap->GetDepth();
  
  // Draw tilemaps
  Matrix4 matrix;
  matrix.Identity();
  for (; tilemap; tilemap = tilemap->GetNext())
    for (int z = zbegin; z < zend; ++z)
      for (int y = ybegin; y < yend; ++y)
        for (int x = xbegin; x < xend; ++x)
          if (tilemap->GetTile(x, y, z) == 1)
            DebugDraw::AddCube(Vector3(x + 0.5f, y + 0.5f, z + 0.5f), matrix, Vector3(1.0f, 1.0f, 1.0f));
}

void Physics::DebugDrawDynamicComponents()
{
  auto it = m_dynamic_components.Begin();
  auto end = m_dynamic_components.End();
  for (; it != end; ++it)
  {
    DynamicComponent *dyn = it->second;
    dyn->DebugDraw();
  }
}

void Physics::DebugDrawTileComponents()
{
  auto it = m_tile_components.Begin();
  auto end = m_tile_components.End();
  for (; it != end; ++it)
  {
    TileComponent *tile = it->second;
    tile->DebugDraw();
  }
}

void Physics::OnHelpActivated()
{
  //m_helping = true;
}

void Physics::OnAxisChange( int old_axis, int new_axis )
{
  m_rotating = true;
}

void Physics::OnAxisChangeComplete( int axis )
{
  m_rotating = false;
}

void Physics::OnAnchorSmushBegin()
{
  m_smushing = true;
}
void Physics::OnAnchorSmushComplete()
{
  m_smushing = false;
}

void Physics::OnPlayerDeathToggle(bool toggle)
{
  m_dead = toggle;
}

void Physics::OnAnchorEvent(int anchor_event_type)
{
  if (anchor_event_type == 2) // clear
  {
    m_smushing = false;
  }
}

}