// David Evans
// 2011/08/31
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Resource/Resource.h"
#include "../Containers/HashMap.h"
#include "../Math/Vector3.h"

namespace Improbability
{

class MetaComposition;

class World : public Resource
{
public:
  class Object
  {
  public:
    String m_blueprint;
    String m_name;
    Vector3 m_position;
  };

  typedef std::vector<String> StringArray;
  typedef std::vector<Object> ObjectArray;
  typedef HashMap<String, StringArray*> StringsMap;

  static void BindMeta(MetaComposition*);

  void Load(char const *filepath);
  void Unload();

  StringArray* GetTilemapNames(char const *type);
  ObjectArray* GetObjectArray() {return &m_objects;}
  StringsMap& GetMap() {return m_maps;}
  String GetFocusObject() {return m_focus_object;}
  String GetGraphicsMap() {return m_graphics_map;}

private:
  StringsMap m_maps;
  ObjectArray m_objects;
  String m_focus_object;
  String m_graphics_map;
};

}