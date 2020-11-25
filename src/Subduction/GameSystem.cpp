// David Evans & Jake Leonard
// 2011/09/27
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "GameSystem.h"
#include "../Core/EngineCore.h"
#include "../Input/InputEvent.h"
#include "../FileIO/Serializer.h"
#include "CameraElevate.h"
#include "../Graphics/Camera.h"
#include "../Logic/LogicComponent.h"
#include "../composition/component.h"

namespace Improbability
{

GameSystem::CoinMap GameSystem::sm_coin_locations;
Improbability::TilePos3 GameSystem::sm_player_pos;
bool GameSystem::sm_loading = false;
int GameSystem::sm_player_facing;

MakeMeta(GameSystem);

void GameSystem::BindMeta(MetaComposition *comp)
{
  comp->MetaAddComponent(MetaNewable);
  comp->MetaAddComponent(MetaCallable);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnSpaceReady);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnKeyPress);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnActionPress);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAxisInitalize);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAxisChange);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnAxisChangeComplete);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnPlayerGrounded);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnPlayerNotGrounded);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnRotateToTarget);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnFrontToggle);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnSideToggle);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnTopToggle);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnNewGame);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnGamePauseSave);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnGameSave);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(QueryHasSaves);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnLoadRequested);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(GameReadyToLoad);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(OnCoinCollection);
  comp->MetaGetComponent(MetaCallable)->MetaRegisterFunction(ReturnTC);
}

GameSystem::GameSystem():
  m_start_axis(-1),
  m_end_axis(-1),
  m_prev_axis(-1),
  m_current_axis(0),
  m_target_axis(-1),
  m_rotate_on_next(false),
  m_player_grounded(true),
  m_initiated(false),
  m_top_enabled(false),
  m_side_enabled(false),
  m_front_enabled(false),
  m_rotating(false),
  m_tile_component(NULL)
{
}

// Inherited methods
void GameSystem::InitializeGlobal()
{
}

void GameSystem::InitializeInstance()
{
  // Register listeners
  g_engine_core->AddEventListener(MessageBank::OnSpaceReady, this);
  g_engine_core->AddEventListener(MessageBank::OnKeyPress, this);
  g_engine_core->AddEventListener(MessageBank::OnActionPress, this);
  g_engine_core->AddEventListener(MessageBank::OnAxisInitalize, this);
  g_engine_core->AddEventListener(MessageBank::OnAxisChange, this);
  g_engine_core->AddEventListener(MessageBank::OnAxisChangeComplete, this);
  g_engine_core->AddEventListener(MessageBank::OnPlayerGrounded, this);
  g_engine_core->AddEventListener(MessageBank::OnPlayerNotGrounded, this);
  g_engine_core->AddEventListener(MessageBank::OnRotateToTarget, this);
  g_engine_core->AddEventListener(MessageBank::OnTopToggle, this);
  g_engine_core->AddEventListener(MessageBank::OnFrontToggle, this);
  g_engine_core->AddEventListener(MessageBank::OnSideToggle, this);
  g_engine_core->AddEventListener(MessageBank::OnNewGame, this);
  g_engine_core->AddEventListener(MessageBank::OnGamePauseSave, this);
  g_engine_core->AddEventListener(MessageBank::OnGameSave, this);
  g_engine_core->AddEventListener(MessageBank::QueryHasSaves, this);
  g_engine_core->AddEventListener(MessageBank::OnLoadRequested, this);
  g_engine_core->AddEventListener(MessageBank::OnCoinCollection, this);
  g_engine_core->AddEventListener(MessageBank::GameReadyToLoad, this);
  g_engine_core->AddEventListener(MessageBank::ReturnTC, this);

  m_cursor_count = 0;

  while (ShowCursor(false) >= 0)
  {
    m_cursor_count++;
  }
}

void GameSystem::UpdateInstance(float dt)
{
  if (m_rotate_on_next)
  {
    m_rotate_on_next = false;
    InitiateAxisChange(m_target_axis);
  }
}

void GameSystem::UninitializeInstance()
{
  g_engine_core->RemoveEventListener(MessageBank::OnSpaceReady, this);
  g_engine_core->RemoveEventListener(MessageBank::OnKeyPress, this);
  g_engine_core->RemoveEventListener(MessageBank::OnActionPress, this);
  g_engine_core->RemoveEventListener(MessageBank::OnAxisInitalize, this);
  g_engine_core->RemoveEventListener(MessageBank::OnAxisChange, this);
  g_engine_core->RemoveEventListener(MessageBank::OnAxisChangeComplete, this);
  g_engine_core->RemoveEventListener(MessageBank::OnPlayerGrounded, this);
  g_engine_core->RemoveEventListener(MessageBank::OnPlayerNotGrounded, this);
  g_engine_core->RemoveEventListener(MessageBank::OnRotateToTarget, this);
  g_engine_core->RemoveEventListener(MessageBank::OnTopToggle, this);
  g_engine_core->RemoveEventListener(MessageBank::OnFrontToggle, this);
  g_engine_core->RemoveEventListener(MessageBank::OnSideToggle, this);
  g_engine_core->RemoveEventListener(MessageBank::OnNewGame, this);
  g_engine_core->RemoveEventListener(MessageBank::OnGamePauseSave, this);
  g_engine_core->RemoveEventListener(MessageBank::OnGameSave, this);
  g_engine_core->RemoveEventListener(MessageBank::QueryHasSaves, this);
  g_engine_core->RemoveEventListener(MessageBank::OnLoadRequested, this);
  g_engine_core->RemoveEventListener(MessageBank::OnCoinCollection, this);
  g_engine_core->RemoveEventListener(MessageBank::GameReadyToLoad, this);
  g_engine_core->RemoveEventListener(MessageBank::ReturnTC, this);

  while (m_cursor_count--)
  {
    ShowCursor(true);
  }
}

void GameSystem::PushLoadedData()
{
  String curSpaceName = g_engine_core->GetSpaceName();
  if(g_isLoaded && sm_coin_locations.Find(curSpaceName) != sm_coin_locations.End())
  {
    std::vector<TilePos3>& coins = sm_coin_locations[curSpaceName];
    for(auto it = coins.begin(); it != coins.end(); ++it)
      g_engine_core->DispatchEvent("CoinCollected", *it);
  }
  if(sm_loading == true && m_tile_component != NULL)
  {
    /* TODO: Set Player Position somehow */
    m_tile_component->m_facing = (Improbability::PLAYERFACING)sm_player_facing;
    sm_loading = false;
  }
}

void GameSystem::OnSpaceReady()
{  
  PushLoadedData();
}

void GameSystem::OnGameSave()
{
  if(CanSave() == false)
  {
    g_engine_core->DispatchEvent("SaveSuccessful", false);
    return;
  }

  SaveGame();
  g_engine_core->DispatchEvent("SaveSuccessful", true);
}

String GameSystem::GetSavePath()
{
  String result;
  char buffer[MAX_PATH];
  if(SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, buffer) == S_OK)
  {
    result.Assign(buffer);
    result += "\\Subduction\\";
  }
  return result;
}

void GameSystem::SaveGame()
{
  String SpaceName = g_engine_core->GetSpaceName();
  sm_player_pos = m_tile_component->m_last_safe_pos;
  sm_player_facing = m_tile_component->m_facing;

  Serializer saveFileGo(GetSavePath() + "savefile.xml", "SaveData", true);

  Write(saveFileGo, sm_player_pos, "PlayerPos");
  Write(saveFileGo, SpaceName, "CurSpace");
  Write(saveFileGo, g_curr_level, "CurLevel");
  Write(saveFileGo, g_rotate_blocks, "RotateBlocks");
  Write(saveFileGo, g_rotate_ability, "RotateAbility");
  Write(saveFileGo, g_anchor_blocks, "AnchorBlocks");
  Write(saveFileGo, g_anchor_ability, "AnchorAbility");
  Write(saveFileGo, g_anchor_ability, "AnchorAbility");
  Write(saveFileGo, g_the_end, "TheEnd");
  Write(saveFileGo, g_nega_binding, "NegaBinding");
  Write(saveFileGo, g_player_facing, "Facing");
  Write(saveFileGo, sm_player_facing, "PlayFace");

  /* Write out the coins */
  for(auto it = sm_coin_locations.Begin(); it != sm_coin_locations.End(); ++it)
  {
    String levelName = it->first;
    saveFileGo.CreateElementGroup("LevelData");
    WriteAttribute(saveFileGo, levelName, "level");

    for(auto lvlCoin = it->second.begin(); lvlCoin != it->second.end(); ++lvlCoin)
      Write(saveFileGo, *lvlCoin, "Coin");
    saveFileGo.FinishElementGroup();
  }

  saveFileGo.SaveFile();
  g_engine_core->DispatchEvent("SaveSuccessful");
}

bool GameSystem::HasSave()
{
  String fileLoc = GetSavePath() + "savefile.xml";

  std::ifstream saveFileLoc(fileLoc.GetCString());
  bool exists = saveFileLoc.good();
  saveFileLoc.close();
  return exists;
}

void GameSystem::OnGamePauseSave()
{
  /* TODO: 
    * Allow the pause menu to save
    * Allow main menu to load files
    * Check about the tile component thing to see if it transfers over spaces
  */
}

void GameSystem::GameReadyToLoad()
{
  /* Load file, fill in data */
  String fileLoc = GetSavePath() + "savefile.xml";
  String SpaceName;

  Serializer savefile(fileLoc);
  savefile.EnterElement("SaveData");
  Read(savefile, SpaceName, "CurSpace");
  Read(savefile, sm_player_pos, "PlayerPos");
  Read(savefile, g_curr_level, "CurLevel");
  Read(savefile, g_rotate_blocks, "RotateBlocks");
  Read(savefile, g_rotate_ability, "RotateAbility");
  Read(savefile, g_anchor_blocks, "AnchorBlocks");
  Read(savefile, g_anchor_ability, "AnchorAbility");
  Read(savefile, g_anchor_ability, "AnchorAbility");
  Read(savefile, g_the_end, "TheEnd");
  Read(savefile, g_nega_binding, "NegaBinding");
  Read(savefile, g_player_facing, "Facing");
  Read(savefile, sm_player_facing, "PlayFace");
  
  String getSpaceName;
  while(savefile.EnterElement("LevelData"))
  {
    savefile.ReadStringAttribute(getSpaceName, "level");
    for(savefile.Begin(); savefile.IsGood(); savefile.Iterate())
    {
      TilePos3 newPos;
      Read(savefile, newPos, "Coin");
      sm_coin_locations[getSpaceName].push_back(newPos);
    }

    savefile.LeaveElement();
  }

  savefile.LeaveElement();

  sm_loading = g_isLoaded = true;
  g_engine_core->DispatchEvent("OnPushSpace", SpaceName);
}

void GameSystem::QueryHasSaves()
{
  g_engine_core->DispatchEvent("HasGameSaves", HasSave());
}

void GameSystem::OnLoadRequested()
{
  if(HasSave() == true)
    g_engine_core->DispatchEvent("OnResetSpace", "Tutorial-space.spc", true);
}

bool GameSystem::CanSave()
{
  if(m_tile_component != NULL)
    return !(m_tile_component->m_anchor_enabled);

  return false;
}

void GameSystem::OnCoinCollection(TilePos3 coinPos)
{
  sm_coin_locations[g_engine_core->GetSpaceName()].push_back(coinPos);
}

void GameSystem::OnTopToggle(bool toggle)
{
  m_top_enabled = toggle;
}
void GameSystem::OnSideToggle(bool toggle)
{
  m_side_enabled = toggle;
}
void GameSystem::OnFrontToggle(bool toggle)
{
  m_front_enabled = toggle;
}

void GameSystem::OnAxisInitalize(int initalAxis)
{
  m_current_axis = initalAxis;
}

void GameSystem::OnAxisChange(int old_axis, int axis)
{
  if(!m_initiated)
  {
    m_prev_axis = old_axis;
    m_current_axis = axis;
    m_target_axis = axis;
  }
  m_rotating = true;
}
void GameSystem::OnAxisChangeComplete(int axis)
{
  m_prev_axis = m_current_axis;
  if(m_target_axis != -1 && m_current_axis != m_target_axis)
  {
    m_current_axis = m_target_axis;
    m_rotate_on_next = true;
  }
  else
  {
    int start = m_start_axis > -1 ? m_start_axis : m_prev_axis;
    int end = m_end_axis > -1 ? m_end_axis : m_current_axis;
    g_engine_core->DispatchEvent("OnRotateComplete", start, end);
    m_rotating = false;
  }
  m_initiated = false;
}
void GameSystem::OnPlayerGrounded()
{
  m_player_grounded = true;
}
void GameSystem::OnPlayerNotGrounded()
{
  m_player_grounded = false;
  if (m_current_axis == 1)
  {
    m_start_axis = 1;
    m_end_axis = 2;
    m_target_axis = 2;
    InitiateAxisChange(2);
  }
}

void GameSystem::OnActionPress(ActionEvent event)
{
  if(event.m_name == "ActionTest")
  {
    Composition *camObj = g_engine_core->SystemCall("ObjectManager", "GetObjectByName", "Camera");
    LogicComponent* logic = camObj->GetComponent<LogicComponent>();
    CameraElevate* newAction = dynamic_cast<CameraElevate*>(logic->PushFront(ActionBank::CameraElevate,0.0f));
    newAction->SetSpeed(5.0f);
    newAction->SetDistance(5.0f); 

    return;
  }

  if(m_player_grounded == false)
    return;

  m_current_axis = g_engine_core->SystemCall("TileSystem", "GetWorldAxis"); 

  int target = m_current_axis;

  //Debug enable roation
  /*m_front_enabled = true;
  m_side_enabled = true;
  m_top_enabled = true;*/

  if (event.m_name == "AxisChangeFront" && m_front_enabled)
  {
    if (m_current_axis == 0)
    {
      target = 2;
      m_target_axis = 2;
    }
    else if(m_prev_axis == 0)
      m_target_axis = 2;

    if (m_current_axis == 1)
    {
      target = 2;
      m_target_axis = 2;
    }
    else if(m_prev_axis == 1)
      m_target_axis = 2;
  }
  if (event.m_name == "AxisChangeTop" && m_top_enabled)
  {
    if(m_current_axis == 0)
      m_prev_axis = 0;
    if (m_prev_axis == 0)
      target = 2;
    else
      target = 1;

    m_target_axis = 1;
  }
  if (event.m_name == "AxisChangeSide" && m_side_enabled)
  {
    if(m_current_axis == 1)
      m_prev_axis = 1;
    if (m_prev_axis == 1)
      target = 2;
    else
      target = 0;
    m_target_axis = 0;
  }
  // For each Axis-Change, initiate responses independently from each other
  if (event.m_name == "AxisChangeRight" && m_side_enabled)
  {
    if(m_current_axis == 1)
      m_prev_axis = 1;
    if (m_prev_axis == 1)
      target = 2;
    else
      target = 0;
    m_target_axis = 0;
  }
  else if (event.m_name == "AxisChangeLeft" && m_front_enabled)
  {
    if (m_current_axis == 0)
    {
      target = 2;
      m_target_axis = 2;
    }
    else if(m_prev_axis == 0)
      m_target_axis = 2;
  }
  else if (event.m_name == "AxisChangeUp" && m_top_enabled)
  {
    if(m_current_axis == 0)
      m_prev_axis = 0;
    if (m_prev_axis == 0)
      target = 2;
    else
      target = 1;

    m_target_axis = 1;
  }
  else if (event.m_name == "AxisChangeDown" && m_front_enabled)
  {
    if (m_current_axis == 1)
    {
      target = 2;
      m_target_axis = 2;
    }
    else if(m_prev_axis == 1)
      m_target_axis = 2;
  }

  m_end_axis = m_target_axis;
  if(target == m_current_axis)
  {
    return;
  }
  else
  {
    m_start_axis = m_current_axis;
    InitiateAxisChange(target);
  }
    
}

void GameSystem::OnRotateToTarget(int axis)
{
  m_target_axis = axis;

  if(m_target_axis != 2 && m_current_axis != 2)
  {
    InitiateAxisChange(2);
  }
  else
    InitiateAxisChange(m_target_axis);
}

void GameSystem::InitiateAxisChange(int axis)
{
  if(!m_rotating)
    g_engine_core->DispatchEvent("OnRotateStart", m_start_axis, m_end_axis);
  m_initiated = true;
  g_engine_core->SystemCall("TileSystem", "SetWorldAxis", axis);
  m_prev_axis = m_current_axis;
  m_current_axis = axis; 
}

void GameSystem::ReturnTC(TileComponent* tc)
{
  m_tile_component = tc;
  PushLoadedData();
}

}
