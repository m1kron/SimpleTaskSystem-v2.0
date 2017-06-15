#pragma once
#include <sts\private_headers\tools\ToolsPlatform.h>

NAMESPACE_STS_BEGIN
NAMESPACE_TOOLS_BEGIN

// Return number of logical cores in the system ( real cores + HT ).
unsigned GetLogicalCoresSize();

///////////////////////////////////////////////////////////
//
// INLINES:
//
///////////////////////////////////////////////////////////

inline unsigned GetLogicalCoresSize()
{
	return PlatformAPI::GetLogicalCoresCountImpl();
}

NAMESPACE_TOOLS_END
NAMESPACE_STS_END