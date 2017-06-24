#pragma once

// Just one platform for now.
#define BSL_PLATFORM_WINDOWS_64_MSVC

// Common platform stuff:
#ifdef BSL_PLATFORM_WINDOWS_64_MSVC
#define BTL_ALIGNED( aligment ) __declspec( align( aligment ) )
#define BTL_CACHE_LINE_SIZE 64
#endif