// Aaron Biddlecom
// 2011/07/23
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Composition/Component.h"

namespace Improbability
{

class Serializer;
class MetaComposition;

class TileGameplayComponent : public Component
{
public:
  static void BindMeta(MetaComposition*);

  int GetTileType(){return m_tile_type;}
  int GetDefaultAxis();
  int GetPreviousAxis();
  void SetPreviousAxis(int axis);
  void SetRotationActive( bool active );
  void SetLevelNumber( float number ){m_level_number = (int)number;}

  //Inherited Methods
  TileGameplayComponent();
  TileGameplayComponent(TileGameplayComponent const &rhs);
  void Initialize();
  void Uninitialize();
  Component* Clone();
  void Serialize(Serializer &serializer) { }
  void Deserialize(Serializer &serializer);
  void SetAnchorActive( bool active );
  int GetAnchorAxis();
  int GetLevelNumber(){return m_level_number;}
  bool IsRotateActive(){return m_rotation_active;}
private:
  int m_prev_axis;
  int m_default_axis;
  bool m_anchor_begin;
  int m_anchor_axis;
  bool m_rotation_active;
  bool m_anchor_active;
  int m_tile_type;
  int m_level_number;
};

}