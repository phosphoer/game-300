// David Evans
// 2011/10/01
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "MetaComposition.h"
#include "MetaMacros.h"

namespace Improbability
{

MetaComposition *g_meta_comp_head = 0;
MetaComposition *g_meta_comp_tail = g_meta_comp_head;

MakeMeta(MetaComposition);

void MetaComposition::BindMeta(MetaComposition *comp)
{
}

MetaComposition::MetaComposition(char const *name, void (*BindFunction)(MetaComposition*)):
  m_name(name), 
  m_bind_function(BindFunction),
  m_next(0)
{
  // Insert the meta data into the linked list
  if (!g_meta_comp_head)
  {
    g_meta_comp_head = this;
  }
  else
  {
    m_next = g_meta_comp_head;
    g_meta_comp_head = this;
  }

  // Bind meta components
  // ClassType::BindMeta(this);
  // if (BindFunction)
    // BindFunction(this);

  g_meta_comp_tail = this;
}

void MetaComposition::Uninitialize()
{
  for (auto it = m_components->Begin(); it != m_components->End(); ++it)
    delete it->second;
}

}