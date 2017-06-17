#pragma once
#include "..\..\platformApi.h"
#include <windows.h>

NAMESPACE_BTL_BEGIN
NAMESPACE_PLATFORM_API_BEGIN

class ManualResetEventImpl
{
protected:
	typedef HANDLE EVENT_NATIVE_HANDLE;

	ManualResetEventImpl();
	~ManualResetEventImpl();
	ManualResetEventImpl( ManualResetEventImpl&& oth_event );
	
	ManualResetEventImpl( const ManualResetEventImpl& ) = delete;
	ManualResetEventImpl& operator= ( const ManualResetEventImpl& ) = delete;

	void SetEvent();
	void ResetEvent();
	bool IsEventSet();
	void Wait();
	void WaitFor( unsigned miliseconds );

	EVENT_NATIVE_HANDLE GetNativeHandle();

private:
	EVENT_NATIVE_HANDLE m_eventHandle;
	bool m_isEventSet;
};

NAMESPACE_PLATFORM_API_END
NAMESPACE_BTL_END
