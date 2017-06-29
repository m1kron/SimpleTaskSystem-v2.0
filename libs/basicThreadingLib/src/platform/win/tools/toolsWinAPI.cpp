#include "toolsWinAPI.h"

NAMESPACE_BTL_BEGIN
NAMESPACE_PLATFORM_API_BEGIN

/////////////////////////////////////////////////////////////
uint32_t GetLogicalCoresCountImpl()
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );
	return sysinfo.dwNumberOfProcessors;
}

NAMESPACE_PLATFORM_API_END
NAMESPACE_BTL_END