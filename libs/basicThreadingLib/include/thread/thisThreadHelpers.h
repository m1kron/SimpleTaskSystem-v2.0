#pragma once
#include "..\..\src\platform\threadPlatform.h"

NAMESPACE_BTL_BEGIN

typedef PlatformAPI::THREAD_ID THREAD_ID;

/////////////////////////////////////////////////////
// Helper functions:
namespace this_thread
{

// Yields thread that called this function
void YieldThread();

// Returns thread id of thread that called this function.
THREAD_ID GetThreadID();

// Sleeps on calling thread by specified amount of time in miliseconds.
void SleepFor( uint32_t miliseconds );

///////////////////////////////////////////////////////////
//
// IMPLEMENTATION:
//
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
inline void YieldThread()
{
	PlatformAPI::YieldThread();
}

///////////////////////////////////////////////////////////
inline THREAD_ID GetThreadID()
{
	return PlatformAPI::GetThreadID();
}

///////////////////////////////////////////////////////////
inline void SleepFor( uint32_t miliseconds )
{
	PlatformAPI::SleepFor( miliseconds );
}

}

NAMESPACE_BTL_END
