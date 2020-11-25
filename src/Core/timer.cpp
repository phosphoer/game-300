// David Evans
// 2011/05/12
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#include "Precompiled.h"
#include "Timer.h"

namespace Improbability
{

Timer::Timer()
{
  QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_ticks_per_second));
  QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_previous_time));
  m_conversion_factor = 1.0f / static_cast<float>(m_ticks_per_second);
}

float Timer::GetDt()
{
  QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_current_time));
  m_offset = m_current_time - m_previous_time;
  m_total_time += m_offset;
  m_previous_time = m_current_time;
  float dt = static_cast<float>(m_offset) * m_conversion_factor;
  return dt;
}

float Timer::GetTotalTime()
{
  GetDt();
  return static_cast<float>(m_total_time) * m_conversion_factor;
}

}