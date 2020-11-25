// David Evans
// 2011/06/17
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Containers/String.h"

#undef GetClassName

namespace Improbability
{

class ResourceManager;

class Resource
{
public:
  friend class ResourceManager;

  virtual ~Resource() {};

  // Loads the resource
  // filepath - Path of the file to load
  virtual void Load(char const *filepath) = 0;

  // Unloads the resource
  virtual void Unload() = 0;

  // Set the pass name of the resource
  void SetPassName(char const *name) {m_pass_name = name;}

  // Get the class name of the resource
  String GetClassName() {return m_name;}

  // Get the pass name
  String GetPassName() {return m_pass_name;}

  // Get the filename of  the resource
  String GetFileName() {return m_filename;}

private:
  // Name of the resource class (e.g. "TextResource")
  String m_name;

  // Filename of the resource
  String m_filename;

  // Name of the pass to get loaded on
  String m_pass_name;
};

}