// David Evans
// 2011/05/18
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

namespace Improbability
{

// Types

// Macros
#define OffsetOf(T,M) (size_t)&(((T *)0)->M)
#define CatI(a,b) a##b
#define Cat(a,b) CatI(a,b)
#define ForceLinkThis(x) int g_force_link_##x = 0;
#define ForceLinkThat(x) {extern int g_force_link_##x; g_force_link_##x = 1;}

// Math defines
float const k_epsilon = 0.000001f;
float const k_pi = 3.1415926f;
float const k_two_pi = k_pi * 2.0f;
float const k_half_pi = k_pi * 0.5f;
#define DegreeToRadian(d) ((d) * k_pi / 180.0f)
#define RedianToDegree(r) ((r) * 180.0f / k_pi)

}