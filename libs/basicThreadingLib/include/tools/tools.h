#pragma once
#include "..\..\src\platform\toolsPlatform.h"

NAMESPACE_BTL_BEGIN

// Return number of logical cores in the system ( real cores + HyperThreading cores ).
uint32_t GetLogicalCoresSize();

///////////////////////////////////////////////////////////
//
// INLINES:
//
///////////////////////////////////////////////////////////

inline uint32_t GetLogicalCoresSize()
{
	return PlatformAPI::GetLogicalCoresCountImpl();
}

NAMESPACE_BTL_END