#pragma once

#include <sts\private_headers\common\NamespaceMacros.h>
#include <Windows.h>

NAMESPACE_STS_BEGIN
NAMESPACE_WINAPI_BEGIN

inline unsigned GetLogicalCoresCountImpl()
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );
	return sysinfo.dwNumberOfProcessors;
}

NAMESPACE_WINAPI_END
NAMESPACE_STS_END