// David Evans
// 2011/05/18
// All content (c) 2010 DigiPen (USA) Corporation, all rights reserved.

#pragma once

#define WIN32_LEAN_AND_MEAN

// standard librarys
#include <cstdarg>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <climits>
#include <iostream>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <algorithm>

// 3rd-party dependencies
#include <windows.h>

// other global includes
#include "Defines.h"
#include "DynamicValue.h"
#include "Variable.h"
#include "DebugErrors.h"
#include "../Meta/MetaMacros.h"
#include "../Meta/Meta.h"
#include "../Meta/PropertyAnimator.h"
#include "../Containers/String.h"
#include "../Containers/HashMap.h"
