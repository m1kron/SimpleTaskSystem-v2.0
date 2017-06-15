#pragma once

#include <sts\private_headers\common\Platform.h>

/////////////////////////////////////////////////////////
// Windows platform:

#ifdef STS_PLATFORM_WINDOWS_64
namespace sts
{
	namespace WinAPI {}
}

namespace PlatformAPI = sts::WinAPI;
#endif
///////////////////////////////////////////////////////