// David Evans
// 2011/07/23
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "TileComponent.h"
#include "TileSystem.h"
#include "../FileIO/Serializer.h"
#include "../Composition/Composition.h"
#include "../Physics/DynamicComponent.h"
#include "../Graphics/DebugDraw.h"
#include "../Math/Matrix4.h"
#include "../Core/EngineCore.h"
#include "TileGameplayComponent.h"
#include "../Graphics/SpriteComponent.h"
#include "../Tile/World.h"

namespace Improbability
{

MakeMeta(TileComponent);

void TileComponent::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaAddComponent(MetaFieldable);
  comp->MetaGetComponent(MetaFieldable)->MetaRegisterField(int, m_axis);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAxisChange);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAxisChangeComplete);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnGrounded);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnKeyPress);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAnchorTileEnter);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnRotateStart);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnRotateComplete);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAxisInitalize);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnPlayerDrop);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAnchorEvent);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnHUDReady);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnRecieveControllerConfig);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GetTileComponent);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(CoinCollected);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(SetPosition);
}

TileComponent::TileComponent():
  m_initialized(false),
  m_dt(0),
  m_width(1),
  m_height(1),
  m_depth(1),
  m_fwidth(.75f),
  m_fheight(1.f),
  m_fdepth(.75f),
  m_is_grounded(false),
  m_offset(0.0f, 0.0f, 0.0f),
  m_layer(0),
  m_axis(0),
  m_prev_axis(0),
  m_anchor_enabled(false),
  m_anchor_axis(-1),
  m_anchor_begin(-1),
  m_anchor_end(-1),
  m_anchor_range(0),
  m_anchor_commit(false),
  m_drop(false),
  m_drop_y(0),
  m_obscured(-1),
  m_rotating(false),
  m_grounded_tile_change(false),
  m_tile_change(false),
  m_tile_pos(TilePos3(0,0,0)),
  m_last_tile_pos(TilePos3(0,0,0)),
  m_last_grounded_pos(TilePos3(0,0,0)),
  m_last_safe_pos(TilePos3(0,0,0)),
  m_grounded_pos(TilePos3(0,0,0)),
  m_memorySize(8),
  m_currMemorySize(0),
  m_maxAge(10.f),
  m_facing(RIGHT),
  m_player_animation_count(0),
  m_frame_delay(.117f),
  m_fall_max(5.f),
  m_levitate(false),
  m_left_space(false),
  m_dead(false),
  m_deadCount(0.f),
  m_silent_anchor(false),
  m_warping(false),
  m_leveling(false),
  m_tutorial(false),
  m_narrating(false),
  m_narrate_count(0.f),
  m_prompt_continue(false),
  m_skip_text(false)
{
}

  TileComponent::TileComponent(TileComponent const &rhs):
  m_initialized(rhs.m_initialized),
  m_dt(0),
  m_width(rhs.m_width),
  m_height(rhs.m_height),
  m_depth(rhs.m_depth),
  m_fwidth(rhs.m_fwidth),
  m_fheight(rhs.m_fheight),
  m_fdepth(rhs.m_fdepth),
  m_is_grounded(false),
  m_offset(rhs.m_offset),
  m_layer(rhs.m_layer),
  m_axis(rhs.m_axis),
  m_anchor_enabled(rhs.m_anchor_enabled),
  m_anchor_commit(rhs.m_anchor_commit),
  m_anchor_axis(rhs.m_anchor_axis),
  m_anchor_begin(rhs.m_anchor_begin),
  m_anchor_end(rhs.m_anchor_end),
  m_anchor_range(rhs.m_anchor_range),
  m_tile_change(rhs.m_tile_change),
  m_grounded_tile_change(rhs.m_grounded_tile_change),
  m_drop(rhs.m_drop),
  m_drop_y(0),
  m_obscured(false),
  m_rotating(false),
  m_memorySize(rhs.m_memorySize),
  m_currMemorySize(rhs.m_currMemorySize),
  m_maxAge(rhs.m_maxAge),
  m_facing(rhs.m_facing),
  m_player_animation_count(rhs.m_player_animation_count),
  m_frame_delay(rhs.m_frame_delay),
  m_fall_max(rhs.m_fall_max),
  m_levitate(rhs.m_levitate),
  m_left_space(rhs.m_left_space),
  m_dead(rhs.m_dead),
  m_deadCount(rhs.m_deadCount),
  m_silent_anchor(rhs.m_silent_anchor),
  m_warping(rhs.m_warping),
  m_leveling(rhs.m_leveling),
  m_tutorial(rhs.m_tutorial),
  m_narrating(rhs.m_narrating),
  m_narrate_count(rhs.m_narrate_count),
  m_prompt_continue(rhs.m_prompt_continue),
  m_skip_text(rhs.m_skip_text),
  m_hud_ready(false),
  m_constant_animation_count(0.f)
{
}

void TileComponent::Initialize()
{
  // Calculate hotspot offsets
  float s = 0.75f;
  float w = m_fwidth;//static_cast<float>(m_width);
  float h = m_fheight;
  float d = m_fdepth;//static_cast<float>(m_depth);
  float size = 0.5f;

  // Boundaries of hotspots
  // The left, bottom, and front ones need an epsilon to put them inside
  // the adjacent tile since 0 is in the first tile but 1 is in the second tile
  float top = h + m_offset.m_y - h*0.50f;
  float bottom = m_offset.m_y - h*0.501f;
  float left = m_offset.m_x - w*0.501f;
  float right = w + m_offset.m_x - w*0.50f;
  float front = m_offset.m_z - d*0.501f;
  float back = d + m_offset.m_z - d*0.50f;

  // xy plane
  for (int i = 0; i < m_height; ++i)
  {
    float fi = static_cast<float>(i);
    m_hs_hor[2].push_back(Vector3(left, m_offset.m_y + fi - h*size * s, 0.0f));
    m_hs_hor[2].push_back(Vector3(left, m_offset.m_y + fi + h*size * s, 0.0f));
    m_hs_hor[2].push_back(Vector3(right, m_offset.m_y + fi - h*size * s, 0.0f));
    m_hs_hor[2].push_back(Vector3(right, m_offset.m_y + fi + h*size * s, 0.0f));
  }
  for (int i = 0; i < m_width; ++i)
  {
    float fi = static_cast<float>(i);
    m_hs_ver[2].push_back(Vector3(m_offset.m_x + fi - w*size * s, top, 0.0f));
    m_hs_ver[2].push_back(Vector3(m_offset.m_x + fi + w*size * s, top, 0.0f));
    m_hs_ver[2].push_back(Vector3(m_offset.m_x + fi - w*size * s, bottom, 0.0f));
    m_hs_ver[2].push_back(Vector3(m_offset.m_x + fi + w*size * s, bottom, 0.0f));
  }

  // zy plane
  for (int i = 0; i < m_height; ++i)
  {
    float fi = static_cast<float>(i);
    m_hs_hor[0].push_back(Vector3(0.0f, m_offset.m_y + fi - h*size * s, front));
    m_hs_hor[0].push_back(Vector3(0.0f, m_offset.m_y + fi + h*size * s, front));
    m_hs_hor[0].push_back(Vector3(0.0f, m_offset.m_y + fi - h*size * s, back));
    m_hs_hor[0].push_back(Vector3(0.0f, m_offset.m_y + fi + h*size * s, back));
  }
  for (int i = 0; i < m_depth; ++i)
  {
    float fi = static_cast<float>(i);
    m_hs_ver[0].push_back(Vector3(0.0f, top, m_offset.m_z + fi - d*size * s));
    m_hs_ver[0].push_back(Vector3(0.0f, top, m_offset.m_z + fi + d*size * s));
    m_hs_ver[0].push_back(Vector3(0.0f, bottom, m_offset.m_z + fi - d*size * s));
    m_hs_ver[0].push_back(Vector3(0.0f, bottom, m_offset.m_z + fi + d*size * s));
  }

  // xz plane
  for (int i = 0; i < m_depth; ++i)
  {
    float fi = static_cast<float>(i);
    m_hs_hor[1].push_back(Vector3(left, 0.0f, m_offset.m_z + fi - d*size * s));
    m_hs_hor[1].push_back(Vector3(left, 0.0f, m_offset.m_z + fi + d*size * s));
    m_hs_hor[1].push_back(Vector3(right, 0.0f, m_offset.m_z + fi - d*size * s));
    m_hs_hor[1].push_back(Vector3(right, 0.0f, m_offset.m_z + fi + d*size * s));
  }
  for (int i = 0; i < m_width; ++i)
  {
    float fi = static_cast<float>(i);
    m_hs_ver[1].push_back(Vector3(m_offset.m_x + fi - w*size * s, 0.0f, front));
    m_hs_ver[1].push_back(Vector3(m_offset.m_x + fi + w*size * s, 0.0f, front));
    m_hs_ver[1].push_back(Vector3(m_offset.m_x + fi - w*size * s, 0.0f, back));
    m_hs_ver[1].push_back(Vector3(m_offset.m_x + fi + w*size * s, 0.0f, back));
  }

  g_engine_core->AddEventListener(MessageBank::OnAxisChange, this);
  g_engine_core->AddEventListener(MessageBank::OnAxisChangeComplete, this);
  g_engine_core->AddEventListener(MessageBank::OnKeyPress, this);
  g_engine_core->AddEventListener(MessageBank::OnAnchorTileEnter, this);
  g_engine_core->AddEventListener(MessageBank::OnRotateStart, this);
  g_engine_core->AddEventListener(MessageBank::OnRotateComplete, this);
  g_engine_core->AddEventListener(MessageBank::OnAxisInitalize, this);
  g_engine_core->AddEventListener(MessageBank::OnPlayerDrop, this);
  g_engine_core->AddEventListener(MessageBank::OnAnchorEvent, this);
  g_engine_core->AddEventListener(MessageBank::OnHUDReady, this);
  g_engine_core->AddEventListener(MessageBank::OnRecieveControllerConfig, this);
  g_engine_core->AddEventListener(MessageBank::GetTileComponent, this);
  g_engine_core->AddEventListener(MessageBank::CoinCollected, this);
  g_engine_core->AddEventListener(MessageBank::SetPosition, this);

  g_engine_core->DispatchEvent("ReturnTC", this);
}

void TileComponent::Uninitialize()
{
  g_engine_core->RemoveEventListener(MessageBank::OnAxisChange, this);
  g_engine_core->RemoveEventListener(MessageBank::OnAxisChangeComplete, this);
  g_engine_core->RemoveEventListener(MessageBank::OnKeyPress, this);
  g_engine_core->RemoveEventListener(MessageBank::OnAnchorTileEnter, this);
  g_engine_core->RemoveEventListener(MessageBank::OnRotateStart, this);
  g_engine_core->RemoveEventListener(MessageBank::OnRotateComplete, this);
  g_engine_core->RemoveEventListener(MessageBank::OnAxisInitalize, this);
  g_engine_core->RemoveEventListener(MessageBank::OnPlayerDrop, this);
  g_engine_core->RemoveEventListener(MessageBank::OnAnchorEvent, this);
  g_engine_core->RemoveEventListener(MessageBank::OnHUDReady, this);
  g_engine_core->RemoveEventListener(MessageBank::GetTileComponent, this);
  g_engine_core->RemoveEventListener(MessageBank::CoinCollected, this);
  g_engine_core->RemoveEventListener(MessageBank::OnRecieveControllerConfig, this);
  g_engine_core->RemoveEventListener(MessageBank::SetPosition, this);
}

Component* TileComponent::Clone()
{
  TileComponent *clone = new TileComponent(*this);
  return clone;
}

void TileComponent::Deserialize(Serializer &serializer)
{
  Read(serializer, m_width, "Width");
  Read(serializer, m_height, "Height");
  Read(serializer, m_depth, "Depth");
  Read(serializer, m_offset, "Offset");
  Read(serializer, m_anchor_range, "AnchorRange");
}

struct collisionDirection
{
  collisionDirection():UP(0),DOWN(0),RIGHT(0),LEFT(0){}
  int UP;
  int DOWN;
  int LEFT;
  int RIGHT;
};

void TileComponent::CheckCollisionInDirection( collisionDirection &data, Vector4 pos, Tilemap *tilemap )
{
  bool pos_complete = true;
  bool neg_complete = true;
  int size = m_hs_ver[m_axis].size();
  for (int i = 0; i < size; ++i)
  {
    // Get offset for this hotspot and object position
    Vector3 offset = m_hs_ver[m_axis][i];

    // Get tiles to the up and down of this one
    TilePos2 tilepos = Tilemap::GetTilePos(pos + offset, m_axis);
    TilePos2 tilepos_u = tilepos;
    TilePos2 tilepos_d = tilepos;
    tilepos_u.m_y += 1; tilepos_d.m_y -= 1;
    char tile_u = tilemap->GetTile(tilepos_u, m_axis, m_layer);
    char tile_d = tilemap->GetTile(tilepos_d, m_axis, m_layer);
    // If the tile at the hotspot is collision
    // We can't be colliding with it if the tiles to the up and down are
    // collision as well
    if (tilemap->GetTile(pos + offset) > 0 && (tile_u == 0 || tile_d == 0))
    {
      if (offset.m_y > 0 && !data.UP)
        data.UP = 2;
      else if(offset.m_y < 0 && !data.DOWN)
        data.DOWN = 2;
    }
    else
    {
      if(offset.m_y > 0)
        pos_complete = false;
      else if(offset.m_y < 0)
        neg_complete = false;
    }
  }
  if(!pos_complete && data.UP)
    data.UP = 1;
  if(!neg_complete && data.DOWN)
    data.DOWN = 1;

  size = m_hs_hor[m_axis].size();
  for (int i = 0; i < size; ++i)
  {
    // Get offset for this hotspot and object position
    Vector3 offset = m_hs_hor[m_axis][i];

    // Get tiles to the left and right of this one
    TilePos2 tilepos = Tilemap::GetTilePos(pos + offset, m_axis);
    TilePos2 tilepos_l = tilepos;
    TilePos2 tilepos_r = tilepos;
    tilepos_l.m_x -= 1; tilepos_r.m_x += 1;
    char tile_l = tilemap->GetTile(tilepos_l, m_axis, m_layer);
    char tile_r = tilemap->GetTile(tilepos_r, m_axis, m_layer);

    // If the tile at the hotspot is collision
    // We can't be colliding with it if the tiles to the left and right are
    // collision as well
    int horiz_axis = 0;
    int axisScalar = 1;
    if(m_axis == 0)
    {
      horiz_axis = 2;
      axisScalar = -1;
    }
    if (tilemap->GetTile(pos + offset) > 0 && (tile_l == 0 || tile_r == 0))
    {
      if (offset.m_a[horiz_axis]*axisScalar > 0 && !data.RIGHT)
        data.RIGHT = 2;
      else if(offset.m_a[horiz_axis]*axisScalar < 0 && !data.LEFT)
        data.LEFT = 2;
    }
    else
    {
      if(offset.m_a[horiz_axis]*axisScalar > 0)
        pos_complete = false;
      else if(offset.m_a[horiz_axis]*axisScalar < 0)
        neg_complete = false;
    }
  }

  if(!pos_complete && data.RIGHT)
    data.RIGHT = 1;
  if(!neg_complete && data.LEFT)
    data.LEFT = 1;
}
void TileComponent::Update(Tilemap *tilemap, float dt)
{
  m_dt = dt;
  // Calculate inhabited cells
  UpdateTilePos();

  // Get my position
  Vector4 pos4 = GetParent()->GetPosition();
  Vector3 pos = pos4.ToVector3();
  TilePos3 tpos = GetTilePos3();
  Vector3 offset(0.0f, -0.001f, 0.0f);

  // Test collision on my axis
  bool h_col = false;
  bool v_col = false;

  //if(m_rotating)
  //  return;

  if(m_left_space)
  {
    ReturnFromSpace();
  }
  if(g_curr_level && m_hud_ready)
  {
    //Narration Stuffs
    switch(g_curr_level)
    {
    case 20:
      if(!g_first_level)
      {
        g_first_level = true;
        HUDNarrativePrompt(LEVEL_ONE);
      }
      break;
    case 21:
      if(!g_second_level)
      {
        g_second_level = true;
        HUDNarrativePrompt(LEVEL_TWO);
      }
      break;
    case 24:
      if(!g_first_level)
      {
        g_first_level = true;
        HUDNarrativePrompt(LEVEL_ONE);
      }
      break;
    case 25:
      if(!g_second_level)
      {
        g_second_level = true;
        HUDNarrativePrompt(LEVEL_TWO);
      }
      break;
    case 22:
      if(!g_final_level)
      {
        g_final_level = true;
        HUDNarrativePrompt(FINAL_LEVEL);
      }
      break;
    }

    g_curr_level = 0;
  }
  if(m_narrating)
  {
    HUDNarrativePrompt();
  }
  if(m_dead || m_warping || m_leveling)
  {
    m_deadCount += dt;
    if(m_dead || m_warping)
    {
      if(m_deadCount > .5f)
      {
        if(m_dead)
        {
          m_dead = false;
          GetParent()->SetPosition(Vector4(m_last_safe_pos.m_x + .5f, m_last_safe_pos.m_y + .5f,m_last_safe_pos.m_z + .5f, 0));
          m_grounded_pos = m_last_safe_pos;
          GetParent()->GetComponent<DynamicComponent>()->SetVelocity(0,0,0);
          g_engine_core->DispatchEvent("OnPlayerDeathToggle", false);
        }
        else if(m_warping)
        {
          if(m_next_level == 23 && m_narrating)
            return;
          g_curr_level = m_next_level;
          m_left_space = true;
          GetParent()->GetComponent<DynamicComponent>()->SetVelocity(0,0,0);
          g_engine_core->DispatchEvent("OnPlayerDeathToggle", false);
          //g_engine_core->DispatchEvent("PushAudioEvent", "Stop_Audio");

          switch(m_next_level)
          {
          case 1:
            g_engine_core->PopSpace();
            break;
          case 20:
            g_engine_core->PushSpace("LevelOne-space.spc", false);
            break;
          case 21:
            g_engine_core->PushSpace("LevelTwo-space.spc", false);
            break;
          case 22:
            g_engine_core->PushSpace("LevelThree-space.spc", false);
            break;
          case 24:
            g_engine_core->PushSpace("LevelFour-space.spc", false);
            break;
          case 25:
            g_engine_core->PushSpace("LevelFive-space.spc", false);
            break;
          case 23:
            g_engine_core->DispatchEvent("PushAudioEvent", "Play_level_change");
            g_engine_core->PopSpace();
            break;
          }

          m_warping = false;
          m_next_level = 0;
          m_deadCount = 0.f;
        }
      }
      // flash player
      else if(m_deadCount > .20f)
      {
        int milliseconds = int(m_deadCount*1000);
        bool flash = milliseconds % 10 ? true : false;
        if(flash)
        {
          if(GetParent()->GetComponent<SpriteComponent>()->GetCurrentAnimation() == 10)
          {
            if(GetParent()->GetComponent<DynamicComponent>()->GetVelocity().m_y < 0)
              GetParent()->GetComponent<SpriteComponent>()->SetAnimation(m_axis ? (m_facing == LEFT ? 5 : 4) : (m_facing ==  DOWN ? 7 : 6));
            else if(m_dead)
              GetParent()->GetComponent<SpriteComponent>()->SetAnimation(m_facing == LEFT || m_facing == DOWN ? 9 : 8);
            else
              GetParent()->GetComponent<SpriteComponent>()->SetAnimation(m_axis ? (m_facing == LEFT ? 1 : 0) : (m_facing ==  DOWN ? 3 : 2));
          }
          else
            GetParent()->GetComponent<SpriteComponent>()->SetAnimation(10);
        }
      }
    }
    // levitate player
    else if(m_leveling)
    {
      if(m_deadCount > 3.f)
      {
        m_leveling = false;
        g_engine_core->DispatchEvent("OnPlayerDeathToggle", false);
        m_deadCount = 0.f;
        GetParent()->GetComponent<DynamicComponent>()->SetVelocity(0,0,0);
      }
      else if(m_deadCount < 2.f)
        GetParent()->SetPosition(GetParent()->GetPosition() + (m_levitate_rate * dt));
      GetParent()->GetComponent<SpriteComponent>()->SetAnimation(m_axis ? (m_facing == LEFT ? 5 : 4) : (m_facing ==  DOWN ? 7 : 6));
    }
    return;
  }


  //////////////////////////////////////////////////////////////////////////
  /////////////////////   COLLISION HANDLING    ////////////////////////////
  //////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////
  // Profile Perspective Collision Handling
  ////////////////////////////////////////////
  if (m_axis != WorldAxis::Top)
  {
    // Check collision on current layer first
    bool grounded = false;  
    bool l_space = true;
    bool r_space = true;
    bool u_space = true;
    bool change_layers = false;
    bool ground_clear = true;

    collisionDirection currData;
    CheckCollisionInDirection(currData, pos4, tilemap);
    if(currData.DOWN)
    {
      grounded = true;
      if(currData.DOWN == 1 && (currData.LEFT == 2 || currData.RIGHT == 2))
        grounded = false;
      m_apex = pos4.m_y;
    }
    if(m_drop && m_anchor_end > -1 && m_anchor_axis == m_axis)
      grounded = !grounded;
    l_space = !currData.LEFT;
    r_space = !currData.RIGHT;
    u_space = !currData.UP;

    if(!grounded && pos4.m_y > m_apex)
      m_apex = pos4.m_y;

    // If anchoring is in effect, check for ground on each anchored layer    
    if (m_anchor_begin < m_anchor_end) 
      std::swap(m_anchor_begin, m_anchor_end);

    collisionDirection otherData;

    bool h_free = false, l_free, r_free, u_free = false;
    bool found_platform = false;
    bool other_h_space = false, other_u_space = false, other_l_space = false, other_r_space = false;
    bool other_grounded = false;
    bool obscured = false;
    m_obscured = -1;
    if ((!grounded || !l_space || !r_space || !u_space) && m_axis == m_anchor_axis && m_anchor_end != -1 && tpos.m_a[m_anchor_axis] >= m_anchor_end && tpos.m_a[m_anchor_axis] <= m_anchor_begin)
    {
      Vector3 snap_position(0, 0, 0);
      Vector3 ground_position(0, 0, 0);
      Vector3 u_space_position(0, 0, 0);
      Vector3 h_space_position(0, 0, 0);
      Vector3 l_space_position(0, 0, 0);
      Vector3 r_space_position(0, 0, 0);

      int difference = m_anchor_begin - tpos.m_a[m_anchor_axis];
      for (int i = 0; i <= m_anchor_begin - m_anchor_end; ++i)
      {
        h_free = false;
        l_free = false;
        r_free = false;
        u_free = false;
        found_platform = false;
        if(m_drop)
          found_platform = !found_platform;
        otherData.UP = 0;
        otherData.LEFT = 0;
        otherData.RIGHT = 0;
        otherData.DOWN = 0;
        Vector3 anchor_offset(0, 0, 0);
        anchor_offset.m_a[m_anchor_axis] = static_cast<float>(difference - i);

        // If this layer is ommited, we need to continue
        if(std::find(m_ommited_layers.begin(), m_ommited_layers.end(), static_cast<int>((pos+anchor_offset).m_a[m_anchor_axis])) != m_ommited_layers.end())
          continue;
        // If this tile is collision, we have to stop looking
        if (GetCollisionTileAtOffset(tilemap, anchor_offset) > 0)
        {
          // If tile obscures player, don't pull player forward
          if(anchor_offset.m_a[m_anchor_axis] > 0)
          {
            other_grounded = false;
            other_u_space = false;
            other_l_space = false;
            other_r_space = false;
            m_obscured = int(pos.m_a[m_anchor_axis] + anchor_offset.m_a[m_anchor_axis]);
          }
          // If obstruction is the behind the player, break out
          else if(anchor_offset.m_a[m_anchor_axis] < 0)
            break;

          // Skip to next layer
          continue;
        }

        //Get current collision stats
        CheckCollisionInDirection(otherData, pos4 + anchor_offset, tilemap);

        // Only check for ground collision if falling
        Vector3 velocity = GetParent()->GetComponent<DynamicComponent>()->GetVelocity();
        if(!obscured)
        {
          if((m_apex >= std::floor(pos4.m_y)+.45f && velocity.m_y < 0) || m_drop)
          {
            //found_platform = CheckGrounded(tilemap, anchor_offset);
            if(otherData.DOWN)
            {
              ground_clear = false;
              found_platform = true;
              if(otherData.DOWN == 1)
              {
                if (otherData.LEFT || otherData.RIGHT)
                  found_platform = false;
              }
              if(m_drop)
                 found_platform = !found_platform;
            }
          }

          // Check if layer fails the grounded test
          if((other_grounded || grounded) && !found_platform)
            continue;

          if(!otherData.LEFT)
          {
            l_free = true;
            if(!other_l_space || (found_platform && !other_grounded))
            {
              other_l_space = true;
              l_space_position = pos + anchor_offset;
            }
          }
          if(!otherData.RIGHT)
          {
            r_free = true;
            if(!other_r_space || (found_platform && !other_grounded))
            {
              other_r_space = true;
              r_space_position = pos + anchor_offset;
            }
          }
          if(!otherData.UP)
          {
            u_free = true;
            if (!other_u_space)
            {
              other_u_space = true;
              u_space_position = pos + anchor_offset;
            }
          }
          if(!other_grounded && found_platform && ((otherData.DOWN && otherData.UP != 1) || m_drop))
          {
            other_grounded = true;
            ground_position = pos + anchor_offset;
          }

        }

        // Check if layer fails the open space test
        if(((u_space || other_u_space) && !u_free) || ((l_space || other_l_space) && !l_free) || ((r_space || other_r_space) && !r_free))
          continue;

        // Layer is not worse than others, so compare piecewise
        else
        {
          // if layer satisfies both open air condition, mark it as best choice
          if((u_space_position.m_a[m_anchor_axis] != r_space_position.m_a[m_anchor_axis]) && (u_free && r_free))
          {
            u_space_position = pos + anchor_offset;
            r_space_position = u_space_position;
            other_u_space = true;
            other_r_space = true;
          }
          if((u_space_position.m_a[m_anchor_axis] != l_space_position.m_a[m_anchor_axis]) && (u_free && l_free))
          {
            u_space_position = pos + anchor_offset;
            l_space_position = u_space_position;
            other_u_space = true;
            other_l_space = true;
          }
          if((r_space_position.m_a[m_anchor_axis] != l_space_position.m_a[m_anchor_axis]) && (r_free && l_free))
          {
            r_space_position = pos + anchor_offset;
            l_space_position = r_space_position;
            other_r_space = true;
            other_l_space = true;
          }
        }
      }
      if(m_drop && !other_grounded)
        m_drop = false;
      if(other_grounded || other_l_space || other_r_space || other_u_space)
      {
        bool ground_better = true;
        bool vertical_better = true;
        bool left_better = true;
        bool right_better = true;
        bool better = false;
        bool defacto_better = false;

        if(!grounded && other_grounded)
        {
          better = true;
          snap_position = ground_position;
          defacto_better = true;
        }
        else if(grounded && !other_grounded)
          ground_better = false;

        if(!u_space && other_u_space)
        {
          snap_position = u_space_position;
          better = true;

          if(ground_better)
            defacto_better = true;
        }
        else if(u_space && !other_u_space)
          vertical_better = false;

        if(!l_space && other_l_space)
        {
          snap_position = l_space_position;
          better = true;
          if(ground_better && vertical_better)
            defacto_better = true;
        }
        else if(l_space && !other_l_space)
          left_better = false;

        if(!r_space && other_r_space)
        {
          snap_position = r_space_position;
          better = true;
          if(ground_better && vertical_better && left_better)
            defacto_better = true;
        }
        else if(r_space && !other_r_space)
          right_better = false;

        if(defacto_better || (better && ground_better && vertical_better && left_better && right_better))
        {
          grounded = other_grounded;
          GetParent()->SetPosition(snap_position);
        }
      }
    }
    else if(m_axis == m_anchor_axis && m_anchor_end != -1 && tpos.m_a[m_anchor_axis] >= m_anchor_end && tpos.m_a[m_anchor_axis] <= m_anchor_begin)
    {
      int difference = m_anchor_begin - tpos.m_a[m_anchor_axis];
      for (int i = 0; i <= m_anchor_begin - m_anchor_end; ++i)
      {
        Vector3 anchor_offset(0, 0, 0);
        anchor_offset.m_a[m_anchor_axis] = static_cast<float>(difference - i);

        // If this tile is collision, we have to stop looking
        if (GetCollisionTileAtOffset(tilemap, anchor_offset) > 0)
        {
          if(anchor_offset.m_a[m_anchor_axis] > 0)
          {
            m_obscured = int(pos.m_a[m_anchor_axis] + anchor_offset.m_a[m_anchor_axis]);
          }
        }
      }
    }

    if(m_drop)
      grounded = !grounded;
    if(m_drop && pos.m_y < m_drop_y-.6f)
      m_drop = false;

    if (grounded && !m_is_grounded)
    {
      GetParent()->Invoke("OnGrounded");
      m_grounded_tile_change = true;
    }
    else if(!grounded && m_is_grounded)
    {
      GetParent()->Invoke("OnNotGrounded");
      m_grounded_tile_change = true;
    }

    m_is_grounded = grounded;

    //Update Tile Systems Obscured flag
    g_engine_core->SystemCall("TileSystem", "SetObscured", m_obscured);
  }
  // If axis is top down, constantly resolve ground collision
  else
  {
    bool grounded = false;
    int size = m_hs_ver[2].size();
    for (int i = 2; i < size; i += 4)
    {
      if (GetCollisionTileAtOffset(tilemap, m_hs_ver[2][i] + offset) > 0)
        grounded = true;
      if (GetCollisionTileAtOffset(tilemap, m_hs_ver[2][i + 1] + offset) > 0)
        grounded = true;
    }

    if (grounded)
    {
      ResolveDepthCollision(m_hs_ver[2][2] + offset);
      SnapDepth();
      if (!m_is_grounded)
      {
        GetParent()->Invoke("OnGrounded");
        m_grounded_tile_change = true;
      }
    }
    else if(!grounded && m_is_grounded)
    {
      GetParent()->Invoke("OnNotGrounded");
      m_grounded_tile_change = true;
    }
    m_is_grounded = grounded;
  }

  int size = m_hs_ver[m_axis].size();
  for (int i = 0; i < size; ++i)
  {
    // Get offset for this hotspot and object position
    Vector3 offset = m_hs_ver[m_axis][i];
    pos4 = GetParent()->GetPosition();

    // Get tiles to the up and down of this one
    TilePos2 tilepos = Tilemap::GetTilePos(pos4 + offset, m_axis);
    TilePos2 tilepos_u = tilepos;
    TilePos2 tilepos_d = tilepos;
    tilepos_u.m_y += 1; tilepos_d.m_y -= 1;
    char tile_u = tilemap->GetTile(tilepos_u, m_axis, m_layer);
    char tile_d = tilemap->GetTile(tilepos_d, m_axis, m_layer);

    // If the tile at the hotspot is collision
    // We can't be colliding with it if the tiles to the up and down are
    // collision as well
    if (tilemap->GetTile(pos4 + offset) > 0 && (tile_u == 0 || tile_d == 0))
    {
      // Resolve the collision with impulses
      ResolveVerticalCollision(offset);

      // Resolve the penetration by snapping along the y axis
      SnapVertical(m_axis == 1 ? (offset.m_z > 0 ? 1 : -1) : (offset.m_y > 0 ? 1 : -1));
    }
  }
  size = m_hs_hor[m_axis].size();
  for (int i = 0; i < size; ++i)
  {
    // Get offset for this hotspot and object position
    Vector3 offset = m_hs_hor[m_axis][i];
    pos4 = GetParent()->GetPosition();

    // Get tiles to the left and right of this one
    TilePos2 tilepos = Tilemap::GetTilePos(pos4 + offset, m_axis);
    TilePos2 tilepos_l = tilepos;
    TilePos2 tilepos_r = tilepos;
    tilepos_l.m_x -= 1; tilepos_r.m_x += 1;
    char tile_l = tilemap->GetTile(tilepos_l, m_axis, m_layer);
    char tile_r = tilemap->GetTile(tilepos_r, m_axis, m_layer);

    // If the tile at the hotspot is collision
    // We can't be colliding with it if the tiles to the left and right are
    // collision as well
    if (tilemap->GetTile(pos4 + offset) > 0 && (tile_l == 0 || tile_r == 0))
    {
      // Resolve the collision with impulses
      ResolveHorizontalCollision(offset);

      // Resolve the penetration by snapping along the x axis
      SnapHorizontal(m_axis ? (offset.m_x > 0 ? 1 : -1) : (offset.m_z > 0 ? 1 : -1));
    }
  }
  
  // Check for special tile occupation
  Tilemap *special_tilemap = g_engine_core->SystemCall("TileSystem", "GetTilemaps", "Special");
  Tilemap *graphics_tilemap = g_engine_core->SystemCall("TileSystem", "GetTilemaps", "Graphics");
  TilePos3 focus_pos = g_engine_core->SystemCall("TileSystem", "GetFocusPosition");
  // Update tile Pos
    // Update Positions
  if(m_tile_pos != focus_pos)
  {
    //m_grounded_pos = focus_pos;
    m_last_tile_pos = m_tile_pos;
    m_tile_pos = focus_pos;
    m_tile_change = true;
  }
  else
    m_tile_change = false;

  if(m_last_safe_pos.m_y - focus_pos.m_y > m_fall_max*4)
  {
    //GetParent()->GetComponent<SpriteComponent>()->SetAnimation(m_facing == LEFT || m_facing == DOWN ? 9 : 8);
    m_dead = true;
    m_deadCount = 0.f;
    g_engine_core->DispatchEvent("OnPlayerDeathToggle", true);
    g_engine_core->DispatchEvent("PushAudioEvent", "Play_death");

    return;
  }
  if(m_grounded_tile_change || m_grounded_pos != focus_pos && m_is_grounded)
  {
    if(m_last_safe_pos.m_y - focus_pos.m_y > m_fall_max)
    {
      GetParent()->GetComponent<SpriteComponent>()->SetAnimation(m_facing == LEFT || m_facing == DOWN ? 9 : 8);
      m_dead = true;
      m_deadCount = 0.f;
      g_engine_core->DispatchEvent("OnPlayerDeathToggle", true);
      g_engine_core->DispatchEvent("PushAudioEvent", "Play_death");

      return;
      /*GetParent()->SetPosition(Vector4(m_last_safe_pos.m_x + .5f, m_last_safe_pos.m_y + .5f,m_last_safe_pos.m_z + .5f, 0));
      m_grounded_pos = m_last_safe_pos;
      focus_pos = m_grounded_pos;*/
    }
    else
    {
      m_last_grounded_pos = m_grounded_pos;
      if(m_is_grounded && !m_drop)
      {
        Tilemap *collision_tilemap = g_engine_core->SystemCall("TileSystem", "GetTilemaps", "Collision");
        TilePos3 groundBelow = focus_pos - TilePos3(0,1,0);
        if(collision_tilemap->GetTile(groundBelow))
        {
          m_grounded_pos = focus_pos;
          m_last_safe_pos = m_grounded_pos;
          m_grounded_tile_change = true;
        }
        else
        {
          m_grounded_tile_change = false;
          int tileMod;
          float horizontal;
          if(m_axis)
            horizontal = GetParent()->GetPosition().m_x;
          else
            horizontal = GetParent()->GetPosition().m_z; 

          tileMod = float(int(horizontal + .5f)) < horizontal ? -1 : 1;
          if(m_axis)
            m_last_safe_pos = m_tile_pos + TilePos3(tileMod, 0, 0);
          else
            m_last_safe_pos = m_tile_pos + TilePos3(0, 0, tileMod);

          //m_last_safe_pos = m_grounded_pos;
        }
      }
    }
  }

  switch(m_axis)
  {
  case 0:
    //Front and Top
    UpdateMemory(0);
    UpdateMemory(1);
    UpdateMemory(2);
    break;
  case 1:
    //Front and Side
    UpdateMemory(0);
    UpdateMemory(1);
    UpdateMemory(2);
    break;
  case 2:
    //Side and Top
    UpdateMemory(1);
    UpdateMemory(0);
    UpdateMemory(2);
    break;
  }

  Tile curr_type = special_tilemap->GetTile(m_grounded_pos);
  Tile last_type = special_tilemap->GetTile(m_last_grounded_pos);
  TileGameplayComponent* specialTile;
  if(last_type)
     specialTile = special_tilemap->GetTileObject(m_last_grounded_pos.m_x, m_last_grounded_pos.m_y, m_last_grounded_pos.m_z);
  //int level;
  int prev;

  //////////////////////////
  // grounded tile exit (triggers if not touching ground within same tile)
  switch(last_type)
  {
  case 0:
    break;
  case 1:
    break;
  case 2:
    if(((m_grounded_tile_change)) && !g_rotate_ability)
    {
      HUDRotatePrompt(false);
      EnableRotateTo(special_tilemap->GetTileObject(m_last_grounded_pos.m_x, m_last_grounded_pos.m_y, m_last_grounded_pos.m_z)->GetDefaultAxis(), false);
      EnableRotateTo(special_tilemap->GetTileObject(m_last_grounded_pos.m_x, m_last_grounded_pos.m_y, m_last_grounded_pos.m_z)->GetPreviousAxis(), false);

      //Set Animation to correct frame
      if(g_rotate_blocks && !g_rotate_ability)
      {
        prev = specialTile->GetPreviousAxis() == -1 ? specialTile->GetDefaultAxis() : specialTile->GetPreviousAxis();
        specialTile->GetSibling<SpriteComponent>()->SetFrame(((specialTile->GetDefaultAxis() - prev + 3) % 3) * 2);
      }
      else
        specialTile->GetSibling<SpriteComponent>()->SetFrame(6);
    }
    break;
  case 3:
    if(((m_grounded_tile_change)) && !g_rotate_ability)
    {
      HUDRotatePrompt(false);
      EnableRotateTo(special_tilemap->GetTileObject(m_last_grounded_pos.m_x, m_last_grounded_pos.m_y, m_last_grounded_pos.m_z)->GetDefaultAxis(), false);
      EnableRotateTo(special_tilemap->GetTileObject(m_last_grounded_pos.m_x, m_last_grounded_pos.m_y, m_last_grounded_pos.m_z)->GetPreviousAxis(), false);

      //Set Animation to correct frame
      if(g_rotate_blocks && !g_rotate_ability)
      {
        prev = specialTile->GetPreviousAxis() == -1 ? specialTile->GetDefaultAxis() : specialTile->GetPreviousAxis();
        specialTile->GetSibling<SpriteComponent>()->SetFrame(((specialTile->GetDefaultAxis() - prev + 3) % 3) * 2);
      }
      else
        specialTile->GetSibling<SpriteComponent>()->SetFrame(6);
    }
    break;
  case 4:
    if(((m_grounded_tile_change)) && !g_rotate_ability)
    {
      HUDRotatePrompt(false);
      EnableRotateTo(special_tilemap->GetTileObject(m_last_grounded_pos.m_x, m_last_grounded_pos.m_y, m_last_grounded_pos.m_z)->GetDefaultAxis(), false);
      EnableRotateTo(special_tilemap->GetTileObject(m_last_grounded_pos.m_x, m_last_grounded_pos.m_y, m_last_grounded_pos.m_z)->GetPreviousAxis(), false);

      //Set Animation to correct frame
      if(g_rotate_blocks && !g_rotate_ability)
      {
        prev = specialTile->GetPreviousAxis() == -1 ? specialTile->GetDefaultAxis() : specialTile->GetPreviousAxis();
        specialTile->GetSibling<SpriteComponent>()->SetFrame(((specialTile->GetDefaultAxis() - prev + 3) % 3) * 2);
      }
      else
        specialTile->GetSibling<SpriteComponent>()->SetFrame(6);
    }
    break;
  //case 5:
  //  if(m_tile_change && !g_anchor_ability)
  //    m_anchor_enabled = false;
  //  break;
    //Enable Rotation Blocks Block
  case 50:
    if(g_rotate_blocks && specialTile->GetSibling<SpriteComponent>()->GetCurrentAnimation() != 3)
    {
      if(!g_unlocked_rblocks)
      {
        g_unlocked_rblocks = true;
        HUDNarrativePrompt(UNLOCK_ROTATION_BLOCKS);
      }
      g_engine_core->DispatchEvent("PushAudioEvent", "Resume_Audio");

      g_engine_core->DispatchEvent("OnToggleCoinVisible", true);
      g_engine_core->DispatchEvent("OnCreateCoins", 5);
      g_engine_core->DispatchEvent("OnUpdateCoinNumber", g_curr_coin_count);
      specialTile->GetSibling<SpriteComponent>()->SetAnimation(3);
    }
    m_levitate = false;
    break;
    //Enable Rotation Ability Block
  case 51:
    if(g_rotate_ability)
    {
      if(!g_unlocked_rability)
      {
        g_unlocked_rability = true;
        HUDNarrativePrompt(UNLOCK_FREE_ROTATION);
      }
      g_engine_core->DispatchEvent("PushAudioEvent", "Resume_Audio");

      g_engine_core->DispatchEvent("OnToggleCoinVisible", true);
      g_coins_needed = 10;
      g_engine_core->DispatchEvent("OnCreateCoins", 10);
      g_engine_core->DispatchEvent("OnUpdateCoinNumber", g_curr_coin_count);
      specialTile->GetSibling<SpriteComponent>()->SetAnimation(3);      
    }
    m_levitate = false;
    break;
    //Enable Anchor Blocks Block
  case 52:
    if(g_anchor_blocks)
    {
      if(!g_unlocked_ablocks)
      {
        g_unlocked_ablocks = true;
        HUDNarrativePrompt(UNLOCK_ANCHOR_BLOCKS);
      }
      g_engine_core->DispatchEvent("PushAudioEvent", "Resume_Audio");

      g_engine_core->DispatchEvent("OnToggleCoinVisible", true);
      g_engine_core->DispatchEvent("OnCreateCoins", 10);
      g_engine_core->DispatchEvent("OnUpdateCoinNumber", g_curr_coin_count);
      specialTile->GetSibling<SpriteComponent>()->SetAnimation(3);
    }
    m_levitate = false;
    break;
    //Enable Anchor Ability Block
  case 53:
    if(g_anchor_ability)
    {
      if(!g_unlocked_aability)
      {
        g_unlocked_aability = true;
        HUDNarrativePrompt(UNLOCK_FREE_ANCHORING);
      }
      g_engine_core->DispatchEvent("PushAudioEvent", "Resume_Audio");

      g_engine_core->DispatchEvent("OnToggleCoinVisible", true);
      g_engine_core->DispatchEvent("OnCreateCoins", 10);
      g_engine_core->DispatchEvent("OnUpdateCoinNumber", g_curr_coin_count);
      specialTile->GetSibling<SpriteComponent>()->SetAnimation(3);
    }
    m_levitate = false;
    break;
    //Enable The End Block
  case 54:
    if(g_rotate_ability)
    {
      g_engine_core->DispatchEvent("PushAudioEvent", "Resume_Audio");

      g_engine_core->DispatchEvent("OnToggleCoinVisible", true);
      g_engine_core->DispatchEvent("OnCreateCoins", 5);
      g_engine_core->DispatchEvent("OnUpdateCoinNumber", g_curr_coin_count);
      specialTile->GetSibling<SpriteComponent>()->SetAnimation(3);
    }
    m_levitate = false;
    break;
  }
  //////////////////////////
  // Grounded Tile Occupation
  bool halfwaypoint = false;
  float horizpos;
  switch(m_axis)
  {
  case 0:
    horizpos = GetParent()->GetPosition().m_z;
    if(abs(horizpos - ((float(m_tile_pos.m_z) + .5f))) < .075f)
      halfwaypoint = true;
    break;
  case 2:
    horizpos = GetParent()->GetPosition().m_x;
    if(abs(horizpos - ((float(m_tile_pos.m_x) + .5f))) < .075f)
      halfwaypoint = true;
    break;
  }
  if(curr_type)
    specialTile = special_tilemap->GetTileObject(m_grounded_pos.m_x, m_grounded_pos.m_y, m_grounded_pos.m_z);
  switch(curr_type)
  {
  case 0:
    break;

    case 1:
      if(m_grounded_tile_change && m_grounded_pos != m_last_grounded_pos)
      {
        SpaceWrapup();
      }
      break;
    case 20:
      if(m_grounded_tile_change && m_grounded_pos != m_last_grounded_pos)
      {
        m_warping = true;
        g_engine_core->DispatchEvent("PushAudioEvent", "Stop_Audio");
        g_engine_core->DispatchEvent("PushAudioEvent", "Play_level_change");
        g_engine_core->DispatchEvent("OnPlayerDeathToggle", true);
        m_next_level = 20;
      }
      break;
    case 21:
      if(m_grounded_tile_change && m_grounded_pos != m_last_grounded_pos)
      {
        m_warping = true;
        g_engine_core->DispatchEvent("PushAudioEvent", "Stop_Audio");
        g_engine_core->DispatchEvent("PushAudioEvent", "Play_level_change");
        g_engine_core->DispatchEvent("OnPlayerDeathToggle", true);
        m_next_level = 21;
      }
      break;
    case 22:
      if(m_grounded_tile_change && m_grounded_pos != m_last_grounded_pos)
      {
        m_warping = true;
        g_engine_core->DispatchEvent("PushAudioEvent", "Stop_Audio");
        g_engine_core->DispatchEvent("PushAudioEvent", "Play_level_change");
        g_engine_core->DispatchEvent("OnPlayerDeathToggle", true);
        m_next_level = 22;
      }
      break;
    case 23:
      if(m_grounded_tile_change && m_grounded_pos != m_last_grounded_pos)
      {
        m_warping = true;
        g_engine_core->DispatchEvent("PushAudioEvent", "Stop_Audio");
        //g_engine_core->DispatchEvent("PushAudioEvent", "Play_level_change");
        g_engine_core->DispatchEvent("OnPlayerDeathToggle", true);
        HUDNarrativePrompt(END_GAME, 0.f, true);
        m_next_level = 23;
      }
      break;
    case 24:
      if(m_grounded_tile_change && m_grounded_pos != m_last_grounded_pos)
      {
        m_warping = true;
        g_engine_core->DispatchEvent("PushAudioEvent", "Stop_Audio");
        g_engine_core->DispatchEvent("PushAudioEvent", "Play_level_change");
        g_engine_core->DispatchEvent("OnPlayerDeathToggle", true);
        m_next_level = 24;
      }
      break;
    case 25:
      if(m_grounded_tile_change && m_grounded_pos != m_last_grounded_pos)
      {
        m_warping = true;
        g_engine_core->DispatchEvent("PushAudioEvent", "Stop_Audio");
        g_engine_core->DispatchEvent("PushAudioEvent", "Play_level_change");
        g_engine_core->DispatchEvent("OnPlayerDeathToggle", true);
        m_next_level = 25;
      }
      break;
  case 2:
    if(m_grounded_tile_change && m_is_grounded)
    {
      HUDRotatePrompt(true);
      if(m_axis != specialTile->GetDefaultAxis())
        EnableRotateTo(specialTile->GetDefaultAxis(), true);
      else if(m_axis == specialTile->GetDefaultAxis())
        EnableRotateTo(specialTile->GetPreviousAxis(), true);
      //RotateTo(0);

      //Set Animation to correct frame
      if(g_rotate_blocks && !g_rotate_ability)
      {
        prev = specialTile->GetPreviousAxis() == -1 ? specialTile->GetDefaultAxis() : specialTile->GetPreviousAxis();
        specialTile->GetSibling<SpriteComponent>()->SetFrame(((specialTile->GetDefaultAxis() - prev + 3) % 3) * 2 + 1);
      }
      else
        specialTile->GetSibling<SpriteComponent>()->SetFrame(6);
    }
    break;
  case 3:
    if(m_grounded_tile_change && m_is_grounded)
    {
      HUDRotatePrompt(true);
      if(m_axis != specialTile->GetDefaultAxis())
        EnableRotateTo(specialTile->GetDefaultAxis(), true);
      else if(m_axis == specialTile->GetDefaultAxis())
        EnableRotateTo(specialTile->GetPreviousAxis(), true);

      //Set Animation to correct frame
      if(g_rotate_blocks && !g_rotate_ability)
      {
        prev = specialTile->GetPreviousAxis() == -1 ? specialTile->GetDefaultAxis() : specialTile->GetPreviousAxis();
        specialTile->GetSibling<SpriteComponent>()->SetFrame(((specialTile->GetDefaultAxis() - prev + 3) % 3) * 2 + 1);
      }
      else
        specialTile->GetSibling<SpriteComponent>()->SetFrame(6);
    }
    //RotateTo(1);

    break;
  case 4:
    if(m_grounded_tile_change && m_is_grounded)
    {
      HUDRotatePrompt(true);
      if(m_axis != specialTile->GetDefaultAxis())
        EnableRotateTo(specialTile->GetDefaultAxis(), true);
      else if(m_axis == specialTile->GetDefaultAxis())
        EnableRotateTo(specialTile->GetPreviousAxis(), true);

      //Set Animation to correct frame
      if(g_rotate_blocks && !g_rotate_ability)
      {
        prev = specialTile->GetPreviousAxis() == -1 ? specialTile->GetDefaultAxis() : specialTile->GetPreviousAxis();
        specialTile->GetSibling<SpriteComponent>()->SetFrame(((specialTile->GetDefaultAxis() - prev + 3) % 3) * 2 + 1);
      }
      else
        specialTile->GetSibling<SpriteComponent>()->SetFrame(6);
    }
    //RotateTo(2);
    break;
  // Front Anchor blocks
  case 5:
    if(m_grounded_tile_change && m_axis == 0 && (g_anchor_blocks && !g_anchor_ability) && m_grounded_pos != m_last_grounded_pos)
    {
      int anchor_layer = m_tile_pos.m_z;
      if(m_axis != specialTile->GetAnchorAxis())
      {
        if(anchor_layer != m_anchor_begin && m_anchor_end == -1)
          Anchor();
        else if(m_anchor_begin != -1 && m_anchor_end == -1)
          Anchor();
        if(m_anchor_begin > -1)
        {
          if(m_anchor_end > -1)
            special_tilemap->ToggleAnchorTilesActive(true, m_anchor_axis);
          else
            specialTile->GetSibling<SpriteComponent>()->SetFrame(1);
        }
        //else
        //  special_tilemap->ToggleAnchorTilesActive(false);
      }
    }
    break;
  // Side Anchor block
  case 6:
    if(m_grounded_tile_change && m_axis == 2 && (g_anchor_blocks && !g_anchor_ability) && m_grounded_pos != m_last_grounded_pos)
    {
      int anchor_layer = m_tile_pos.m_x;
      if(m_axis != specialTile->GetAnchorAxis())
      {
        if(anchor_layer != m_anchor_begin && m_anchor_end == -1)
          Anchor();
        else if(m_anchor_begin != -1 && m_anchor_end == -1)
          Anchor();
        if(m_anchor_begin > -1)
        {
          if(m_anchor_end > -1)
            special_tilemap->ToggleAnchorTilesActive(true, m_anchor_axis);
          else
            specialTile->GetSibling<SpriteComponent>()->SetFrame(1);
        }
    /*    else
          special_tilemap->ToggleAnchorTilesActive(false);*/
      }
    }
    break;
    ////////////////////////////////////////////
    ////////////////////////////////////////////
    ////////////////////////////////////////////
    // Tutorial tip tiles
    // Free Rotate
  case 32:
    if(!g_free_rotated && g_rotate_ability)
    {
      g_free_rotated = true;
      HUDNarrativePrompt(ROTATING);
    }
    break;
    //Anchor Drop
  case 30:
    if(!g_anchor_dropped && m_axis == m_anchor_axis && m_anchor_end > -1)
    {
      //g_anchor_dropped = true;
      HUDNarrativePrompt(ANCHOR_DROPPING);
    }
    break;
    //Post anchor drop
  case 31:
    if(!g_anchor_dropped)
    {
      HUDNarrativePrompt(POST_ANCHOR_DROPPING);
    }
    break;
    //Walk behind prompt
  case 37:
    if(!g_walked_behind)
    {
      HUDNarrativePrompt(WALK_BEHIND);
    }
    break;
    //Player obscured
  case 38:
    if(!g_walked_behind)
    {
      g_walked_behind = true;
    }
    break;
    //Clear Anchoring
  case 34:
    if(!g_anchor_cleared)
    {
      g_anchor_cleared = true;
      HUDNarrativePrompt(ANCHOR_CLEARING);
    }
    break;
    //Set Anchoring
  case 33:
    if(g_anchor_ability)
    {  
      if(m_grounded_tile_change && m_last_grounded_pos != m_grounded_pos)
      {
        if(!g_anchor_tried && m_anchor_begin == -1)
        {
          HUDNarrativePrompt(ANCHOR_SETTING);
        }
        else if(!g_anchor_set && m_anchor_begin > -1)
        {
          if(m_grounded_pos.m_x != m_anchor_begin)
            HUDNarrativePrompt(ANCHOR_COMMITTING);
        }
      }
    }
  
    break;
    //Warp to Hub
  case 36:
    if(!g_warped_back)
    {
      g_warped_back = true;
      HUDNarrativePrompt(HUB_RETURN);
    }
    break;
    ////////////////////////////////////////////
    ////////////////////////////////////////////
    ////////////////////////////////////////////
    // "Bank" Switch tiles

    // "Enable Rotation Tiles
  case 50:
    if(g_curr_coin_count >= 5 && !g_rotate_blocks && halfwaypoint)
    {
      ChangeMemoryLimit(m_memorySize);
      m_silent_anchor = true;
      g_engine_core->DispatchEvent("OnAnchorEvent", 2);
      m_silent_anchor = false;
      g_engine_core->DispatchEvent("PushAudioEvent", "Pause_Audio");
      g_engine_core->DispatchEvent("PushAudioEvent", "Play_level_up");

      g_engine_core->DispatchEvent("OnToggleCoinVisible", false);
      m_levitate = true;
      specialTile->GetSibling<SpriteComponent>()->SetAnimation(2);
      special_tilemap->ToggleRotationTiles(true);

      g_rotate_blocks = true;
      g_curr_coin_count = 0;

      m_leveling = true;
      g_engine_core->DispatchEvent("OnPlayerDeathToggle", true);
      //ChangeMemoryLimit(5);
      return;
    }
    break;
    // "Enable Free Rotation
  case 51:
    if(g_curr_coin_count >= 5 && !g_rotate_ability && halfwaypoint)
    {
      //Hub Music Change
      g_engine_core->DispatchEvent("PushAudioEvent", "Stop_ambient");
      //g_engine_core->DispatchEvent("PushAudioEvent", "Play_hub_music");
      g_engine_core->DispatchEvent("PushAudioEvent", "Play_level_music");
      g_engine_core->DispatchEvent("PushAudioEvent", "Pause_Audio");
      g_engine_core->DispatchEvent("PushAudioEvent", "Play_level_up");

      g_engine_core->DispatchEvent("OnToggleCoinVisible", false);
      m_levitate = true;
      g_rotate_ability = true;

      g_engine_core->DispatchEvent("OnSideToggle", g_rotate_ability);
      g_engine_core->DispatchEvent("OnTopToggle", g_rotate_ability);
      g_engine_core->DispatchEvent("OnFrontToggle", g_rotate_ability);

      special_tilemap->ToggleRotationTiles(false);
      g_engine_core->DispatchEvent("HideHudItem", "RotationPrompt");
      specialTile->GetSibling<SpriteComponent>()->SetAnimation(2);
      g_curr_coin_count = 0;

      m_leveling = true;
      g_engine_core->DispatchEvent("OnPlayerDeathToggle", true);

      // Increase memory size
      ChangeMemoryLimit(m_currMemorySize + 1);
      m_silent_anchor = true;
      g_engine_core->DispatchEvent("OnAnchorEvent", 2);
      m_silent_anchor = false;
      return;
    }
    break;
    //Enable Anchor Blocks Block
  case 52:
    if(g_curr_coin_count >= 10 && !g_anchor_blocks && halfwaypoint)
    {
      int coins = g_curr_coin_count;
      g_engine_core->DispatchEvent("PushAudioEvent", "Pause_Audio");
      g_engine_core->DispatchEvent("PushAudioEvent", "Play_level_up");

      g_engine_core->DispatchEvent("OnToggleCoinVisible", false);
      g_anchor_blocks = true;
      special_tilemap->ToggleAnchorTiles(true);
      specialTile->GetSibling<SpriteComponent>()->SetAnimation(2);
      m_levitate = true;
      g_curr_coin_count = 0;

      m_leveling = true;
      g_engine_core->DispatchEvent("OnPlayerDeathToggle", true);

      // Increase memory size
      ChangeMemoryLimit(m_currMemorySize + 1);
      m_silent_anchor = true;
      g_engine_core->DispatchEvent("OnAnchorEvent", 2);
      m_silent_anchor = false;
      return;
    }
    break;
    //Enable Anchor Ability Block
  case 53:
    if(g_curr_coin_count >= 10 && !g_anchor_ability && halfwaypoint)
    {
      g_engine_core->DispatchEvent("PushAudioEvent", "Pause_Audio");
      g_engine_core->DispatchEvent("PushAudioEvent", "Play_level_up");

      g_engine_core->DispatchEvent("OnToggleCoinVisible", false);
      m_anchor_commit = false;
      m_anchor_axis = -1;
      m_anchor_begin = -1;
      m_anchor_end = -1;

      g_engine_core->SystemCall("TileSystem", "SetAnchorAxis", m_anchor_axis);
      g_engine_core->SystemCall("TileSystem", "SetAnchorBegin", m_anchor_begin);
      g_engine_core->SystemCall("TileSystem", "SetAnchorEnd", m_anchor_end);
      m_silent_anchor = true;
      g_engine_core->DispatchEvent("OnAnchorEvent", 2);
      m_silent_anchor = false;


      g_anchor_ability = true;
      special_tilemap->ToggleAnchorTiles(false);
      specialTile->GetSibling<SpriteComponent>()->SetAnimation(2);
      m_levitate = true;
      g_curr_coin_count = 0;

      m_leveling = true;
      g_engine_core->DispatchEvent("OnPlayerDeathToggle", true);

      // Increase memory size
      ChangeMemoryLimit(m_currMemorySize + 1);
      m_silent_anchor = true;
      g_engine_core->DispatchEvent("OnAnchorEvent", 2);
      m_silent_anchor = false;
      return;
    }
    break;
    //Enable The End Block
  case 54:
    if(g_curr_coin_count >= 5 && !g_the_end && halfwaypoint)
    {
      g_engine_core->DispatchEvent("PushAudioEvent", "Pause_Audio");
      g_engine_core->DispatchEvent("PushAudioEvent", "Play_level_up");

      g_engine_core->DispatchEvent("OnToggleCoinVisible", false);
      g_the_end = true;
      specialTile->GetSibling<SpriteComponent>()->SetAnimation(2);
      m_levitate = true;
      g_curr_coin_count = 0;

      m_leveling = true;
      g_engine_core->DispatchEvent("OnPlayerDeathToggle", true);

      // Increase memory size
      ChangeMemoryLimit(m_currMemorySize + 1);
      m_silent_anchor = true;
      g_engine_core->DispatchEvent("OnAnchorEvent", 2);
      m_silent_anchor = false;
      return;
    }
    break;
  }

  //////////////////////////
  // Tile occupation
  curr_type = special_tilemap->GetTile(m_tile_pos);
  last_type = special_tilemap->GetTile(m_last_tile_pos);
  if(curr_type)
    specialTile = special_tilemap->GetTileObject(m_tile_pos.m_x, m_tile_pos.m_y, m_tile_pos.m_z);

  switch(curr_type)
  {
    // Collectible
  case 7:
    UnlockTiles(special_tilemap, m_tile_pos);
    break;
    // Artifact
  case 8:
    if(specialTile->GetSibling<SpriteComponent>()->GetCurrentFrame() != 1)
    {
      special_tilemap->SetTile(Vector4(float(m_tile_pos.m_x), float(m_tile_pos.m_y), float(m_tile_pos.m_z), 0.0f), 0);
      specialTile->GetSibling<SpriteComponent>()->SetFrame(1);
      g_engine_core->DispatchEvent("PushAudioEvent", "Play_artifact_get");
    }
    break;
  }

  m_grounded_tile_change = false;
  m_tile_change = false;
  AnimateTiles(dt);
  AnimatePlayer(dt);
}

void TileComponent::SnapHorizontal(int centerShift)
{
  Vector4 pos = GetParent()->GetPosition();

  // Figure out which axis to snap to
  if (m_axis == 1 || m_axis == 2)
  {
    pos.m_x = std::floor(pos.m_x + m_offset.m_x) + (.5f + centerShift*(.5f - .5f*m_fwidth) - m_offset.m_x);
    GetSibling<DynamicComponent>()->KillHorizontalVelocity(0);
  }
  else if (m_axis == 0)
  {
    pos.m_z = std::floor(pos.m_z + m_offset.m_z) + (.5f + centerShift*(.5f - .5f*m_fdepth) - m_offset.m_z);
    GetSibling<DynamicComponent>()->KillHorizontalVelocity(2);
  }

  GetParent()->SetPosition(pos);
}

void TileComponent::SnapVertical(int centerShift)
{
  Vector4 pos = GetParent()->GetPosition();

  // Figure out which axis to snap to
  if (m_axis == 0 || m_axis == 2)
    pos.m_y = std::floor(pos.m_y + m_offset.m_y) + (.5f + centerShift*(.5f - .5f*m_fheight) - m_offset.m_y);
  else if (m_axis == 1)
    pos.m_z = std::floor(pos.m_z + m_offset.m_z) + (.5f + centerShift*(.5f - .5f*m_fdepth) - m_offset.m_z);

  GetParent()->SetPosition(pos);
}

void TileComponent::SnapDepth()
{
  Vector4 pos = GetParent()->GetPosition();

  // Figure out which axis to snap to
  pos.m_a[m_axis] = std::floor(pos.m_a[m_axis] + m_offset.m_a[m_axis]) + (.5f - m_offset.m_a[m_axis]);

  GetParent()->SetPosition(pos);
}

void TileComponent::SetAnchorAxis(int axis)
{
  if (axis == WorldAxis::Front || axis == WorldAxis::Right)
    m_anchor_axis = axis;
}

void TileComponent::SetAnchorBegin(int begin)
{
  m_anchor_begin = begin;
}

void TileComponent::SetAnchorEnd(int end)
{
  m_anchor_end = end;
}

void TileComponent::DebugDraw()
{
  // Update position
  Matrix4 matrix;
  matrix.Identity();
  Vector4 pos = GetParent()->GetPosition();
  Vector3 pos3 = pos.ToVector3();

  // Debug draw origin
  DebugDraw::AddCoordinates(pos3, matrix, 1.0f);

  // Debug draw hotspots
  int size = m_hs_hor[m_axis].size();
  for (int i = 0; i < size; ++i)
    DebugDraw::AddQuad(pos3 + m_hs_hor[m_axis][i], matrix, Vector2(0.1f, 0.1f));
  size = m_hs_ver[m_axis].size();
  for (int i = 0; i < size; ++i)
    DebugDraw::AddQuad(pos3 + m_hs_ver[m_axis][i], matrix, Vector2(0.1f, 0.1f));

  // Debug draw collider
  Vector3 debug_pos = pos3 + m_offset;
  debug_pos.m_x += m_width / 2.0f - 0.5f;
  debug_pos.m_y += m_height / 2.0f - 0.5f;
  debug_pos.m_z += m_depth / 2.0f - 0.5f;
  DebugDraw::AddCube(debug_pos, matrix, Vector3(m_width, m_height, m_depth));
}
void TileComponent::OnRotateStart(int start_axis, int end_axis)
{
  //g_engine_core->DispatchEvent("PushAudioEvent", "Play_rotate");
  //if(g_anchor_ability && end_axis == m_anchor_axis && m_anchor_end == -1)
  //{
  //  Anchor();
  //}

  m_prev_axis = start_axis;
  m_axis = end_axis;

  Tilemap *special_tilemap = g_engine_core->SystemCall("TileSystem", "GetTilemaps", "Special");
  Tile curr_type = special_tilemap->GetTile(m_tile_pos);

  switch(curr_type)
  {
  case 2:
    if(m_axis == 0)
    {
      TileGameplayComponent* specialTile = special_tilemap->GetTileObject(m_tile_pos.m_x, m_tile_pos.m_y, m_tile_pos.m_z);
      specialTile->SetPreviousAxis(start_axis);
      if(specialTile->IsRotateActive())
        specialTile->GetSibling<SpriteComponent>()->SetFrame(((specialTile->GetDefaultAxis() - start_axis + 3) % 3) * 2 + 1);
    }
    break;
  case 3:
    if(m_axis == 1)
    {
      TileGameplayComponent* specialTile = special_tilemap->GetTileObject(m_tile_pos.m_x, m_tile_pos.m_y, m_tile_pos.m_z);
      specialTile->SetPreviousAxis(start_axis);
      if(specialTile->IsRotateActive())
        specialTile->GetSibling<SpriteComponent>()->SetFrame(((specialTile->GetDefaultAxis() - start_axis + 3) % 3) * 2 + 1);
    }
    break;
  case 4:
    if(m_axis == 2)
    {
      TileGameplayComponent* specialTile = special_tilemap->GetTileObject(m_tile_pos.m_x, m_tile_pos.m_y, m_tile_pos.m_z);
      specialTile->SetPreviousAxis(start_axis);
      if(specialTile->IsRotateActive())
        specialTile->GetSibling<SpriteComponent>()->SetFrame(((specialTile->GetDefaultAxis() - start_axis + 3) % 3) * 2 + 1);
    }
    break;
  }
  if(curr_type > 0 && curr_type < 5)
  {
    switch(start_axis)
    {
    case 0:
      g_engine_core->DispatchEvent("OnSideToggle", true);
      break;
    case 1:
      g_engine_core->DispatchEvent("OnTopToggle", true);
      break;
    case 2:
      g_engine_core->DispatchEvent("OnFrontToggle", true);
      break;
    }
  }

  g_engine_core->DispatchEvent("HideHudItem", "RotationPrompt");
}
void TileComponent::OnRotateComplete(int start_axis, int end_axis)
{
  if(start_axis != end_axis)
  {
    if(!g_first_rotate)
    { 
      g_first_rotate = true;
      HUDNarrativePrompt(FIRST_ROTATION);
    }
    else if(!g_first_anchor && end_axis == m_anchor_axis)
    {
      g_first_anchor = true;
      HUDNarrativePrompt(FIRST_ANCHOR);
    }
  }
  m_grounded_tile_change = true;
  m_tile_change = true;

  if(g_rotate_ability && !g_free_rotated)
  {
    Tilemap *special_tilemap = g_engine_core->SystemCall("TileSystem", "GetTilemaps", "Special");
    Tile type = special_tilemap->GetTile(m_tile_pos);
    if(!(type == 2 || type == 3 || type == 4 || type == 51))
      g_free_rotated = true;
  }
}
void TileComponent::OnAxisChange(int old_axis, int new_axis)
{
  if(m_initialized)
  {
    g_engine_core->DispatchEvent("PushAudioEvent", "Stop_rotate");
    g_engine_core->DispatchEvent("PushAudioEvent", "Play_rotate");
    Tilemap *special_tilemap = g_engine_core->SystemCall("TileSystem", "GetTilemaps", "Special");

    Tile curr_type = special_tilemap->GetTile(GetTilePos3());
    Tile actual_type = special_tilemap->GetTile(m_tile_pos);
    if((!g_rotate_ability && curr_type < 2 || curr_type > 4) && actual_type)
      GetParent()->SetPosition(Vector3(m_tile_pos.m_x+.5f, m_tile_pos.m_y+.5f, m_tile_pos.m_z+.5f));
  }

  m_axis = new_axis;
  SnapDepth();

  DynamicComponent *dyn = GetSibling<DynamicComponent>();
  if (dyn) dyn->SetVelocity(Vector3(0, 0, 0));

  m_rotating = true;
}

void TileComponent::OnAxisChangeComplete(int axis)
{
  m_rotating = false;
}

void TileComponent::OnAxisInitalize(int axis)
{
  m_levitate_rate = Vector3(0.f, .75f, 0.f);
  m_initialized = true;
  TilePos3 tpos = GetTilePos3();

  g_engine_core->DispatchEvent("OnSideToggle", g_rotate_ability);
  g_engine_core->DispatchEvent("OnTopToggle", g_rotate_ability);
  g_engine_core->DispatchEvent("OnFrontToggle", g_rotate_ability);
  m_tile_pos = tpos;
  m_grounded_pos = m_tile_pos;
  m_last_grounded_pos = m_tile_pos;
  m_last_safe_pos = m_tile_pos;
  m_last_tile_pos = m_tile_pos;
  int startTrack = GetParent()->GetComponent<SpriteComponent>()->GetCurrentAnimation();
  if(g_player_facing == -1)
  {
    switch(startTrack)
    {
    case 0:
      m_facing = RIGHT;
      break;
    case 1:
      m_facing = LEFT;
      break;
    case 2:
      m_facing = UP;
      break;
    case 3:
      m_facing = DOWN;
      break;
    }
    g_player_facing = static_cast<int>(m_facing);
  }
  else
    m_facing = static_cast<PLAYERFACING>(g_player_facing);

  if(g_coin_count == 0)
  {
    g_coins_needed = 5;
    ChangeMemoryLimit(m_currMemorySize);
  }
  else
    ChangeMemoryLimit(m_memorySize);

  g_engine_core->DispatchEvent("OnToggleCoinVisible", true);

  UpdateMemory(0);
  UpdateMemory(1);
  UpdateMemory(2);

  Tilemap *special_tilemap = g_engine_core->SystemCall("TileSystem", "GetTilemaps", "Special");
  if(!g_rotate_ability && !g_rotate_blocks)
    special_tilemap->ToggleRotationTiles(false);
  if(!g_anchor_ability && g_anchor_blocks)
  {
    special_tilemap->ToggleAnchorTilesActive(false, 0);
    special_tilemap->ToggleAnchorTilesActive(false, 2);
  }

  special_tilemap->SetArtifactsActive();
  special_tilemap->SetCollectiblesActive();

  //Play the musaks
  if(!g_coin_count)
  {
    //g_engine_core->DispatchEvent("OnNewGame");
    g_engine_core->DispatchEvent("PushAudioEvent", "Play_ambient");
    HUDNarrativePrompt(OPENING, 2.5f, true);
  }
  else
  {
    int level = g_engine_core->SystemCall("TileSystem", "GetLevelNumber");
    if( level >= 1 && g_curr_level != 22 )
      g_engine_core->DispatchEvent("PushAudioEvent", "Play_hub_music");
    else if(g_curr_level == 22)
      g_engine_core->DispatchEvent("PushAudioEvent", "Play_ambient");
    else
      g_engine_core->DispatchEvent("PushAudioEvent", "Play_level_music");
  }
}

void TileComponent::OnGrounded()
{
  TilePos3 pos = GetTilePos3();
  //std::printf("Grounded: (%i, %i, %i)\n", pos.m_x, pos.m_y, pos.m_z);
}

bool TileComponent::CheckGrounded(Tilemap *tilemap, Vector3 offset)
{
  Vector3 final_offset = Vector3(0.0f, -0.001f, 0.0f) + offset;
  int size = m_hs_ver[m_axis].size();
  for (int i = 2; i < size; i += 4)
  {
    char tile_type = GetCollisionTileAtOffset(tilemap, m_hs_ver[m_axis][i] + final_offset);    
    if(tile_type >= 1 && tile_type <= 2)
    {
      GetParent()->Invoke("GroundType", tile_type);
      return true;
    }

    tile_type = GetCollisionTileAtOffset(tilemap, m_hs_ver[m_axis][i + 1] + final_offset);
    if(tile_type >= 1 && tile_type <= 2)
    {
      GetParent()->Invoke("GroundType", tile_type);
      return true;
    }
    /*if (GetCollisionTileAtOffset(tilemap, m_hs_ver[m_axis][i] + final_offset) == 1)
      return true;
    if (GetCollisionTileAtOffset(tilemap, m_hs_ver[m_axis][i + 1] + final_offset) == 1)
      return true;*/
  }

  return false;
}

void TileComponent::UpdateTilePos()
{
  Vector4 pos = GetParent()->GetPosition();
  //printf("Position: (%f, %f, %f)\n", pos.m_x, pos.m_y, pos.m_z);
  m_layer = static_cast<int>(std::floor(pos.m_a[m_axis]));
}

void TileComponent::ResolveHorizontalCollision(Vector3 contact_offset)
{
  DynamicComponent *dyn = GetSibling<DynamicComponent>();
  if (!dyn)
    return;

  // Calculate the normal and resolve the collision
  Vector3 v = dyn->GetVelocity();
  Vector3 pos = GetParent()->GetPosition().ToVector3();
  g_engine_core->DispatchEvent("OnPlayerSideCollision");
  if (m_axis == 0 || m_axis == 1)
  {
    Vector3 normal((contact_offset.m_x < 0) ? -1.0f : 1.0f, 0.0f, 0.0f);
    dyn->ResolveCollision(normal);
    DebugDraw::AddArrow(pos + contact_offset, -normal, 1.0f);
  }
  if (m_axis == 2)
  {
    Vector3 normal(0.0f, 0.0f, (contact_offset.m_z < 0) ? -1.0f : 1.0f);
    dyn->ResolveCollision(normal);
    DebugDraw::AddArrow(pos + contact_offset, -normal, 1.0f);
  }
}

void TileComponent::ResolveVerticalCollision(Vector3 contact_offset)
{
  DynamicComponent *dyn = GetSibling<DynamicComponent>();
  if (!dyn)
    return;

  // Calculate the normal and resolve the collision
  Vector3 v = dyn->GetVelocity();
  Vector3 pos = GetParent()->GetPosition().ToVector3();
  if (m_axis == 0 || m_axis == 2)
  {
    if(v.m_y > 0 && !m_is_grounded)
      g_engine_core->DispatchEvent("OnPlayerHeadCollision");
    Vector3 normal(0.0f, (contact_offset.m_y < 0) ? -1.0f : 1.0f, 0.0f);
    bool applyFriction = contact_offset.m_y < 0 ? true : false;
    dyn->ResolveCollision(normal);
    DebugDraw::AddArrow(pos + contact_offset, -normal, 1.0f);
  }
  if (m_axis == 1)
  {
    Vector3 normal(0.0f, 0.0f, (contact_offset.m_z < 0) ? -1.0f : 1.0f);
    dyn->ResolveCollision(normal);
    DebugDraw::AddArrow(pos + contact_offset, -normal, 1.0f);
  }
}

void TileComponent::ResolveDepthCollision(Vector3 contact_offset)
{
  DynamicComponent *dyn = GetSibling<DynamicComponent>();
  if (!dyn)
    return;

  // Calculate the normal and resolve the collision
  Vector3 pos = GetParent()->GetPosition().ToVector3();
  Vector3 normal(0, 0, 0);
  normal.m_a[m_axis] = (contact_offset.m_a[m_axis] < 0.0f) ? -1.0f : 1.0f;
  dyn->ResolveCollision(normal);
  DebugDraw::AddArrow(pos + contact_offset, -normal, 1.0f);
}

TilePos2 TileComponent::GetTilePos2()
{
  Vector4 pos = GetParent()->GetPosition();
  return Tilemap::GetTilePos(pos, m_axis);
}

TilePos3 TileComponent::GetTilePos3()
{
  Vector4 pos = GetParent()->GetPosition();
  return Tilemap::GetTilePos(pos);
}

char TileComponent::GetCollisionTileAtOffset(Tilemap *tilemap, Vector3 offset)
{
  char returnval = tilemap->GetTile(GetParent()->GetPosition() + offset);
  return returnval;
  //return tilemap->GetTile(GetParent()->GetPosition() + offset);
}

bool TileComponent::IsGrounded()
{
  return m_is_grounded;
}

void TileComponent::OnAnchorTileEnter()
{
  Anchor();
}
void TileComponent::Anchor(bool undo)
{
  if(undo)
  {
    m_anchor_end = -1;
    g_engine_core->SystemCall("TileSystem", "SetAnchorEnd", m_anchor_end);
    g_engine_core->DispatchEvent("OnAnchorEvent", 2);
    return;
  }
  // No anchor set
  if (m_anchor_axis == -1)
  {
    if (m_axis != 1)
    {
      m_ommited_layers.clear();
      g_engine_core->SystemCall("GraphicsSystem", "SetOmmitedLayers", reinterpret_cast<int*>(&m_ommited_layers));
      m_anchor_commit = false;
      m_anchor_axis = m_axis ? 0 : 2;
      g_engine_core->SystemCall("TileSystem", "SetAnchorAxis", m_anchor_axis);
      m_anchor_begin = int(GetParent()->GetPosition().m_a[m_anchor_axis]);
      g_engine_core->SystemCall("TileSystem", "SetAnchorBegin", m_anchor_begin);
      g_engine_core->SystemCall("TileSystem", "SetAnchorRange", m_anchor_range);
      g_engine_core->DispatchEvent("OnAnchorEvent", 0);
    }
  }
  // first anchor set
  else if(m_anchor_end < 0)
  {
    if (m_axis != m_anchor_axis)
    {
      if(int(GetParent()->GetPosition().m_a[m_anchor_axis]) == m_anchor_begin)
        return;
      m_anchor_end = int(GetParent()->GetPosition().m_a[m_anchor_axis]);
      if(abs(m_anchor_end - m_anchor_begin) > m_anchor_range)
        m_anchor_end = m_anchor_end > m_anchor_begin ? m_anchor_begin + m_anchor_range : m_anchor_begin - m_anchor_range;

      g_engine_core->SystemCall("TileSystem", "SetAnchorEnd", m_anchor_end);
      g_engine_core->DispatchEvent("OnAnchorEvent", 1);
    }
    else
    {
      m_anchor_commit = false;
      m_anchor_axis = -1;
      m_anchor_begin = -1;
      m_anchor_end = -1;
      m_anchor_enabled = false;

      g_engine_core->SystemCall("TileSystem", "SetAnchorAxis", m_anchor_axis);
      g_engine_core->SystemCall("TileSystem", "SetAnchorBegin", m_anchor_begin);
      g_engine_core->SystemCall("TileSystem", "SetAnchorEnd", m_anchor_end);

      g_engine_core->DispatchEvent("OnAnchorEvent", 2);
    }
  }
  else
  {
    int currLayer =  int(GetParent()->GetPosition().m_a[m_anchor_axis]);
    if(g_nega_binding && currLayer < m_anchor_begin && currLayer > m_anchor_end && m_anchor_axis != m_axis)
    {
      std::list<int>::iterator layerInMem = std::find(m_ommited_layers.begin(), m_ommited_layers.end(), currLayer);

      if(layerInMem == m_ommited_layers.end())
        m_ommited_layers.push_back(currLayer);
      else
        m_ommited_layers.remove(currLayer);

      g_engine_core->SystemCall("GraphicsSystem", "SetOmmitedLayers", reinterpret_cast<int*>(&m_ommited_layers));
    }
    else
    {
      m_anchor_commit = false;
      m_anchor_axis = -1;
      m_anchor_begin = -1;
      m_anchor_end = -1;
      m_anchor_enabled = false;

      g_engine_core->SystemCall("TileSystem", "SetAnchorAxis", m_anchor_axis);
      g_engine_core->SystemCall("TileSystem", "SetAnchorBegin", m_anchor_begin);
      g_engine_core->SystemCall("TileSystem", "SetAnchorEnd", m_anchor_end);
      g_engine_core->DispatchEvent("OnAnchorEvent", 2);
    }
  }
}
void TileComponent::OnKeyPress(KeyboardEvent key)
{
  Tilemap *special_tilemap = g_engine_core->SystemCall("TileSystem", "GetTilemaps", "Special");
  int level;
  switch(key.m_keycode)
  {
  case kKeyBackspace:
    level = g_engine_core->SystemCall("TileSystem", "GetLevelNumber");
    if(level != 1)
      SpaceWrapup();
    break;
  case kKeyEnter:
    m_skip_text = true;
    break;
  case kKeyMinus:
#ifndef _DEBUG
    break;
#endif
    g_curr_coin_count++;
    g_engine_core->DispatchEvent("OnUpdateCoinNumber", g_curr_coin_count);

    break;
  case kKeyPlus:
#ifndef _DEBUG
    break;
#endif
    ChangeMemoryLimit(m_currMemorySize + 1);
    if(!g_rotate_blocks)
    {
      g_rotate_blocks = true;
      special_tilemap->ToggleRotationTiles(true);
    }
    else if(!g_rotate_ability)
    {
      g_rotate_ability = true;
      g_engine_core->DispatchEvent("OnSideToggle", true);
      g_engine_core->DispatchEvent("OnTopToggle", true);
      g_engine_core->DispatchEvent("OnFrontToggle", true);

      special_tilemap->ToggleRotationTiles(false);
      g_engine_core->DispatchEvent("HideHudItem", "RotationPrompt");
    }
    else if(!g_anchor_blocks)
    {
      g_anchor_blocks = true;
      special_tilemap->ToggleAnchorTiles(true);
    }
    else if(!g_anchor_ability)
    {
      m_anchor_commit = false;
      m_anchor_axis = -1;
      m_anchor_begin = -1;
      m_anchor_end = -1;

      g_engine_core->SystemCall("TileSystem", "SetAnchorAxis", m_anchor_axis);
      g_engine_core->SystemCall("TileSystem", "SetAnchorBegin", m_anchor_begin);
      g_engine_core->SystemCall("TileSystem", "SetAnchorEnd", m_anchor_end);
      m_silent_anchor = true;
      g_engine_core->DispatchEvent("OnAnchorEvent", 2);
      m_silent_anchor = false;

      g_anchor_ability = true;
      special_tilemap->ToggleAnchorTiles(false);
    }
    break;
  case kKeyShift:
    if(m_anchor_enabled || g_anchor_ability && !m_rotating)
    {
      m_anchor_commit = true;
      Anchor();
    }
    else
    {
      if(m_anchor_end > -1)
      {
        special_tilemap->ToggleAnchorTilesActive(false, m_anchor_axis);
        m_anchor_commit = false;
        m_anchor_axis = -1;
        m_anchor_begin = -1;
        m_anchor_end = -1;
        m_anchor_enabled = false;

        g_engine_core->SystemCall("TileSystem", "SetAnchorAxis", m_anchor_axis);
        g_engine_core->SystemCall("TileSystem", "SetAnchorBegin", m_anchor_begin);
        g_engine_core->SystemCall("TileSystem", "SetAnchorEnd", m_anchor_end);
        g_engine_core->DispatchEvent("OnAnchorEvent", 2);
      }
    }
    break;
  //case kKeyS:
  //  if(m_anchor_end > -1 && m_axis == m_anchor_axis)
  //  {
  //    if(m_is_grounded && !m_drop)
  //    {
  //      m_drop = true;
  //      m_drop_y = GetParent()->GetPosition().m_y;
  //    }
  //  }
  //  break;
  }
}

void TileComponent::OnPlayerDrop()
{
  if(m_anchor_end > -1 && m_axis == m_anchor_axis)
  {
    if(m_is_grounded && !m_drop)
    {
      m_drop = true;
      m_drop_y = GetParent()->GetPosition().m_y;
    }
  }
}
void TileComponent::RotateTo(int axis)
{
  int temp_prev = m_prev_axis;
  m_prev_axis = m_axis;

  if(axis == m_axis)
    g_engine_core->DispatchEvent("OnRotateToTarget", temp_prev);
  else
    g_engine_core->DispatchEvent("OnRotateToTarget", axis);
}
void TileComponent::EnableRotateTo(int axis, bool enable)
{
  switch(axis)
  {
  case 0:
    g_engine_core->DispatchEvent("OnSideToggle", enable);
    break;
  case 1:
    g_engine_core->DispatchEvent("OnTopToggle", enable);
    break;
  case 2:
    g_engine_core->DispatchEvent("OnFrontToggle", enable);
    break;
  }
}

void TileComponent::ChangeMemoryLimit(int newLimit)
{
  m_currMemorySize = newLimit;
  UpdateMemory(0);
  UpdateMemory(1);
  UpdateMemory(2);
  g_engine_core->SystemCall("TileSystem", "SetMemoryRange", newLimit);
}
//void TileComponent::UpdateMemory(int axis)
//{
//  int back, front;
//  std::list<int> removals;
//  std::list<MemoryLayer>::iterator foundLayer;
//  switch(axis)
//  {
//    // Side Memory (x-axis)
//  case 0:
//    // Age visible memory
//    if(axis == m_axis)
//    {
//      for(std::list<MemoryLayer>::iterator it = m_SideMemory.begin(); it != m_SideMemory.end(); ++it)
//      {
//        it->age += m_dt;
//        if(it->age >= it->maxAge)
//          removals.push_front(it->layer);
//      }
//      for(std::list<int>::iterator it = removals.begin(); it != removals.end(); ++it)
//        m_SideMemory.remove(*std::find(m_SideMemory.begin(), m_SideMemory.end(), *it));
//    }
//
//    // Add new memory
//    // current layer
//    foundLayer = std::find(m_SideMemory.begin(), m_SideMemory.end(), m_tile_pos.m_a[axis]);
//    if(foundLayer != m_SideMemory.end())
//      m_SideMemory.remove(*foundLayer);
//    m_SideMemory.push_front(MemoryLayer(m_tile_pos.m_a[axis], m_maxAge));
//
//    // back layer
//    foundLayer = std::find(m_SideMemory.begin(), m_SideMemory.end(), m_tile_pos.m_a[axis] - 1);
//    if(foundLayer != m_SideMemory.end())
//      m_SideMemory.remove(*foundLayer);
//    m_SideMemory.push_front(MemoryLayer(m_tile_pos.m_a[axis] - 1, m_maxAge));
//
//    // front layer
//    foundLayer = std::find(m_SideMemory.begin(), m_SideMemory.end(), m_tile_pos.m_a[axis] + 1);
//    if(foundLayer != m_SideMemory.end())
//      m_SideMemory.remove(*foundLayer);
//    m_SideMemory.push_front(MemoryLayer(m_tile_pos.m_a[axis] + 1, m_maxAge));
//
//    while(m_SideMemory.size() > m_MemorySize)
//      m_SideMemory.pop_back();
//    g_engine_core->SystemCall("GraphicsSystem", "SetSideMemory", reinterpret_cast<int*>(&m_SideMemory));
//
//    break;
//    // Top Memory (y-axis)
//  case 1:
//    // Age visible memory
//    if(axis == m_axis)
//    {
//      for(std::list<MemoryLayer>::iterator it = m_TopMemory.begin(); it != m_TopMemory.end(); ++it)
//      {
//        it->age += m_dt;
//        if(it->age >= it->maxAge)
//          removals.push_front(it->layer);
//      }
//      for(std::list<int>::iterator it = removals.begin(); it != removals.end(); ++it)
//        m_TopMemory.remove(*std::find(m_TopMemory.begin(), m_TopMemory.end(), *it));
//    }
//
//    // Add new memory
//    // current layer
//    foundLayer = std::find(m_TopMemory.begin(), m_TopMemory.end(), m_tile_pos.m_a[axis]);
//    if(foundLayer != m_TopMemory.end())
//      m_TopMemory.remove(*foundLayer);
//    m_TopMemory.push_front(MemoryLayer(m_tile_pos.m_a[axis], m_maxAge));
//
//    // back layer
//    foundLayer = std::find(m_TopMemory.begin(), m_TopMemory.end(), m_tile_pos.m_a[axis] - 1);
//    if(foundLayer != m_TopMemory.end())
//      m_TopMemory.remove(*foundLayer);
//    m_TopMemory.push_front(MemoryLayer(m_tile_pos.m_a[axis] - 1, m_maxAge));
//
//    // front layer
//    foundLayer = std::find(m_TopMemory.begin(), m_TopMemory.end(), m_tile_pos.m_a[axis] + 1);
//    if(foundLayer != m_TopMemory.end())
//      m_TopMemory.remove(*foundLayer);
//    m_TopMemory.push_front(MemoryLayer(m_tile_pos.m_a[axis] + 1, m_maxAge));
//
//    while(m_TopMemory.size() > m_MemorySize)
//      m_TopMemory.pop_back();
//    g_engine_core->SystemCall("GraphicsSystem", "SetTopMemory", reinterpret_cast<int*>(&m_TopMemory));
//    break;
//    // Front Memory (z-axis)
//  case 2:
//    // Age visible memory
//    if(axis == m_axis)
//    {
//      for(std::list<MemoryLayer>::iterator it = m_FrontMemory.begin(); it != m_FrontMemory.end(); ++it)
//      {
//        it->age += m_dt;
//        if(it->age >= it->maxAge)
//          removals.push_front(it->layer);
//      }
//      for(std::list<int>::iterator it = removals.begin(); it != removals.end(); ++it)
//        m_FrontMemory.remove(*std::find(m_FrontMemory.begin(), m_FrontMemory.end(), *it));
//    }
//
//    // Add new memory
//    // current layer
//    foundLayer = std::find(m_FrontMemory.begin(), m_FrontMemory.end(), m_tile_pos.m_a[axis]);
//    if(foundLayer != m_FrontMemory.end())
//      m_FrontMemory.remove(*foundLayer);
//    m_FrontMemory.push_front(MemoryLayer(m_tile_pos.m_a[axis], m_maxAge));
//
//    // back layer
//    foundLayer = std::find(m_FrontMemory.begin(), m_FrontMemory.end(), m_tile_pos.m_a[axis] - 1);
//    if(foundLayer != m_FrontMemory.end())
//      m_FrontMemory.remove(*foundLayer);
//    m_FrontMemory.push_front(MemoryLayer(m_tile_pos.m_a[axis] - 1, m_maxAge));
//
//    // front layer
//    foundLayer = std::find(m_FrontMemory.begin(), m_FrontMemory.end(), m_tile_pos.m_a[axis] + 1);
//    if(foundLayer != m_FrontMemory.end())
//      m_FrontMemory.remove(*foundLayer);
//    m_FrontMemory.push_front(MemoryLayer(m_tile_pos.m_a[axis] + 1, m_maxAge));
//
//    while(m_FrontMemory.size() > m_MemorySize)
//      m_FrontMemory.pop_back();
//    g_engine_core->SystemCall("GraphicsSystem", "SetFrontMemory", reinterpret_cast<int*>(&m_FrontMemory));
//    break;
//  }
//}
void TileComponent::UpdateMemory(int axis)
{
  //int back, front;
  switch(axis)
  {
  case 0:
    /*if(m_tile_pos.m_x != m_last_tile_pos.m_x)
    {
      if((axis != m_anchor_axis || m_anchor_end == -1) || (m_anchor_end != -1 && m_anchor_axis == axis && (m_anchor_end > m_last_tile_pos.m_a[axis] || m_anchor_begin < m_last_tile_pos.m_a[axis] )))
      {
        if(std::find(m_SideMemory.begin(), m_SideMemory.end(), m_last_tile_pos.m_x) == m_SideMemory.end())
        {
          m_SideMemory.push_front(MemoryLayer(m_last_tile_pos.m_x, 0));
        }
      }
    }
    back = m_tile_pos.m_a[axis] + 1;
    front = back - 2;

    if(m_anchor_end != -1 && m_anchor_axis == axis && (m_anchor_end <= m_tile_pos.m_a[axis] && m_anchor_begin >= m_tile_pos.m_a[axis] ))
    {
      back = m_anchor_begin + 1;
      front = m_anchor_end - 1;
    }*/

    //if(std::find(m_SideMemory.begin(), m_SideMemory.end(), back) == m_SideMemory.end())
    //  m_SideMemory.push_front(MemoryLayer(back, 0));
    //if(std::find(m_SideMemory.begin(), m_SideMemory.end(), front) == m_SideMemory.end())
    //  m_SideMemory.push_front(MemoryLayer(front, 0));

    //while(m_SideMemory.size() > m_MemorySize)
    //{
    //  int max = std::max_element(m_SideMemory.begin(), m_SideMemory.end())->layer;
    //  int min = std::min_element(m_SideMemory.begin(), m_SideMemory.end())->layer;

    //  int expired_layer = abs(m_tile_pos.m_x - max) > abs(m_tile_pos.m_x - min) ? max : min;
    //  m_SideMemory.remove(*std::find(m_SideMemory.begin(), m_SideMemory.end(), expired_layer));
    //}


    // Clear memory
    m_SideMemory.clear();
    // If binding
    if(m_anchor_end != -1 && m_anchor_axis == axis && (m_anchor_end <= m_tile_pos.m_a[axis] && m_anchor_begin >= m_tile_pos.m_a[axis] ))
    {
      // Add active layers
      for (int i = m_anchor_end; i <= m_anchor_begin; i++)
      {
        m_SideMemory.push_front(MemoryLayer(i, 0));
      }
      // Add each layer in front and behind active layers
      for (unsigned i = 1; i <= m_currMemorySize; i++)
      {
        m_SideMemory.push_front(MemoryLayer(m_anchor_end-i, 0));
        m_SideMemory.push_front(MemoryLayer(m_anchor_begin+i, 0));
      }
    }
    else
    {
      int currLayer = m_tile_pos.m_a[m_axis];
      m_SideMemory.push_front(MemoryLayer(currLayer, 0));
      // Add each layer in front and behind active layers
      for (unsigned i = 1; i <= m_currMemorySize; i++)
      {
        m_SideMemory.push_front(MemoryLayer(currLayer-i, 0));
        m_SideMemory.push_front(MemoryLayer(currLayer+i, 0));
      }
    }

    m_SideMemory.sort();
    g_engine_core->SystemCall("GraphicsSystem", "SetSideMemory", reinterpret_cast<int*>(&m_SideMemory));
    break;
  case 1:
    /*if(m_tile_pos.m_y != m_last_tile_pos.m_y)
    {
      if((axis != m_anchor_axis || m_anchor_end == -1) || (m_anchor_end != -1 && m_anchor_axis == axis && (m_anchor_end > m_last_tile_pos.m_a[axis] || m_anchor_begin < m_last_tile_pos.m_a[axis] )))
      {
        if(std::find(m_TopMemory.begin(), m_TopMemory.end(), m_last_tile_pos.m_y) == m_TopMemory.end())
        {
          m_TopMemory.push_front(MemoryLayer(m_last_tile_pos.m_y, 0));
        }
      }
    }
    back = m_tile_pos.m_a[axis] + 1;
    front = back - 2;

    if(std::find(m_TopMemory.begin(), m_TopMemory.end(), back) == m_TopMemory.end())
      m_TopMemory.push_front(MemoryLayer(back, 0));
    if(std::find(m_TopMemory.begin(), m_TopMemory.end(), front) == m_TopMemory.end())
      m_TopMemory.push_front(MemoryLayer(front, 0));

    while(m_TopMemory.size() > m_MemorySize)
    {
      int max = std::max_element(m_TopMemory.begin(), m_TopMemory.end())->layer;
      int min = std::min_element(m_TopMemory.begin(), m_TopMemory.end())->layer;

      int expired_layer = abs(m_tile_pos.m_y - max) > abs(m_tile_pos.m_y - min) ? max : min;
      m_TopMemory.remove(*std::find(m_TopMemory.begin(), m_TopMemory.end(), expired_layer));
    }*/



    // Clear memory
    m_TopMemory.clear();
    // If binding
    if(m_anchor_end != -1 && m_anchor_axis == axis && (m_anchor_end <= m_tile_pos.m_a[axis] && m_anchor_begin >= m_tile_pos.m_a[axis] ))
    {
      // Add active layers
      for (int i = m_anchor_end; i <= m_anchor_begin; i++)
      {
        m_TopMemory.push_front(MemoryLayer(i, 0));
      }
      // Add each layer in front and behind active layers
      for (unsigned i = 1; i <= m_currMemorySize; i++)
      {
        m_TopMemory.push_front(MemoryLayer(m_anchor_end-i, 0));
        m_TopMemory.push_front(MemoryLayer(m_anchor_begin+i, 0));
      }
    }
    else
    {
      int currLayer = m_tile_pos.m_a[m_axis];
      m_TopMemory.push_front(MemoryLayer(currLayer, 0));
      // Add each layer in front and behind active layers
      for (unsigned i = 1; i <= m_currMemorySize; i++)
      {
        m_TopMemory.push_front(MemoryLayer(currLayer-i, 0));
        m_TopMemory.push_front(MemoryLayer(currLayer+i, 0));
      }
    }

    m_TopMemory.sort();
    g_engine_core->SystemCall("GraphicsSystem", "SetTopMemory", reinterpret_cast<int*>(&m_TopMemory));
    break;
  case 2:
    /*if(m_tile_pos.m_z != m_last_tile_pos.m_z)
    {
      if((axis != m_anchor_axis || m_anchor_end == -1) || (m_anchor_end != -1 && m_anchor_axis == axis && (m_anchor_end > m_last_tile_pos.m_a[axis] || m_anchor_begin < m_last_tile_pos.m_a[axis] )))
      {
        if(std::find(m_FrontMemory.begin(), m_FrontMemory.end(), m_last_tile_pos.m_z) == m_FrontMemory.end())
        {
          m_FrontMemory.push_front(MemoryLayer(m_last_tile_pos.m_z, 0));
        }
      }
    }

    back = m_tile_pos.m_a[axis] + 1;
    front = back - 2;

    if(m_anchor_end != -1 && m_anchor_axis == axis && (m_anchor_end <= m_tile_pos.m_a[axis] && m_anchor_begin >= m_tile_pos.m_a[axis] ))
    {
      back = m_anchor_begin + 1;
      front = m_anchor_end - 1;
    }

    if(std::find(m_FrontMemory.begin(), m_FrontMemory.end(), back) == m_FrontMemory.end())
      m_FrontMemory.push_front(MemoryLayer(back, 0));
    if(std::find(m_FrontMemory.begin(), m_FrontMemory.end(), front) == m_FrontMemory.end())
      m_FrontMemory.push_front(MemoryLayer(front, 0));

    while(m_FrontMemory.size() > m_MemorySize)
    {
      int max = std::max_element(m_FrontMemory.begin(), m_FrontMemory.end())->layer;
      int min = std::min_element(m_FrontMemory.begin(), m_FrontMemory.end())->layer;

      int expired_layer = abs(m_tile_pos.m_z - max) > abs(m_tile_pos.m_z - min) ? max : min;
      m_FrontMemory.remove(*std::find(m_FrontMemory.begin(), m_FrontMemory.end(), expired_layer));
    }*/


    // Clear memory
    m_FrontMemory.clear();
    // If binding
    if(m_anchor_end != -1 && m_anchor_axis == axis && (m_anchor_end <= m_tile_pos.m_a[axis] && m_anchor_begin >= m_tile_pos.m_a[axis] ))
    {
      // Add active layers
      for (int i = m_anchor_end; i <= m_anchor_begin; i++)
      {
        m_FrontMemory.push_front(MemoryLayer(i, 0));
      }
      // Add each layer in front and behind active layers
      for (unsigned i = 1; i <= m_currMemorySize; i++)
      {
        m_FrontMemory.push_front(MemoryLayer(m_anchor_end-i, 0));
        m_FrontMemory.push_front(MemoryLayer(m_anchor_begin+i, 0));
      }
    }
    else
    {
      int currLayer = m_tile_pos.m_a[m_axis];
      m_FrontMemory.push_front(MemoryLayer(currLayer, 0));
      // Add each layer in front and behind active layers
      for (unsigned i = 1; i <= m_currMemorySize; i++)
      {
        m_FrontMemory.push_front(MemoryLayer(currLayer-i, 0));
        m_FrontMemory.push_front(MemoryLayer(currLayer+i, 0));
      }
    }

    m_FrontMemory.sort();
    g_engine_core->SystemCall("GraphicsSystem", "SetFrontMemory", reinterpret_cast<int*>(&m_FrontMemory));
    break;
  }
}

void TileComponent::HUDRotatePrompt(bool enable)
{
  if(g_rotate_ability)
    return;

  Tilemap *special_tilemap = g_engine_core->SystemCall("TileSystem", "GetTilemaps", "Special");
  TileGameplayComponent* specialTile;
  
  if(enable)
    specialTile = special_tilemap->GetTileObject(m_grounded_pos.m_x, m_grounded_pos.m_y, m_grounded_pos.m_z);
  else
    specialTile = special_tilemap->GetTileObject(m_last_grounded_pos.m_x, m_last_grounded_pos.m_y, m_last_grounded_pos.m_z);

  int currentAbility = 0;

  if(specialTile->GetDefaultAxis() != m_axis)
  {
    currentAbility = specialTile->GetDefaultAxis();
  }
  else
  {
    currentAbility = specialTile->GetPreviousAxis();
  }
  // Just have the one HUD element for rotation, and update its text dynamically
  // turn off during rotation
  // turn off when stepped off tile
  // turn on when on tile
  switch(currentAbility)
  {
  case 0:
    if(enable)
    {
      g_engine_core->DispatchEvent("DisplayHudItem","RotationPrompt");
      g_engine_core->DispatchEvent("OnQueryControllerConfig");
      switch(m_controller_config)
      {
      case NORMAL_DIRECTIONAL:
        g_engine_core->DispatchEvent("UpdateHudText", "RotationPrompt", "Press RIGHT");
        break;
      case NORMAL_POSITIONAL:
        g_engine_core->DispatchEvent("UpdateHudText", "RotationPrompt", "Press RIGHT");
        break;
      case SOUTHPAW_DIRECTIONAL:
        g_engine_core->DispatchEvent("UpdateHudText", "RotationPrompt", "Press 'D'");
        break;
      case SOUTHPAW_POSITIONAL:
        g_engine_core->DispatchEvent("UpdateHudText", "RotationPrompt", "Press 'D'");
        break;
      }
    }
    else
      g_engine_core->DispatchEvent("HideHudItem","RotationPrompt");
    break;
  case 1:
    if(enable)
    {
      g_engine_core->DispatchEvent("DisplayHudItem","RotationPrompt");
      g_engine_core->DispatchEvent("OnQueryControllerConfig");
      switch(m_controller_config)
      {
      case NORMAL_DIRECTIONAL:
        g_engine_core->DispatchEvent("UpdateHudText", "RotationPrompt", "Press UP");
        break;
      case NORMAL_POSITIONAL:
        g_engine_core->DispatchEvent("UpdateHudText", "RotationPrompt", "Press DOWN");
        break;
      case SOUTHPAW_DIRECTIONAL:
        g_engine_core->DispatchEvent("UpdateHudText", "RotationPrompt", "Press 'W'");
        break;
      case SOUTHPAW_POSITIONAL:
        g_engine_core->DispatchEvent("UpdateHudText", "RotationPrompt", "Press 'A'");
        break;
      }
    }
    else
      g_engine_core->DispatchEvent("HideHudItem","RotationPrompt");
    break;
  case 2:
    if(enable)
    {
      g_engine_core->DispatchEvent("DisplayHudItem","RotationPrompt");
      if(m_axis == 0)
      {
        g_engine_core->DispatchEvent("OnQueryControllerConfig");
        switch(m_controller_config)
        {
        case NORMAL_DIRECTIONAL:
          g_engine_core->DispatchEvent("UpdateHudText", "RotationPrompt", "Press LEFT");
          break;
        case NORMAL_POSITIONAL:
          g_engine_core->DispatchEvent("UpdateHudText", "RotationPrompt", "Press LEFT");
          break;
        case SOUTHPAW_DIRECTIONAL:
          g_engine_core->DispatchEvent("UpdateHudText", "RotationPrompt", "Press 'A'");
          break;
        case SOUTHPAW_POSITIONAL:
          g_engine_core->DispatchEvent("UpdateHudText", "RotationPrompt", "Press 'S'");
          break;
        }
      }
      else
      {
        g_engine_core->DispatchEvent("OnQueryControllerConfig");
        switch(m_controller_config)
        {
        case NORMAL_DIRECTIONAL:
          g_engine_core->DispatchEvent("UpdateHudText", "RotationPrompt", "Press DOWN");
          break;
        case NORMAL_POSITIONAL:
          g_engine_core->DispatchEvent("UpdateHudText", "RotationPrompt", "Press LEFT");
          break;
        case SOUTHPAW_DIRECTIONAL:
          g_engine_core->DispatchEvent("UpdateHudText", "RotationPrompt", "Press 'S'");
          break;
        case SOUTHPAW_POSITIONAL:
          g_engine_core->DispatchEvent("UpdateHudText", "RotationPrompt", "Press 'S'");
          break;
        }
      }
    }
    else
      g_engine_core->DispatchEvent("HideHudItem","RotationPrompt");
    break;
  }
}


void TileComponent::HUDNarrativePrompt(NARRATIVEMESSAGES message_id, float time, bool freeze)
{
  //NewMessage(DisplayHudItem)
  //  NewMessage(IsHudItemDisplayed)
  //  NewMessage(UpdateHudText)
  //  NewMessage(HideHudItem)
  //  NewMessage(ChangeHudItemPos)
  if(message_id == NO_NARRATIVE_MESSAGE)
  {
    if(!m_prompt_continue && (m_narrate_count <= 0.f || m_skip_text))
    {
      m_narrating = false;
      m_narrate_count = 0.f;
      g_engine_core->DispatchEvent("HideHudItem","NarrationPrompt");
      if(!m_leveling && !m_dead && !m_warping)
        g_engine_core->DispatchEvent("OnPlayerDeathToggle", false);
      return;
    }
    else if(m_prompt_continue)
    {
      if(m_skip_text)
      {
        m_narrating = false;
        m_narrate_count = 0.f;
        m_skip_text = false;
        m_prompt_continue = false;
        g_engine_core->DispatchEvent("HideHudItem","NarrationPrompt");
        if(!m_leveling && !m_dead && !m_warping)
          g_engine_core->DispatchEvent("OnPlayerDeathToggle", false);
        return;

      }
      
      if(m_narrate_count <= 0.f)
      {
        if(m_narration_message.Find("Enter to Continue") == m_narration_message.k_end)
          m_narration_message += "<br><br><br>Enter to Continue...";
      }
      g_engine_core->DispatchEvent("DisplayHudItem","NarrationPrompt");
      g_engine_core->DispatchEvent("UpdateHudText", "NarrationPrompt", m_narration_message.GetCString());
    }
    m_narrate_count -= m_dt;

    return;
  }
  // Choose and display message
  else
  {
    String message;
    switch(message_id)
    {
    case MOVEMENT:
      g_engine_core->DispatchEvent("OnQueryControllerConfig");
      switch (m_controller_config)
      {
      case NORMAL_DIRECTIONAL:
        message = "Move with WASD";
        break;
      case NORMAL_POSITIONAL:
        message = "Move with WASD";
        break;
      case SOUTHPAW_DIRECTIONAL:
        message = "Move with the ARROW KEYS";
        break;
      case SOUTHPAW_POSITIONAL:
        message = "Move with ARROW KEYS";
        break;
      }
      break;
    case ANCHOR_DROPPING:
      g_engine_core->DispatchEvent("OnQueryControllerConfig");
      switch (m_controller_config)
      {
      case NORMAL_DIRECTIONAL:
        message = "DROP from BOUND platforms with 'S'";
          break;
      case NORMAL_POSITIONAL:
        message = "DROP from BOUND platforms with 'S'";
          break;
      case SOUTHPAW_DIRECTIONAL:
        message = "DROP from BOUND platforms <br>\
                   with the DOWN ARROW";
          break;
      case SOUTHPAW_POSITIONAL:
        message = "DROP from BOUND platforms <br>\
                   with the DOWN ARROW";
          break;
      }
      break;
    case ROTATING:
      g_engine_core->DispatchEvent("OnQueryControllerConfig");
      switch (m_controller_config)
      {
      case NORMAL_DIRECTIONAL:
        message = "Rotate between viewpoints <br>\
                   with the ARROW KEYS";
          break;
      case NORMAL_POSITIONAL:
        message = "Rotate between viewpoints <br>\
                   with LEFT, DOWN and RIGHT";
          break;
      case SOUTHPAW_DIRECTIONAL:
        message = "Rotate between viewpoints with WASD";
          break;
      case SOUTHPAW_POSITIONAL:
        message = "Rotate between viewpoints <br>\
                   with A, S and D";
          break;
      }
      break;
    case POST_ANCHOR_DROPPING:
      message = "...but only if there's space to do so";
      break;
    case WALK_BEHIND:
      message = "You can walk behind BOUND walls...";
      time = 15.f;
      break;
    case ANCHOR_CLEARING:
      message = "Release BINDS with SHIFT";
        break;
    case ANCHOR_SETTING:
      message = "Start BINDING with SHIFT...";
        break;
    case ANCHOR_COMMITTING:
      message = "...and set the BINDS <br>\
                 with SHIFT again";
        break;
    case HUB_RETURN:
      message = "Use BACKSPACE to return <br>\
                 to the HUB at any time";
        break;


    case MESSAGE_OVERRIDE:
      message = " ";
      break;
    case OPENING:
      message =  "\"The Flat Princess\" <br>\
                 <br>\
                 We begin when the Princess has just lost her third dimension.<br>\
                 The Princess has been struck by a terrible predicament. <br>\
                 She is used to seeing and living in three dimensions, <br>\
                 but it seems she has been rendered two-dimensional.<br>\
                 <br>\
                 She mustn't get stuck on this fact though. She gets up and decides <br>\
                 to explore the strange place she is in.";

      break;
    case COINS:
      message =  "These tokens... They seem to give the Princess a <br>\
                  little peace. She would like to collect them.";
      time = 5.f;
      break;
    case COIN_COMPLETE:
      message =  "There seem to be no more \"peaces\" here, but that block below <br>\
                  seems to have lit up. <br>\
                  <br>\
                  The Princess goes to investigate.";
      time = 5.f;
      break;
    case UNLOCK_ROTATION_BLOCKS:
      message =  "What is this? The Princess suddenly feels a little more freedom. <br>\
                  And that block up above seems to have been lit.";
      time = 5.f;
      break;
    case FIRST_ROTATION:
      message =  "It seems these blocks have the magic to allow the Princess to <br>\
                  turn 90 degrees! While she is sad to still be two dimensional, <br>\
                  she is glad that the world is not. <br>\
                  <br>\
                  Perhaps these blocks are the key to recovering her third dimension!";
      time = 7.f;
      break;
    case UNLOCK_FREE_ROTATION:
      message =  "The Princess feels even more freed, and it <br>\
                  seems she can now turn whenever she wants!";
      time = 5.f;
      break;
    case LEVEL_ONE:
      message =  "The Princess wonders what this place is. <br>\
                 <br>\
                  It seems there are more peaces to collect here...";
      time = 5.f;
      break;
    case UNLOCK_ANCHOR_BLOCKS:
      message =  "The Princess feels she has unlocked some new power. <br>\
                  It feels slightly foreign to her. <br>\
                  Also, more blocks seem to have lit up.";
      time = 7.f;
      break;
    case FIRST_ANCHOR:
      message =  "The Princess ponders on what is happening... <br>\
                  <br>\
                  She seems to exist in every plane between the two blocks, <br>\
                  or more like the world parallel to her is bound together. <br>\
                  <br>\
                  Curious...";
      time = 7.f;
      break;
    case LEVEL_TWO:
       message =  "The princess discovers another strange place. It seems <br>\
                   there are binding blocks here and more peaces.";
       time = 5.f;
      break;
    case UNLOCK_FREE_ANCHORING:
       message =  "Once more, the princess feels a surge of power.  <br>\
                   <br>\
                   She senses that she no longer needs the binding blocks <br>\
                   to bind the world...";
       time = 5.f;
      break;
    case FIRST_FREE_ANCHOR:
       message =  "The princess feels the world pull together around her.  <br>\
                   More than that, though, she feels her memories doing the same...";
       break;
       time = 5.f;
    case FINAL_LEVEL:
       message =  "Another world to explore.  <br>\
                   <br>\
                   But something is different...  This world feels...damaged.   <br>\
                   As though it's been pulled apart.";
       break;
       time = 5.f;
    case END_GAME:
       message =  "The Princess stepped onto the star block once more.  How does<br>\
                  her story end?  Join us next time to find out.<br>\
                  <br>\
                  Until then, you can go back to the Hub and try for any secrets<br>\
                  you might have missed!";
      break;
    }
    g_engine_core->DispatchEvent("DisplayHudItem","NarrationPrompt");
    g_engine_core->DispatchEvent("UpdateHudText", "NarrationPrompt", message.GetCString());
    m_narrating = true;
    m_narration_message = message;
    m_narrate_count = time;
    m_prompt_continue = false;
    m_skip_text = false;
    if(freeze)
    {
      g_engine_core->DispatchEvent("OnPlayerDeathToggle", true);
      m_prompt_continue = true;
    }
  }
}

void TileComponent::AnimatePlayer( float dt )
{
  m_facing = static_cast<PLAYERFACING>(g_player_facing);
  Vector3 velocity = GetParent()->GetComponent<DynamicComponent>()->GetVelocity();
  bool moving = false;
  int frameIndex = 1;
  switch(m_axis)
  {
    // Side
  case 0:
    if(abs(velocity.m_z) > 1.f)
    {
      m_facing = velocity.m_z > 0 ? DOWN : UP;
      moving = true;
    }
    break;

    // Top
  case 1:
    if(abs(velocity.m_x) > 1.f)
    {
      m_facing = velocity.m_x > 0 ? RIGHT : LEFT;
      moving = true;
    }
    else if(abs(velocity.m_z) > 1.f)
    {
      m_facing = velocity.m_z > 0 ? DOWN : UP;
      moving = true;
    }
    break;

    // Front
  case 2:
    if(abs(velocity.m_x) > 1.f)
    {
      m_facing = velocity.m_x > 0 ? RIGHT : LEFT;
      moving = true;
    }
    break;

  }

  if(!m_is_grounded)
  {
    if(m_facing == UP)
    {
      if(velocity.m_y < 0)
      {
        GetParent()->Invoke("SetAnimation", 6u);
        GetParent()->Invoke("SetFrame", 0u);
      }
      else
      {
        GetParent()->Invoke("SetAnimation", 2u);
        GetParent()->Invoke("SetFrame", 1u);
      }
    }
    else if(m_facing == DOWN)
    {
      if(velocity.m_y < 0)
      {
        GetParent()->Invoke("SetAnimation", 7u);
        GetParent()->Invoke("SetFrame", 0u);
      }
      else
      {
        GetParent()->Invoke("SetAnimation", 3u);
        GetParent()->Invoke("SetFrame", 1u);
      }
    }
    else if(m_facing == RIGHT)
    {
      if(velocity.m_y < 0)
      {
        GetParent()->Invoke("SetAnimation", 4u);
        GetParent()->Invoke("SetFrame", 0u);
      }
      else
      {
        GetParent()->Invoke("SetAnimation", 0u);
        GetParent()->Invoke("SetFrame", 1u);
      }
    }
    else
    {
      if(velocity.m_y < 0)
      {
        GetParent()->Invoke("SetAnimation", 5u);
        GetParent()->Invoke("SetFrame", 0u);
      }
      else
      {
        GetParent()->Invoke("SetAnimation", 1u);
        GetParent()->Invoke("SetFrame", 1u);
      }
    }
  }
  else
  {
    switch(m_facing)
    {
    case RIGHT:
      if(!m_levitate)
        GetParent()->Invoke("SetAnimation", 0u);
      else
        GetParent()->Invoke("SetAnimation", 4u);
      break;
    case LEFT:
      if(!m_levitate)
        GetParent()->Invoke("SetAnimation", 1u);
      else
        GetParent()->Invoke("SetAnimation", 5u);
      break;
    case UP:
      if(!m_levitate)
        GetParent()->Invoke("SetAnimation", 2u);
      else
        GetParent()->Invoke("SetAnimation", 6u);
      break;
    case DOWN:
      if(!m_levitate)
        GetParent()->Invoke("SetAnimation", 3u);
      else
        GetParent()->Invoke("SetAnimation", 7u);
      break;
    }
  }

  if(!moving)
  {
    m_player_animation_count = 0;
    GetParent()->Invoke("SetAnimationFrame", 1u);
  }
  else
  {
    m_player_animation_count += dt;

    if(m_player_animation_count > m_frame_delay)
    {
      m_player_animation_count = 0;
      GetParent()->Invoke("IncrementFrame");
    }
  }
  g_player_facing = static_cast<int>(m_facing);
}
void TileComponent::AnimateTiles( float dt )
{
  if(m_constant_animation_count > m_frame_delay)
  {
    m_constant_animation_count = 0.f;
    Tilemap *special_tilemap = g_engine_core->SystemCall("TileSystem", "GetTilemaps", "Special");
    special_tilemap->AnimateWarpTiles();
  }
  else
    m_constant_animation_count += dt;
}

void TileComponent::ReturnFromSpace()
{
  m_left_space = false;
  Tilemap *special_tilemap = g_engine_core->SystemCall("TileSystem", "GetTilemaps", "Special");
  m_anchor_axis = -1;
  m_anchor_begin = -1;
  m_anchor_end = -1;
  g_engine_core->SystemCall("TileSystem", "SetAnchorAxis", m_anchor_axis);
  g_engine_core->SystemCall("TileSystem", "SetAnchorBegin", m_anchor_begin);
  g_engine_core->SystemCall("TileSystem", "SetAnchorEnd", m_anchor_end);
  if(g_anchor_blocks && !g_anchor_ability)
  {
    special_tilemap->ToggleAnchorTilesActive(false, 0);
    special_tilemap->ToggleAnchorTilesActive(false, 2);
  }
  m_silent_anchor = true;
  g_engine_core->DispatchEvent("OnAnchorEvent", 2);
  m_silent_anchor = false;

  if(g_curr_coin_count >= 10 && !g_anchor_blocks && g_rotate_ability)
    special_tilemap->SetAnchorBlocksBlockActive();
  else if(g_curr_coin_count >= 10 && !g_anchor_ability && g_anchor_blocks)
    special_tilemap->SetAnchorAbilityBlockActive();

  int coins = g_curr_coin_count;
  //g_engine_core->DispatchEvent("OnCreateCoins", g_coins_needed);
  g_engine_core->DispatchEvent("OnUpdateCoinNumber", coins);
  g_engine_core->DispatchEvent("PushAudioEvent", "Play_level_music");
}

void TileComponent::OnAnchorEvent(int event)
{
  if(m_silent_anchor)
    return;
  switch(event)
  {
  case 0:
    //g_engine_core->DispatchEvent("PushAudioEvent", "Stop_anchor_release");
    //g_engine_core->DispatchEvent("PushAudioEvent", "Stop_anchor_commit");
    g_engine_core->DispatchEvent("PushAudioEvent", "Play_anchor_set");
    if(g_anchor_ability)
      g_anchor_tried = true;
    break;
  case 1:
    //g_engine_core->DispatchEvent("PushAudioEvent", "Stop_anchor_release");
    g_engine_core->DispatchEvent("PushAudioEvent", "Play_anchor_release");
    //g_engine_core->DispatchEvent("PushAudioEvent", "Stop_anchor_set");
    if(g_anchor_ability)
      g_anchor_set = true;
    break;
  case 2:
    UpdateMemory(0);
    UpdateMemory(1);
    UpdateMemory(2);
    g_engine_core->DispatchEvent("PushAudioEvent", "Play_anchor_commit");
    //g_engine_core->DispatchEvent("PushAudioEvent", "Stop_anchor_commit");
    //g_engine_core->DispatchEvent("PushAudioEvent", "Stop_anchor_set");
    break;
  }
}

void TileComponent::SpaceWrapup()
{
  Tilemap *special_tilemap = g_engine_core->SystemCall("TileSystem", "GetTilemaps", "Special");
  m_anchor_axis = -1;
  m_anchor_begin = -1;
  m_anchor_end = -1;
  g_engine_core->SystemCall("TileSystem", "SetAnchorAxis", m_anchor_axis);
  g_engine_core->SystemCall("TileSystem", "SetAnchorBegin", m_anchor_begin);
  g_engine_core->SystemCall("TileSystem", "SetAnchorEnd", m_anchor_end);
  g_warped_back = false;
  special_tilemap->ToggleAnchorTilesActive(false, 0);
  special_tilemap->ToggleAnchorTilesActive(false, 2);
  special_tilemap->ToggleAnchorTilesActive(false, 0);
  special_tilemap->ToggleAnchorTilesActive(false, 2);
  m_silent_anchor = true;
  g_engine_core->DispatchEvent("OnAnchorEvent", 2);
  m_silent_anchor = false;
  g_engine_core->DispatchEvent("PushAudioEvent", "Stop_audio");
  g_engine_core->DispatchEvent("PushAudioEvent", "Play_level_change");
  if(g_curr_coin_count == 10)
    g_engine_core->DispatchEvent("PushAudioEvent", "Play_coin_complete");

  g_player_facing = static_cast<int>(m_facing);
  //g_engine_core->PopSpace();
  m_warping = true;
  m_deadCount = 0.f;
  //g_engine_core->DispatchEvent("PushAudioEvent", "Stop_Audio");
  //g_engine_core->DispatchEvent("PushAudioEvent", "Play_level_change");
  g_engine_core->DispatchEvent("OnPlayerDeathToggle", true);
  m_next_level = 1;

  special_tilemap->RepairMap(0);

}

void TileComponent::OnHUDReady()
{
  m_hud_ready = true;
  g_engine_core->DispatchEvent("OnCreateCoins", g_coins_needed);
  g_engine_core->DispatchEvent("OnUpdateCoinNumber", g_curr_coin_count);
}

void TileComponent::OnRecieveControllerConfig( unsigned config )
{
  //m_controller_config = CONTROLCONFIG(config);
  m_controller_config = NORMAL_DIRECTIONAL;
}

void TileComponent::GetTileComponent()
{
  g_engine_core->DispatchEvent("ReturnTC", this);
}

void TileComponent::CoinCollected(TilePos3 position)
{
  Tilemap *special_tilemap = g_engine_core->SystemCall("TileSystem", "GetTilemaps", "Special");
  UnlockTiles(special_tilemap,position, true);
}

void TileComponent::UnlockTiles(Tilemap* collectMap, TilePos3 pos, bool loadUnlock/*=false*/)
{
  TileGameplayComponent* specialTile = collectMap->GetTileObject(pos.m_x, pos.m_y, pos.m_z);
  if(specialTile->GetSibling<SpriteComponent>()->GetCurrentFrame() != 1)
  {
    if(!g_coin_get && loadUnlock == false)
    {
      g_coin_get = true;
      HUDNarrativePrompt(COINS, 5.f);
    }
    g_curr_coin_count++;
    g_coin_count++;
    g_engine_core->DispatchEvent("OnUpdateCoinNumber", g_curr_coin_count);
    collectMap->SetTile(Vector4(float(m_tile_pos.m_x), float(m_tile_pos.m_y), float(m_tile_pos.m_z), 0.0f), 0);
    if(loadUnlock == false)
      g_engine_core->DispatchEvent("OnCoinCollection", m_tile_pos);
    specialTile->GetSibling<SpriteComponent>()->SetFrame(1);
    if(loadUnlock == false)
      g_engine_core->DispatchEvent("PushAudioEvent", "Play_coin_get");
  }

  int level = g_engine_core->SystemCall("TileSystem", "GetLevelNumber");
  switch(level)
  {
  case 1:
    if(g_curr_coin_count == 5 && !g_rotate_blocks)
    {
      if(!g_coins_complete && loadUnlock == false)
      {
        g_coins_complete = true;
        HUDNarrativePrompt(COIN_COMPLETE, 5.f);
      }
      if(loadUnlock == false)
        g_engine_core->DispatchEvent("PushAudioEvent", "Play_coin_complete");

      collectMap->SetRotationBlocksBlockActive();
    }
    if(g_curr_coin_count == 5 && !g_rotate_ability && g_rotate_blocks)
    {
      if(loadUnlock == false)
        g_engine_core->DispatchEvent("PushAudioEvent", "Play_coin_complete");

      collectMap->SetRotationAbilityBlockActive();
    }
    break;
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
    if(g_curr_coin_count == 5)
    {
      //g_engine_core->DispatchEvent("PushAudioEvent", "Play_Coin_Complete");
      SpaceWrapup();
    }
    else if(g_curr_coin_count == 10)
    {
      //g_engine_core->DispatchEvent("PushAudioEvent", "Play_Coin_Complete");
      SpaceWrapup();
    }
    break;
  }
}

void TileComponent::SetPosition(TilePos3 position)
{
  GetParent()->SetPosition(Vector3(position.m_x, position.m_y, position.m_z));
}

}