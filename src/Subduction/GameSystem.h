// David Evans & Jake Leonard
// 2011/09/27
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Core/System.h"
#include "../Composition/Composition.h"
#include "../Tile/Tilemap.h"
#include "../Tile/TileComponent.h"

namespace Improbability
{

class KeyboardEvent;
class ActionEvent;
class MetaComposition;
class TilePos3;
class TileComponent;

class GameSystem : public System
{
public:
  typedef std::vector<TilePos3> LevelCoins;
  typedef Improbability::HashMap<String, LevelCoins> CoinMap;

  static void BindMeta(MetaComposition*);
  GameSystem();

  // Inherited methods
  void InitializeGlobal();
  void InitializeInstance();
  void UpdateInstance(float dt);
  void UninitializeInstance();
  void UninitializeGlobal() { }
  void Copy(System *dest, bool deep) { }
  void Deserialize(Serializer &serializer) { }

  // Messages
  void OnSpaceReady();
  void OnKeyPress(KeyboardEvent) { }
  void OnActionPress(ActionEvent);
  void OnAxisInitalize(int initalAxis);
  void OnAxisChangeComplete(int axis);
  void OnAxisChange(int old_axis, int axis);
  void InitiateAxisChange(int axis);
  void OnPlayerNotGrounded();
  void OnPlayerGrounded();
  void OnRotateToTarget(int axis);
  void OnTopToggle(bool toggle);
  void OnSideToggle(bool toggle);
  void OnFrontToggle(bool toggle);

  // Saving and loading
  void OnGameSave();
  void OnGamePauseSave();
  void QueryHasSaves();
  void OnLoadRequested();
  void GameReadyToLoad();
  void OnNewGame() { sm_coin_locations.Clear(); }
  void OnCoinCollection(TilePos3 coinPos);
  void ReturnTC(TileComponent* tc);
  void PushLoadedData();
private:
  int m_start_axis;
  int m_end_axis;
  int m_current_axis;
  int m_target_axis;
  bool m_rotate_on_next;
  int m_prev_axis;
  bool m_player_grounded;
  int m_cursor_count;
  bool m_initiated;

  //Input flags
  bool m_top_enabled;
  bool m_side_enabled;
  bool m_front_enabled;
  bool m_rotating;
  
  //Save and load members
  static CoinMap sm_coin_locations;
  TileComponent* m_tile_component;
  static TilePos3 sm_player_pos;
  static bool sm_loading;
  static int sm_player_facing;
  void SaveGame();
  bool CanSave();
  bool HasSave();
  String GameSystem::GetSavePath();
};

}