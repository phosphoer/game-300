// David Evans
// 2011/05/13
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{

class Timer
{
public:
  Timer();

  float GetDt();
  float GetTotalTime();

private:
  __int64 m_previous_time;
  __int64 m_current_time;
  __int64 m_ticks_per_second;
  __int64 m_total_time;
  __int64 m_offset;
  float m_conversion_factor;
};

}