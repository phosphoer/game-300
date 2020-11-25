// David Evans
// 2011/09/08
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Resource/Resource.h"

namespace Improbability
{

class MetaComposition;

class InputActions : public Resource
{
public:
  typedef std::vector<String> StringArray;

  static void BindMeta(MetaComposition*);

  // Loads the resource
  // filepath - Path of the file to load
  void Load(char const *filepath);

  // Unloads the resource
  void Unload();

  // Get action associated with key
  String GetAction(unsigned key);

  // Number of keys to host actions for
  static int const k_num_keys = 256;
  
private:
  // Array of action names indexed by key value
  String m_actions[k_num_keys];
};

}