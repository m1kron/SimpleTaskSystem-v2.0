#pragma once
#include "..\winPlatformCommon.h"

NAMESPACE_BTL_BEGIN

class ThreadBase;

NAMESPACE_PLATFORM_API_BEGIN

typedef DWORD THREAD_ID;
#define INVALID_THREAD_ID 0

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

NAMESPACE_PLATFORM_API_END
NAMESPACE_BTL_END
