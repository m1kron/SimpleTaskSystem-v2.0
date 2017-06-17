#pragma once
#include "..\..\..\basicThreadingLib\include\synchro\spinLock.h"
#include "..\..\..\basicThreadingLib\include\synchro\LockGuards.h"

NAMESPACE_STS_BEGIN

///////////////////////////////////////////////////
// Implementation of lock based multiple-producer
// multiple-consumer circular FIFO queue.
// Default LockPrimitive is SpinLock.
template < class T, unsigned SIZE, class TLockPrimitive = btl::SpinLock >
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
	unsigned Size_NotThreadSafe() const;

	// Returns maximum size of this queue.
	unsigned GetMaxSize() const;

private:
	// Helper function to calculate modulo SIZE of the queue from counter.
	unsigned CounterToIndex( unsigned counter ) const;

	// -----------------------------------------
	typedef btl::LockGuard< TLockPrimitive > TLockGuard;

	T* m_queue[ SIZE ];		
	TLockPrimitive m_lock;
	unsigned m_readCounter;
	unsigned m_writeCounter;
};

//////////////////////////////////////////////////////////////
//
// INLINES:
//
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
template < class T, unsigned SIZE, class TLockPrimitive >
inline LockBasedPtrQueue<T, SIZE, TLockPrimitive>::LockBasedPtrQueue()
	: m_writeCounter( 0 )
	, m_readCounter( 0 )
{
	STATIC_ASSERT( IsPowerOf2< SIZE >::value == 1, "SIZE of LockBasedPtrQueue has to be power of 2!" );
}

//////////////////////////////////////////////////////////////
template < class T, unsigned SIZE, class TLockPrimitive >
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
template < class T, unsigned SIZE, class TLockPrimitive >
inline T* LockBasedPtrQueue<T, SIZE, TLockPrimitive>::PopFront()
{
	TLockGuard guard( m_lock );
	if( m_readCounter < m_writeCounter )
		return m_queue[ CounterToIndex( m_readCounter++ ) ];

	return nullptr;
}

//////////////////////////////////////////////////////////////
template < class T, unsigned SIZE, class TLockPrimitive >
inline T* LockBasedPtrQueue<T, SIZE, TLockPrimitive>::PopBack()
{
	TLockGuard guard( m_lock );
	if ( m_writeCounter > m_readCounter )
		return m_queue[ CounterToIndex( --m_writeCounter ) ];

	return nullptr;
}

//////////////////////////////////////////////////////////////
template < class T, unsigned SIZE, class TLockPrimitive >
inline unsigned LockBasedPtrQueue<T, SIZE, TLockPrimitive>::Size_NotThreadSafe() const
{
	return m_writeCounter - m_readCounter;
}

//////////////////////////////////////////////////////////////
template < class T, unsigned SIZE, class TLockPrimitive >
inline unsigned LockBasedPtrQueue<T, SIZE, TLockPrimitive>::GetMaxSize() const
{
	return SIZE;
}

template < class T, unsigned SIZE, class TLockPrimitive >
inline unsigned LockBasedPtrQueue<T, SIZE, TLockPrimitive>::CounterToIndex( unsigned counter ) const
{
	return counter & ( SIZE - 1 );
}

NAMESPACE_STS_END