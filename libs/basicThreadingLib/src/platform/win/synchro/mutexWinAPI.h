#pragma once
#include "..\..\platformApi.h"
#include <windows.h>

NAMESPACE_BTL_BEGIN
NAMESPACE_PLATFORM_API_BEGIN

class MutexImpl
{
protected:
	typedef CRITICAL_SECTION* MUTEX_NATIVE_HANDLE;

	MutexImpl();
	~MutexImpl();

	// Mutex cannot be moved or copied.
	MutexImpl( MutexImpl&& other ) = delete;
	MutexImpl( const MutexImpl& ) = delete;
	MutexImpl& operator= ( const MutexImpl& ) = delete;

	void Lock();
	bool TryLock();
	void Unlock();

	MUTEX_NATIVE_HANDLE NativeHandle();

private:
	CRITICAL_SECTION m_criticalSection;
};

////////////////////////////////////////////////////////
//
// INLINES:
//
////////////////////////////////////////////////////////

inline MutexImpl::MUTEX_NATIVE_HANDLE MutexImpl::NativeHandle()
{
	return &m_criticalSection;
}

NAMESPACE_PLATFORM_API_END
NAMESPACE_BTL_END
