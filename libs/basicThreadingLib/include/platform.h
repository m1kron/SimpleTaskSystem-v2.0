#pragma once

// Just one platform for now.
#define BSL_PLATFORM_WINDOWS_64

#ifdef BSL_PLATFORM_WINDOWS_64
#include "..\src\platform\win\alignmentWinAPI.h"
#endif