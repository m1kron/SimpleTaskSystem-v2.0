#pragma once
#include "..\..\src\platform\fiberPlatform.h"

NAMESPACE_BTL_BEGIN

typedef PlatformAPI::FIBER_ID FIBER_ID;

/////////////////////////////////////////////////////
// Helper functions:
namespace this_fiber
{

// Returns fiber id of fiber that called this function.
FIBER_ID GetFiberID();

// Switches to another fiber.
void SwitchToFiber( FIBER_ID new_fiber_id );

// Converts this fiber to regular thread. 
bool ConvertFiberToThread();

// Convert thread to fibers( allows to switch to another fiber ).
FIBER_ID ConvertThreadToFiber( void* params = nullptr );

// Returns true if thread is already converted to fiber.
bool IsThreadConvertedToFiber();

////////////////////////////////////////////////////////////////////////
//
// IMPLEMENTATION:
//
////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
inline FIBER_ID GetFiberID()
{
	return PlatformAPI::GetCurrentFiberID();
}

//////////////////////////////////////////////////////////////
inline void SwitchToFiber( FIBER_ID new_fiber_id )
{
	PlatformAPI::SwitchToFiber( new_fiber_id );
}

//////////////////////////////////////////////////////////////
inline bool ConvertFiberToThread()
{
	return PlatformAPI::ConvertFiberToThread();
}

///////////////////////////////////////////////////////////
inline FIBER_ID ConvertThreadToFiber( void* params )
{
	return PlatformAPI::ConvertThreadToFiber( params );
}

///////////////////////////////////////////////////////////
inline bool IsThreadConvertedToFiber()
{
	return PlatformAPI::IsThreadConvertedToFiber();
}

}

NAMESPACE_BTL_END
