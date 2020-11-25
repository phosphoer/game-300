// David Evans
// 2011/07/21
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Tilemap.h"
#include "TileGameplayComponent.h"
#include "../Core/EngineCore.h"
#include "../Composition/Composition.h"
#include "../Graphics/SpriteComponent.h"
#include "../containers/string.h"
#include "../FileIO/Serializer.h"

namespace Improbability
{

MakeMetaBasic(TilePos2);
MakeMetaBasic(TilePos3);
MakeMeta(Tilemap);

void Tilemap::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
}

Tilemap::Tilemap():
  m_next(0),
  m_grid(0),
  m_origGrid(0),
  m_width(0),
  m_height(0),
  m_depth(0),
  m_tile_list(std::list<TileGameplayComponent*>()),
  m_tile_objects(HashMap<String, TileGameplayComponent*>())
{
}
Tilemap::Tilemap(Tilemap& rhs):
  m_next(0),
  m_grid(0),
  m_origGrid(0),
  m_width(0),
  m_height(0),
  m_depth(0),
  m_tile_list(std::list<TileGameplayComponent*>()),
  m_tile_objects(HashMap<String, TileGameplayComponent*>())
{
}
  
Tilemap::~Tilemap()
{}

void Tilemap::Load(char const *filepath)
{
  std::FILE *file = std::fopen(filepath, "rt");

  // Get width, height, depth
  std::fscanf(file, "%i\n%i\n%i\n", &m_width, &m_height, &m_depth);

  // Validate sizes
  ErrorIf(m_width <= 0 || m_height <= 0 || m_depth <= 0, 
          "Tilemap::Load(): Invalid size of %i, %i, %i", 
          m_width, m_height, m_depth);
  
  // Allocate the size
  Allocate(m_width, m_height, m_depth);

  // Get tilemap data
  for (int z = 0; z < m_depth; ++z)
  {
    for (int y = m_height - 1; y >= 0; --y)
    {
      for (int x = 0; x < m_width; ++x)
      {
        int tile = 0;
        std::fscanf(file, "%i", &tile);

        if(IsOutside(x, y, z))
          continue;

        m_origGrid[x + y * m_width + z * m_width * m_height] = static_cast<Tile>(tile);
      }
    }
  }
  std::fclose(file);
}

void Tilemap::ResetTilemap()
{
  for (int z = 0; z < m_depth; ++z)
  {
    for (int y = m_height - 1; y >= 0; --y)
    {
      for (int x = 0; x < m_width; ++x)
        m_grid[x + y * m_width + z * m_width * m_height] = m_origGrid[x + y * m_width + z * m_width * m_height];
    }
  }
}

void Tilemap::Unload()
{
  if (!m_grid)
    return;

  delete[] m_grid;

  if(m_origGrid)
    delete[] m_origGrid;

  m_origGrid = m_grid = 0;
}

void Tilemap::SetTile(int x, int y, int z, Tile value)
{
  if (IsOutside(x, y, z))
    return;

  m_grid[x + y * m_width + z * m_width * m_height] = value;
}

void Tilemap::SetTile(Vector4 pos, Tile value)
{
  TilePos3 tpos = GetTilePos(pos);
  SetTile(tpos.m_x, tpos.m_y, tpos.m_z, value);
}

Tile Tilemap::GetTile(int x, int y, int z)
{
  if (IsOutside(x, y, z))
    return 0;

  return m_grid[x + y * m_width + z * m_width * m_height];
}

Tile Tilemap::GetTile(TilePos2 pos, int axis, int layer)
{
  // If either coordinate is negative they are outside the map
  if (pos.m_x < 0 || pos.m_y < 0 || layer < 0)
    return 0;

  if (axis == 0)
  {
    if (pos.m_x >= m_depth || pos.m_y >= m_height || layer >= m_width)
      return 0;
    return GetTile(layer, pos.m_y, pos.m_x);
  }
  else if (axis == 1)
  {
    if (pos.m_x >= m_width || pos.m_y >= m_depth || layer >= m_height)
      return 0;
    return GetTile(pos.m_x, layer, pos.m_y);
  }
  else if (axis == 2)
  {
    if (pos.m_x >= m_width || pos.m_y >= m_height || layer >= m_depth)
      return 0;
    return GetTile(pos.m_x, pos.m_y, layer);
  }

  ErrorIf(1, "Tilemap::GetTilePos: Invalid axis of %i specified", axis);
  return 1;
}

Tile Tilemap::GetTile(TilePos3 pos)
{
  return GetTile(pos.m_x, pos.m_y, pos.m_z);
}

Tile Tilemap::GetTile(Vector4 pos)
{
  TilePos3 tpos = GetTilePos(pos);

  return GetTile(tpos.m_x, tpos.m_y, tpos.m_z);
}

bool Tilemap::IsOutside(int x, int y, int z)
{
  if (x < 0 || x >= m_width) return true;
  if (y < 0 || y >= m_height) return true;
  if (z < 0 || z >= m_depth) return true;

  return false;
}

TilePos2 Tilemap::GetTilePos(Vector4 pos, int axis)
{
  if (axis == 0)
    return TilePos2(std::floor(pos.m_z), std::floor(pos.m_y));
  else if (axis == 1)
    return TilePos2(std::floor(pos.m_x), std::floor(pos.m_z));
  else if (axis == 2)
    return TilePos2(std::floor(pos.m_x), std::floor(pos.m_y));

  ErrorIf(1, "Tilemap::GetTilePos: Invalid axis of %i specified", axis);
  return TilePos2(-1, -1);
}

TilePos3 Tilemap::GetTilePos(Vector4 pos)
{
  return TilePos3(std::floor(pos.m_x), std::floor(pos.m_y), std::floor(pos.m_z));
}

void Tilemap::Allocate(int x, int y, int z)
{
  m_grid = new Tile[x * y * z];
  m_origGrid = new Tile[x*y*z];
}

// Generates a HashMap of objects of a given type
void Tilemap::BuildObjectMap(int mapType)
{
  switch(mapType)
  {
    //Special Tilemap
  case 0:

    m_tile_objects.Clear();
    m_tile_list.clear();

    for(int x = 0; x < m_width; x++)
    for(int y = 0; y < m_height; y++)
    for(int z = 0; z < m_depth; z++)
    {
      int type = GetTile(x, y, z);
      Composition* newTile;
      int current;
      switch(type)
      {
        // no tile
      case 0:
        break;
        // side rotation tile
      case 2:
        newTile = g_engine_core->SystemCall("ObjectManager", "CreateObject", "SideRotationTile.bp");
        m_tile_objects.Insert(String::Formatted("%i, %i, %i", x, y+1, z), static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        current = GetTile(x, y, z);
        SetTile(x, y, z, 0);
        SetTile(x, y+1, z, -current);
        m_tile_list.push_back(static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        newTile->SetPosition(Vector3(float(x)+.5f, float(y)+.5f, float(z)+.5f));
        break;
        // top rotation tile
      case 3:
        newTile = g_engine_core->SystemCall("ObjectManager", "CreateObject", "TopRotationTile.bp");
        m_tile_objects.Insert(String::Formatted("%i, %i, %i", x, y+1, z), static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        current = GetTile(x, y, z);
        SetTile(x, y, z, 0);
        SetTile(x, y+1, z, -current);
        m_tile_list.push_back(static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        newTile->SetPosition(Vector3(float(x)+.5f, float(y)+.5f, float(z)+.5f));
        break;
        // front rotation tile
      case 4:
        newTile = g_engine_core->SystemCall("ObjectManager", "CreateObject", "FrontRotationTile.bp");
        m_tile_objects.Insert(String::Formatted("%i, %i, %i", x, y+1, z), static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        current = GetTile(x, y, z);
        SetTile(x, y, z, 0);
        SetTile(x, y+1, z, -current);
        m_tile_list.push_back(static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        newTile->SetPosition(Vector3(float(x)+.5f, float(y)+.5f, float(z)+.5f));
        break;
        // anchor tile Front
      case 5:
        newTile = g_engine_core->SystemCall("ObjectManager", "CreateObject", "AnchorFrontTile.bp");
        m_tile_objects.Insert(String::Formatted("%i, %i, %i", x, y+1, z), static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        current = GetTile(x, y, z);
        SetTile(x, y, z, 0);
        SetTile(x, y+1, z, -current);
        m_tile_list.push_back(static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        newTile->SetPosition(Vector3(float(x)+.5f, float(y)+.5f, float(z)+.5f));
        break;
        // anchor tile Side
      case 6:
        newTile = g_engine_core->SystemCall("ObjectManager", "CreateObject", "AnchorSideTile.bp");
        m_tile_objects.Insert(String::Formatted("%i, %i, %i", x, y+1, z), static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        current = GetTile(x, y, z);
        SetTile(x, y, z, 0);
        SetTile(x, y+1, z, -current);
        m_tile_list.push_back(static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        newTile->SetPosition(Vector3(float(x)+.5f, float(y)+.5f, float(z)+.5f));
        break;
        // Collectible
      case 7:
        newTile = g_engine_core->SystemCall("ObjectManager", "CreateObject", "CollectibleTile.bp");
        m_tile_objects.Insert(String::Formatted("%i, %i, %i", x, y, z), static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        m_tile_list.push_back(static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        newTile->SetPosition(Vector3(float(x)+.5f, float(y)+.5f, float(z)+.5f));
        break;
        // Artifact
      case 8:
        newTile = g_engine_core->SystemCall("ObjectManager", "CreateObject", "ArtifactTile.bp");
        m_tile_objects.Insert(String::Formatted("%i, %i, %i", x, y, z), static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        m_tile_list.push_back(static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        newTile->SetPosition(Vector3(float(x)+.5f, float(y)+.5f, float(z)+.5f));
        break;

        // Level Warp Blocks
      case 1:
        newTile = g_engine_core->SystemCall("ObjectManager", "CreateObject", "WarpTile.bp");
        m_tile_objects.Insert(String::Formatted("%i, %i, %i", x, y+1, z), static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent"))->SetLevelNumber(1);
        current = GetTile(x, y, z);
        SetTile(x, y, z, 0);
        SetTile(x, y+1, z, -current);
        m_tile_list.push_back(static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        newTile->SetPosition(Vector3(float(x)+.5f, float(y)+.5f, float(z)+.5f));
        break;
      case 20:
        newTile = g_engine_core->SystemCall("ObjectManager", "CreateObject", "WarpTile.bp");
        m_tile_objects.Insert(String::Formatted("%i, %i, %i", x, y+1, z), static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent"))->SetLevelNumber(20);
        current = GetTile(x, y, z);
        SetTile(x, y, z, 0);
        SetTile(x, y+1, z, -current);
        m_tile_list.push_back(static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        newTile->SetPosition(Vector3(float(x)+.5f, float(y)+.5f, float(z)+.5f));
        break;
      case 21:
        newTile = g_engine_core->SystemCall("ObjectManager", "CreateObject", "WarpTile.bp");
        m_tile_objects.Insert(String::Formatted("%i, %i, %i", x, y+1, z), static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent"))->SetLevelNumber(21);
        current = GetTile(x, y, z);
        SetTile(x, y, z, 0);
        SetTile(x, y+1, z, -current);
        m_tile_list.push_back(static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        newTile->SetPosition(Vector3(float(x)+.5f, float(y)+.5f, float(z)+.5f));
        break;
      case 22:
        newTile = g_engine_core->SystemCall("ObjectManager", "CreateObject", "WarpTile.bp");
        m_tile_objects.Insert(String::Formatted("%i, %i, %i", x, y+1, z), static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent"))->SetLevelNumber(22);
        current = GetTile(x, y, z);
        SetTile(x, y, z, 0);
        SetTile(x, y+1, z, -current);
        m_tile_list.push_back(static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        newTile->SetPosition(Vector3(float(x)+.5f, float(y)+.5f, float(z)+.5f));
        break;
      case 23:
        newTile = g_engine_core->SystemCall("ObjectManager", "CreateObject", "WarpTile.bp");
        m_tile_objects.Insert(String::Formatted("%i, %i, %i", x, y+1, z), static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent"))->SetLevelNumber(23);
        current = GetTile(x, y, z);
        SetTile(x, y, z, 0);
        SetTile(x, y+1, z, -current);
        m_tile_list.push_back(static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        newTile->SetPosition(Vector3(float(x)+.5f, float(y)+.5f, float(z)+.5f));
        break;
      case 24:
        newTile = g_engine_core->SystemCall("ObjectManager", "CreateObject", "WarpTile.bp");
        m_tile_objects.Insert(String::Formatted("%i, %i, %i", x, y+1, z), static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent"))->SetLevelNumber(24);
        current = GetTile(x, y, z);
        SetTile(x, y, z, 0);
        SetTile(x, y+1, z, -current);
        m_tile_list.push_back(static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        newTile->SetPosition(Vector3(float(x)+.5f, float(y)+.5f, float(z)+.5f));
        break;
      case 25:
        newTile = g_engine_core->SystemCall("ObjectManager", "CreateObject", "WarpTile.bp");
        m_tile_objects.Insert(String::Formatted("%i, %i, %i", x, y+1, z), static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent"))->SetLevelNumber(25);
        current = GetTile(x, y, z);
        SetTile(x, y, z, 0);
        SetTile(x, y+1, z, -current);
        m_tile_list.push_back(static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        newTile->SetPosition(Vector3(float(x)+.5f, float(y)+.5f, float(z)+.5f));
        break;

 /*       elif(tile.LevelOneScript != None):
        return '20';
        elif(tile.LevelTwoScript != None):
        return '21';
        elif(tile.LevelThreeScript != None):
        return '22';
        elif(tile.EndWarpScript != None):
        return '23';
        elif(tile.LevelFourScript != None):
        return '24';
        elif(tile.LevelFiveScript != None):
        return '25';*/

        // Coin Bank Cubes
        //Enable Rotation Blocks
      case 50:
        newTile = g_engine_core->SystemCall("ObjectManager", "CreateObject", "EnableRotationBlocksTile.bp");
        m_tile_objects.Insert(String::Formatted("%i, %i, %i", x, y+1, z), static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        current = GetTile(x, y, z);
        SetTile(x, y, z, 0);
        SetTile(x, y+1, z, -current);
        m_tile_list.push_back(static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        newTile->SetPosition(Vector3(float(x)+.5f, float(y)+.5f, float(z)+.5f));
        break;
        // Enable Rotation Ability
      case 51:
        newTile = g_engine_core->SystemCall("ObjectManager", "CreateObject", "EnableRotationAbilityTile.bp");
        m_tile_objects.Insert(String::Formatted("%i, %i, %i", x, y+1, z), static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        current = GetTile(x, y, z);
        SetTile(x, y, z, 0);
        SetTile(x, y+1, z, -current);
        m_tile_list.push_back(static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        newTile->SetPosition(Vector3(float(x)+.5f, float(y)+.5f, float(z)+.5f));
        break;
        // Enable Anchor Blocks
      case 52:
        newTile = g_engine_core->SystemCall("ObjectManager", "CreateObject", "EnableAnchorBlocksTile.bp");
        m_tile_objects.Insert(String::Formatted("%i, %i, %i", x, y+1, z), static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        current = GetTile(x, y, z);
        SetTile(x, y, z, 0);
        SetTile(x, y+1, z, -current);
        m_tile_list.push_back(static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        newTile->SetPosition(Vector3(float(x)+.5f, float(y)+.5f, float(z)+.5f));
        break;
        // Enable Anchor
      case 53:
        newTile = g_engine_core->SystemCall("ObjectManager", "CreateObject", "EnableAnchorAbilityTile.bp");
        m_tile_objects.Insert(String::Formatted("%i, %i, %i", x, y+1, z), static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        current = GetTile(x, y, z);
        SetTile(x, y, z, 0);
        SetTile(x, y+1, z, -current);
        m_tile_list.push_back(static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
        newTile->SetPosition(Vector3(float(x)+.5f, float(y)+.5f, float(z)+.5f));
        break;
        // Warp Tile
      //case 55:
      //  newTile = g_engine_core->SystemCall("ObjectManager", "CreateObject", "WarpTile.bp");
      //  m_tile_objects.Insert(String::Formatted("%i, %i, %i", x, y+1, z), static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
      //  current = GetTile(x, y, z);
      //  SetTile(x, y, z, 0);
      //  SetTile(x, y+1, z, -current);
      //  m_tile_list.push_back(static_cast<TileGameplayComponent*>(newTile->GetComponent("TileGameplayComponent")));
      //  newTile->SetPosition(Vector3(float(x)+.5f, float(y)+.5f, float(z)+.5f));
      //  break;
      }
    }
    break;
  }

  for(int x = 0; x < m_width; x++)
  for(int y = 0; y < m_height; y++)
  for(int z = 0; z < m_depth; z++)
  {
    int type = GetTile(x, y, z);
    if(type < 0)
      SetTile(x,y,z, type * -1);

  }
}

void Tilemap::RepairMap( int mapType )
{
  switch(mapType)
  {
    //Special Tilemap
  case 0:
    m_tile_objects.Clear();
    m_tile_list.clear();
    for(int x = 0; x < m_width; x++)
    for(int y = 0; y < m_height; y++)
    for(int z = 0; z < m_depth; z++)
    {
      int type = GetTile(x, y, z);
      /*Composition* newTile;
      int current;*/
      switch(type)
      {
      case 2:
          SetTile(x, y, z, 0);
          SetTile(x, y-1, z, -type);
          break;
        case 3:
          SetTile(x, y, z, 0);
          SetTile(x, y-1, z, -type);
          break;
        case 4:
          SetTile(x, y, z, 0);
          SetTile(x, y-1, z, -type);
          break;
        case 5:
          SetTile(x, y, z, 0);
          SetTile(x, y-1, z, -type);
          break;
        case 6:
          SetTile(x, y, z, 0);
          SetTile(x, y-1, z, -type);
          break;
        case 1:
          SetTile(x, y, z, 0);
          SetTile(x, y-1, z, -type);
          break;
        case 20:
          SetTile(x, y, z, 0);
          SetTile(x, y-1, z, -type);
          break;
        case 21:
          SetTile(x, y, z, 0);
          SetTile(x, y-1, z, -type);
          break;
        case 22:
          SetTile(x, y, z, 0);
          SetTile(x, y-1, z, -type);
          break;
        case 23:
          SetTile(x, y, z, 0);
          SetTile(x, y-1, z, -type);
          break;
        case 24:
          SetTile(x, y, z, 0);
          SetTile(x, y-1, z, -type);
          break;
        case 25:
          SetTile(x, y, z, 0);
          SetTile(x, y-1, z, -type);
          break;
        case 50:
          SetTile(x, y, z, 0);
          SetTile(x, y-1, z, -type);
          break;
        case 51:
          SetTile(x, y, z, 0);
          SetTile(x, y-1, z, -type);
          break;
        case 52:
          SetTile(x, y, z, 0);
          SetTile(x, y-1, z, -type);
          break;
        case 53:
          SetTile(x, y, z, 0);
          SetTile(x, y-1, z, -type);
          break;
        case 54:
          SetTile(x, y, z, 0);
          SetTile(x, y-1, z, -type);
          break;
      }
    }
    for(int x = 0; x < m_width; x++)
    for(int y = 0; y < m_height; y++)
    for(int z = 0; z < m_depth; z++)
    {
      int type = GetTile(x, y, z);
      if(type < 0)
        SetTile(x,y,z, type * -1);

    }

    break;
  }

}


TileGameplayComponent* Tilemap::GetTileObject( int x, int y, int z )
{
  return m_tile_objects[String::Formatted("%i, %i, %i", x, y, z)];
}

void Tilemap::ToggleRotationTiles( bool toggle )
{
  for(std::list<TileGameplayComponent*>::iterator i = m_tile_list.begin(); i != m_tile_list.end(); ++i)
  {
    SpriteComponent* sprite = (*i)->GetSibling<SpriteComponent>();
    if(sprite && (*i)->GetTileType() >= 2 && (*i)->GetTileType() <= 4)
    {
      (*i)->SetRotationActive(toggle);
      if(toggle)
      {
        int prev = (*i)->GetPreviousAxis() == -1 ? (*i)->GetDefaultAxis() : (*i)->GetPreviousAxis();
        sprite->SetFrame((((*i)->GetDefaultAxis() - prev + 3) % 3) * 2);
      }
      else
      {
        sprite->SetFrame(6);
      }
    }
  }
}
void Tilemap::ToggleAnchorTiles( bool toggle )
{
  for(std::list<TileGameplayComponent*>::iterator i = m_tile_list.begin(); i != m_tile_list.end(); ++i)
  {
    SpriteComponent* sprite = (*i)->GetSibling<SpriteComponent>();
    if(sprite && (*i)->GetTileType() >= 5 && (*i)->GetTileType() <= 6)
    {
      if(toggle)
      {
        sprite->SetFrame(0);
      }
      else
      {
        sprite->SetFrame(2);
      }
    }
  }
}
void Tilemap::ToggleAnchorTilesActive( bool toggle, int anchor_axis )
{
  int tile_type = anchor_axis ? 5 : 6;
  for(std::list<TileGameplayComponent*>::iterator i = m_tile_list.begin(); i != m_tile_list.end(); ++i)
  {
    SpriteComponent* sprite = (*i)->GetSibling<SpriteComponent>();
    if(sprite && (*i)->GetTileType() == tile_type)
    {
      if(toggle)
        sprite->SetFrame(1);
      else
        sprite->SetFrame(0);
    }
  }
}

void Tilemap::SetRotationBlocksBlockActive()
{
  for(std::list<TileGameplayComponent*>::iterator i = m_tile_list.begin(); i != m_tile_list.end(); ++i)
  {
    SpriteComponent* sprite = (*i)->GetSibling<SpriteComponent>();
    if(sprite && (*i)->GetTileType() == 50)
    {
      sprite->SetAnimation(1);
    }
  }
}

void Tilemap::SetRotationAbilityBlockActive()
{
  for(std::list<TileGameplayComponent*>::iterator i = m_tile_list.begin(); i != m_tile_list.end(); ++i)
  {
    SpriteComponent* sprite = (*i)->GetSibling<SpriteComponent>();
    if(sprite && (*i)->GetTileType() == 51)
    {
      sprite->SetAnimation(1);
    }
  }
}

void Tilemap::SetAnchorBlocksBlockActive()
{
  for(std::list<TileGameplayComponent*>::iterator i = m_tile_list.begin(); i != m_tile_list.end(); ++i)
  {
    SpriteComponent* sprite = (*i)->GetSibling<SpriteComponent>();
    if(sprite && (*i)->GetTileType() == 52)
    {
      sprite->SetAnimation(1);
    }
  }
}

void Tilemap::SetAnchorAbilityBlockActive()
{
  for(std::list<TileGameplayComponent*>::iterator i = m_tile_list.begin(); i != m_tile_list.end(); ++i)
  {
    SpriteComponent* sprite = (*i)->GetSibling<SpriteComponent>();
    if(sprite && (*i)->GetTileType() == 53)
    {
      sprite->SetAnimation(1);
    }
  }
}

void Tilemap::SetCollectiblesActive()
{
  for(std::list<TileGameplayComponent*>::iterator i = m_tile_list.begin(); i != m_tile_list.end(); ++i)
  {
    SpriteComponent* sprite = (*i)->GetSibling<SpriteComponent>();
    if(sprite && (*i)->GetTileType() == 7)
    {
      sprite->SetFrame(0);
    }
  }
}

void Tilemap::SetArtifactsActive()
{
  for(std::list<TileGameplayComponent*>::iterator i = m_tile_list.begin(); i != m_tile_list.end(); ++i)
  {
    SpriteComponent* sprite = (*i)->GetSibling<SpriteComponent>();
    if(sprite && (*i)->GetTileType() == 8)
    {
      sprite->SetFrame(0);
    }
  }
}

void Tilemap::AnimateWarpTiles()
{
  for(std::list<TileGameplayComponent*>::iterator i = m_tile_list.begin(); i != m_tile_list.end(); ++i)
  {
    SpriteComponent* sprite = (*i)->GetSibling<SpriteComponent>();
    if(sprite && (*i)->GetTileType() == 55)
    {
      sprite->IncrementFrame();
    }
  }
}


void TilePos3::Deserialize(Serializer &serializer)
{
  Read(serializer, m_x, "X"); Read(serializer, m_y, "Y"); Read(serializer, m_z, "Z");
}

void TilePos3::Serialize(Serializer &serializer)
{
  Write(serializer, m_x, "X"); Write(serializer, m_y, "Y"); Write(serializer, m_z, "Z");
}

void TilePos3::BindMeta(MetaComposition* comp)
{
  comp->MetaAddComponent(MetaNewable);
}

}