#pragma once

#include <windows.h>
#include <sts\private_headers\common\NamespaceMacros.h>

NAMESPACE_STS_BEGIN

class ThreadBase;

NAMESPACE_WINAPI_BEGIN

typedef DWORD THREAD_ID;

////////////////////////////////////////////////////////////////
void YieldThread();
THREAD_ID GetThreadID();
void SleepFor( unsigned miliseconds );
void ConvertToFiber( void* params = nullptr );

////////////////////////////////////////////////////////////////
class ThreadImpl
{
protected:
	ThreadImpl();
	ThreadImpl( ThreadImpl&& other );

	ThreadImpl(const ThreadImpl&) = delete;
	ThreadImpl& operator=( const ThreadImpl& ) = delete;

	~ThreadImpl();

	void StartThread( ThreadBase* thread );
	void Join();
	void Detach();
	THREAD_ID GetThreadID() const;

	void SetThreadName( const char* thread_name );
	
private:
	THREAD_ID m_id;
	HANDLE m_threadHandle;
};

NAMESPACE_WINAPI_END
NAMESPACE_STS_END