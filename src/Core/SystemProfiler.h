// David Evans
// 2011/05/12
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#include <map>

#include "../Containers/String.h"

namespace Improbability
{

class SystemProfiler
{
public:
  typedef std::map<String, float> ProfileMap;

public:
  void UpdateSystemTime(char const *name, float dt);

  float GetSystemTime(char const *name);

private:
  ProfileMap m_profiles;
};

}