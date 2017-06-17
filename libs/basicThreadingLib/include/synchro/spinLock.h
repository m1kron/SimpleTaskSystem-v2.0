#pragma once
#include "..\atomic\atomic.h"
#include "..\..\..\commonLib\include\types.h"

NAMESPACE_BTL_BEGIN

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
	Atomic< int32_t > m_atomic;
};

NAMESPACE_BTL_END
