#pragma once
#include "..\..\..\..\..\basicThreadingLib\include\thread\thisThreadHelpers.h"
#include "..\..\..\..\..\basicThreadingLib\include\atomic\atomic.h"

NAMESPACE_STS_BEGIN

///////////////////////////////////////////////////
// Implementation of lockfree multiple-producer
// multiple-consumer circular FIFO queue.
//  
//  . - empty slot in queue
//  | - ready to read slot ( taken and has data )
//  : - slot is takan, but producer is still writing data to that slot.
//
//	.......||||||||||||||||||||||||:::::::::::::::::......
//		   ^					  ^				   ^
//      read counter     commited counter       write counter
//
template < class T, uint32_t SIZE >
class LockFreePtrQueue
{
public:
	// Push back item to queue. Increases size by 1.
	// Returns true if success.
	bool PushBack( T* const item );

	// Pops first element from queue. Decreases size by 1 and returns obtained item.
	// Returns nullptr in case of failture.
	T* PopFront();

	// Returns size of the queue. WARNING! THIS IN GENERAL WILL RETRUN ONLY APPROXIMATED SIZE!
    uint32_t GetCurrentSize() const;

	// Returns maximum size of this queue.
	uint32_t GetMaxSize() const;

private:
	// Helper function to calculate modulo SIZE of the queue from counter.
	uint32_t CounterToIndex( uint32_t counter ) const;

	btl::Atomic< uint32_t > m_readCounter;
	T* m_queue[ SIZE ];					///< [NOTE]: this arrary also works as a padding between read and write counters to avoid contention.
	btl::Atomic< uint32_t > m_writeCounter;
	char m_padding[ ( BTL_CACHE_LINE_SIZE - sizeof( btl::Atomic< uint32_t > ) ) ]; ///<Another padding to avoid contention.
	btl::Atomic< uint32_t > m_committedWriteCounter;
};

//////////////////////////////////////////////////////////////
//
// INLINES:
//
//////////////////////////////////////////////////////////////
template < class T, uint32_t SIZE >
inline bool LockFreePtrQueue<T, SIZE>::PushBack( T* const item )
{
	// First, check if the queue is full:
	uint32_t write_counter = 0;
	do
	{
		write_counter = m_writeCounter.Load( btl::MemoryOrder::Acquire );
		uint32_t read_counter = m_readCounter.Load( btl::MemoryOrder::Relaxed ); 

		if( ( read_counter + SIZE ) == ( write_counter ) )
		{
			return false; // Queue is full
		}

	// Try to reserve slot, if m_WriteCounter != write_counter, it means that other 
	// thread was faster than our and we have to retry whole operation.
	} while( !m_writeCounter.CompareExchange( write_counter, write_counter + 1, btl::MemoryOrder::Acquire ) );

	// Add stuff to the queue
	m_queue[ CounterToIndex( write_counter ) ] = item;

	// Last thing: we have to commit the change, so every thread knows that we 
	// finished adding new item to the queue. Threads have to commit their data in the same
	// order as they were writing data to the queue - to gain that, every thread has to set committedWriteCounter
	// to be + 1 of write counter that given thread got.
	uint32_t expected = write_counter;
	while( !m_committedWriteCounter.CompareExchange( expected, write_counter + 1, btl::MemoryOrder::Release ) )
	{
		ASSERT( expected <= write_counter );///< Actually fatal assert..
		// Remember that in case of failture of CompareExchange, expected value will contain current
		// value of atomic, so we have to set to have wriet_counter once agian.
		expected = write_counter;
		
		// We have to wait for another thread to finish pushing new data.
		// This means that another thread first started to pushing data, but it took him more
		// time than us, so give him time to finish it's job by yielding.
		// [ NOTE ] Remember that in STS we should have max 2 concurent producer( worker thread and main thread )
		// since we are always adding new task to current worker.

		btl::this_thread::YieldThread();
	}

	return true;
}

//////////////////////////////////////////////////////////////
template < class T, uint32_t SIZE >
inline T* LockFreePtrQueue<T, SIZE>::PopFront()
{
	uint32_t current_read_counter = 0;
	T* return_item = nullptr;

	do
	{
		// First check, whether there is anything to pop:
		current_read_counter = m_readCounter.Load( btl::MemoryOrder::Acquire );
		uint32_t current_committed_counter = m_committedWriteCounter.Load( btl::MemoryOrder::Relaxed );

		if( current_read_counter == current_committed_counter )
		{
			// Queue is empty(or another threads are still committing their's changes).
			return nullptr;
		}

		// Grab the data.
		uint32_t queue_index = CounterToIndex( current_read_counter );
		return_item = m_queue[ queue_index ];

		// [NOTE]: we can't nullptr queue item here, cuz we don't know yet, whether we succeed.
		// ( Other thread can be reading the data now and he will win on increasing read counter ).

	// Try to increase read counter. If failed, it means that another thread has already read it,
	// so we have to retry whole operation.
	} while( !m_readCounter.CompareExchange( current_read_counter, current_read_counter + 1, btl::MemoryOrder::Release ) );

	// [NOTE]: we can't nullptr queue item here, cuz it would require another synchronization between threads
	// ( other thread can be pushing new data right now at the same place if the queue is almost full ).

	// Return the data.
	return return_item;
}	

//////////////////////////////////////////////////////////////
template < class T, uint32_t SIZE >
inline uint32_t LockFreePtrQueue<T, SIZE>::GetCurrentSize() const
{
	return ( m_writeCounter - m_readCounter );
}

//////////////////////////////////////////////////////////////
template < class T, uint32_t SIZE >
inline uint32_t LockFreePtrQueue<T, SIZE>::GetMaxSize() const
{
	return SIZE;
}

//////////////////////////////////////////////////////////////
template < class T, uint32_t SIZE >
inline uint32_t LockFreePtrQueue<T, SIZE>::CounterToIndex( uint32_t counter ) const
{
	STATIC_ASSERT( IsPowerOf2< SIZE >::value == 1, "SIZE of LockFreePtrQueue has to be power of 2!" );
	return counter & ( SIZE - 1 ); 
}

NAMESPACE_STS_END