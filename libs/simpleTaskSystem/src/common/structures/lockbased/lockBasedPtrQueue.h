#pragma once
#include "..\..\..\..\..\basicThreadingLib\include\synchro\spinLock.h"
#include "..\..\..\..\..\basicThreadingLib\include\synchro\lockGuards.h"

NAMESPACE_STS_BEGIN
NAMESPACE_COMMON_BEGIN

///////////////////////////////////////////////////
// Implementation of lock based multiple-producer
// multiple-consumer circular FIFO queue.
// Default LockPrimitive is SpinLock.
template < class T, uint32_t SIZE, class TLockPrimitive = btl::SpinLock >
class LockBasedPtrQueue
{
public:
	LockBasedPtrQueue();

	// Push item back to the queue. Increases size by 1.
	// Returns true if success.
	bool PushBack( T* const item );

	// Pops first element from queue. Decreases size by 1 and returns obtained item.
	// Returns nullptr in case of failture ( queue is empty ).
	T* PopFront();

	// Pops element from back. Decreases size by 1 and returns obtained item.
	// Returns nullptr in case of failture ( queue is empty ).
	T* PopBack();

	// Returns size of the queue.
	uint32_t GetCurrentSize() const;

	// Returns maximum size of this queue.
	uint32_t GetMaxSize() const;

private:
	// Helper function to calculate modulo SIZE of the queue from counter.
	uint32_t CounterToIndex( uint32_t counter ) const;

	// Returns size of the queue. This is not thread safe.
	uint32_t GetCurrentSize_NotThreadSafe() const;

	// -----------------------------------------
	typedef btl::LockGuard< TLockPrimitive > TLockGuard;

	T* m_queue[ SIZE ];		
	mutable TLockPrimitive m_lock;
	uint32_t m_readCounter;
	uint32_t m_writeCounter;
};

//////////////////////////////////////////////////////////////
//
// INLINES:
//
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
template < class T, uint32_t SIZE, class TLockPrimitive >
inline LockBasedPtrQueue<T, SIZE, TLockPrimitive>::LockBasedPtrQueue()
	: m_writeCounter( 0 )
	, m_readCounter( 0 )
{
	STATIC_ASSERT( IsPowerOf2< SIZE >::value == 1, "SIZE of LockBasedPtrQueue has to be power of 2!" );
}

//////////////////////////////////////////////////////////////
template < class T, uint32_t SIZE, class TLockPrimitive >
inline bool LockBasedPtrQueue<T, SIZE, TLockPrimitive>::PushBack( T* const item )
{
	TLockGuard guard( m_lock );
	if( m_readCounter + SIZE > m_writeCounter )
	{
		m_queue[ CounterToIndex( m_writeCounter++ ) ] = item;
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////
template < class T, uint32_t SIZE, class TLockPrimitive >
inline T* LockBasedPtrQueue<T, SIZE, TLockPrimitive>::PopFront()
{
	TLockGuard guard( m_lock );
	if( GetCurrentSize_NotThreadSafe() > 0 )
		return m_queue[ CounterToIndex( m_readCounter++ ) ];

	return nullptr;
}

//////////////////////////////////////////////////////////////
template < class T, uint32_t SIZE, class TLockPrimitive >
inline T* LockBasedPtrQueue<T, SIZE, TLockPrimitive>::PopBack()
{
	TLockGuard guard( m_lock );
	if ( GetCurrentSize_NotThreadSafe() > 0 )
		return m_queue[ CounterToIndex( --m_writeCounter ) ];

	return nullptr;
}

//////////////////////////////////////////////////////////////
template < class T, uint32_t SIZE, class TLockPrimitive >
inline uint32_t LockBasedPtrQueue<T, SIZE, TLockPrimitive>::GetCurrentSize() const
{
	TLockGuard guard( m_lock );
	return GetCurrentSize_NotThreadSafe();
}

//////////////////////////////////////////////////////////////
template < class T, uint32_t SIZE, class TLockPrimitive >
inline uint32_t LockBasedPtrQueue<T, SIZE, TLockPrimitive>::GetMaxSize() const
{
	return SIZE;
}

//////////////////////////////////////////////////////////////
template < class T, uint32_t SIZE, class TLockPrimitive >
inline uint32_t LockBasedPtrQueue<T, SIZE, TLockPrimitive>::CounterToIndex( uint32_t counter ) const
{
	return counter & ( SIZE - 1 );
}

//////////////////////////////////////////////////////////////
template<class T, uint32_t SIZE, class TLockPrimitive>
inline uint32_t LockBasedPtrQueue<T, SIZE, TLockPrimitive>::GetCurrentSize_NotThreadSafe() const
{
	return m_writeCounter - m_readCounter;
}

NAMESPACE_COMMON_END
NAMESPACE_STS_END