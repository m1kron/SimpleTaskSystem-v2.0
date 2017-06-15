#pragma once

#include <sts\private_headers\synchro\SynchronizationPlatform.h>
#include <commonlib\Macros.h>

NAMESPACE_STS_BEGIN

//////////////////////////////////////////
// Represents general purpose mutex.
class Mutex : private PlatformAPI::MutexImpl
{
	BASE_CLASS( PlatformAPI::MutexImpl );

public:
	typedef __base::MUTEX_NATIVE_HANDLE MUTEX_NATIVE_HANDLE;

	Mutex() {}
	~Mutex() {}

	Mutex( const Mutex& ) = delete;
	Mutex& operator= ( const Mutex& ) = delete;

	// Locks the mutex. Blocks until gained the ownership of the mutex.
	void Lock();

	// Attempts to gain ownership of the mutex without blocking. Returns true if succeed.
	bool TryLock();

	// Unlocks the mutex.
	void Unlock();

	// Returns system native handle to the mutex.
	MUTEX_NATIVE_HANDLE NativeHandle();
};

///////////////////////////////////////////////////
//
// INLINES:
//
///////////////////////////////////////////////////

///////////////////////////////////////////////////
inline void Mutex::Lock()
{
	__base::Lock();
}

///////////////////////////////////////////////////
inline bool Mutex::TryLock()
{
	return __base::TryLock();
}

///////////////////////////////////////////////////
inline void Mutex::Unlock()
{
	__base::Unlock();
}

///////////////////////////////////////////////////
inline Mutex::MUTEX_NATIVE_HANDLE Mutex::NativeHandle()
{
	return __base::NativeHandle();
}

NAMESPACE_STS_END