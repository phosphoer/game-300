// David Evans
// 2011/09/27
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Resource/Resource.h"
#include "../Containers/HashMap.h"

namespace Improbability
{

class MetaComposition;

class IntToStringResource : public Resource
{
public:
  static void BindMeta(MetaComposition*);

  // Load the blueprint from a file
  // filepath - Filepath to load
  void Load(char const *filepath);

  // Unloads the blueprint
  void Unload();

  // Get a string from the map
  String GetString(int id);

private:
  // Map of ints to strings
  HashMap<int, String> m_map;
};

}