// David Evans
// 2011/07/19
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include "../Resource/Resource.h"

namespace Improbability
{

class Composition;
class MetaComposition;

class Blueprint : public Resource
{
public:
  static void BindMeta(MetaComposition*);

  // Load the blueprint from a file
  // filepath - Filepath to load
  void Load(char const *filepath);

  // Unloads the blueprint
  void Unload();

  // Get the composition
  Composition* GetComposition() {return m_composition;}

private:
  // The composition the blueprint represents
  Composition *m_composition;

  // Filename
  String m_filename;
};

}