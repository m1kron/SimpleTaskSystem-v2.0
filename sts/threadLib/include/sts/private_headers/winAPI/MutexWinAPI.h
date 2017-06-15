#pragma once

#include <windows.h>
#include <sts\private_headers\common\NamespaceMacros.h>

NAMESPACE_STS_BEGIN
NAMESPACE_WINAPI_BEGIN

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
	CRITICAL_SECTION m_CriticalSection;
};

////////////////////////////////////////////////////////
//
// INLINES:
//
////////////////////////////////////////////////////////

inline MutexImpl::MUTEX_NATIVE_HANDLE MutexImpl::NativeHandle()
{
	return &m_CriticalSection;
}

NAMESPACE_WINAPI_END
NAMESPACE_STS_END