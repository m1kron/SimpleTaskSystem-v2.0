#pragma once

#include <windows.h>
#include <sts\private_headers\common\NamespaceMacros.h>

NAMESPACE_STS_BEGIN
NAMESPACE_WINAPI_BEGIN

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

NAMESPACE_WINAPI_END
NAMESPACE_STS_END