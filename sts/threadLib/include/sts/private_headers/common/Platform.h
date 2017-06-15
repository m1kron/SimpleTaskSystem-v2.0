#pragma once

#define STS_PLATFORM_WINDOWS_64

#ifdef STS_PLATFORM_WINDOWS_64
#define STS_ALIGNED( aligment ) __declspec( align( aligment ) )
#define STS_CACHE_LINE_SIZE 64
#endif