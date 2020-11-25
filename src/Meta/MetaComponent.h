// David Evans
// 2011/10/01
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{

class MetaComposition;

class MetaComponent
{
public:
  // Get the name of the component
  virtual String GetComponentName() = 0;
  virtual ~MetaComponent() {}

  // Pointer to the parent meta composition
  MetaComposition *m_parent;
};

}