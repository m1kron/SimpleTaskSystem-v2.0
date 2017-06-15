#pragma once
#include <sts\private_headers\common\NamespaceMacros.h>
#include <sts\lowlevel\atomic\Atomic.h>

NAMESPACE_STS_BEGIN

//////////////////////////////////////////
// Implementation of simple, not-recursive spinlock.
class SpinLock 
{
public:
	SpinLock();
	~SpinLock();

	SpinLock( const SpinLock& ) = delete;
	SpinLock& operator= ( const SpinLock& ) = delete;

	// Locks the mutex. Blocks until gained the ownership of the mutex.
	void Lock();

	// Tries to lock. If returns true, then lock is obtaned. 
	bool TryToLock();

	// Unlocks the mutex.
	void Unlock();

private:
	sts::Atomic< int > m_atomic;
};

NAMESPACE_STS_END