// David Evans
// 2011/06/30
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "Resource.h"

namespace Improbability
{

class MetaComposition;

class TextResource : public Resource
{
public:
  static void BindMeta(MetaComposition*);

  // Loads the resource
  // filepath - Path of the file to load
  void Load(char const *filepath);

  // Unloads the resource
  void Unload();

  // Get the string buffer
  char const* GetBuffer(){return m_buffer;}
  int GetSize(){return m_size;}

private:
  // Text buffer read in
  char *m_buffer;

  // Size of the buffer
  int m_size;
};

}