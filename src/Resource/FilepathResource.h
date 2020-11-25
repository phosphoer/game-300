// David Evans
// 2011/07/20
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "Resource.h"
#include "../Containers/String.h"

namespace Improbability
{

class MetaComposition;

class FilepathResource : public Resource
{
public:
  static void BindMeta(MetaComposition*);

  // Loads the resource
  // filepath - Path of the file to load
  void Load(char const *filepath);

  // Unloads the resource
  void Unload();

  // Get the string buffer
  String GetFilepath(){return m_filepath.GetCString();}

private:
  // Path of the file
  String m_filepath;
};

}