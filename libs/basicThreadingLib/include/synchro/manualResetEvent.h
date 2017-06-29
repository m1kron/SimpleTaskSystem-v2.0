#pragma once
#include "..\..\src\platform\manualResetEventPlatform.h"

NAMESPACE_BTL_BEGIN

class ManualResetEvent : private PlatformAPI::ManualResetEventImpl
{
	BASE_CLASS( PlatformAPI::ManualResetEventImpl );

public:
	typedef __base::EVENT_NATIVE_HANDLE EVENT_NATIVE_HANDLE;

	ManualResetEvent() {}
	~ManualResetEvent() {}
	ManualResetEvent( ManualResetEvent&& oth_event ) {}

	ManualResetEvent( const ManualResetEvent& ) = delete;
	ManualResetEvent& operator= ( const ManualResetEvent& ) = delete;

	// Set event to signaled state.
	void SetEvent();

	// Resets event to nonsignaled state.
	void ResetEvent();

	// Returns true if event is in signaled state.
	bool IsEventSet();

	// Blocks thread until event is in signaled state.
	void Wait();

	// Blocks thread until event is in signaled state or specified time in miliseconds has elapsed.
	void WaitFor( uint32_t miliseconds );

	// Returns platform native handle.
	EVENT_NATIVE_HANDLE GetNativeHandle();
};

///////////////////////////////////////////////////////////
//
// INLINES:
//
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
inline void ManualResetEvent::SetEvent()
{
	__base::SetEvent();
}

///////////////////////////////////////////////////////////
inline void ManualResetEvent::ResetEvent()
{
	__base::ResetEvent();
}

///////////////////////////////////////////////////////////
inline bool ManualResetEvent::IsEventSet()
{
	return __base::IsEventSet();
}

///////////////////////////////////////////////////////////
inline void ManualResetEvent::Wait()
{
	__base::Wait();
}

///////////////////////////////////////////////////////////s
inline void ManualResetEvent::WaitFor( uint32_t miliseconds )
{
	__base::WaitFor( miliseconds );
}

///////////////////////////////////////////////////////////
inline ManualResetEvent::EVENT_NATIVE_HANDLE ManualResetEvent::GetNativeHandle()
{
	return __base::GetNativeHandle();
}

NAMESPACE_BTL_END
