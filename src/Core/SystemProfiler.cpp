// David Evans
// 2011/05/12
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "SystemProfiler.h"

namespace Improbability
{

void SystemProfiler::UpdateSystemTime(char const *name, float dt)
{
  m_profiles[name] = dt;
}

float SystemProfiler::GetSystemTime(char const *name)
{
  ProfileMap::iterator it = m_profiles.find(name);
  // Assert finding the system

  return it->second;
}

}