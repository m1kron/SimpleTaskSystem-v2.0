#pragma once
#include <commonlib\Macros.h>
#include <sts\private_headers\thread\ThreadPlatform.h>

NAMESPACE_STS_BEGIN

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
void SleepFor( unsigned miliseconds );

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
inline void SleepFor( unsigned miliseconds )
{
	PlatformAPI::SleepFor( miliseconds );
}

}

NAMESPACE_STS_END