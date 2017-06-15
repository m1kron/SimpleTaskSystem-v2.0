#include <sts\private_headers\tasking\TaskAllocator.h>
#include <sts\tools\PositiveNumberHasher.h>
#include <commonlib\compile_time_tools\IsPowerOf2.h>

NAMESPACE_STS_BEGIN

///////////////////////////////////////////////////
TaskAllocator::TaskAllocator()
{
	STATIC_ASSERT( IsPowerOf2< TASK_POOL_SIZE >::value == 1, "TASK_POOL_SIZE has to be power of 2!" );
}

///////////////////////////////////////////////////
TaskHandle TaskAllocator::AllocateNewTask()
{
	// Hashing is used to avoid false sharing of pool markers.
	// [NOTE] : think about the hash taking cache line size into calcualtion to avoid contention. (?)
	// [IDEA] : what about taking worker thread id as hash parameter?
	++m_hashedNumber;
	unsigned num = CalcHashedNumber< TASK_POOL_SIZE >( m_hashedNumber );

	for( unsigned i = 0; i < TASK_POOL_SIZE; ++i )
	{
		unsigned index = ( num + i ) & ( TASK_POOL_SIZE - 1 ); // fast modulo
		if( m_poolMarkers[ index ].Exchange( 1, MemoryOrder::Acquire ) == 0 )
		{
			// We have available task:
			return TaskHandle( &m_taskPool[ index ] );
		}
	}

	return INVALID_TASK_HANDLE;
}

////////////////////////////////////////////////////
void TaskAllocator::ReleaseTask( TaskHandle& task_handle )
{
	ASSERT( task_handle != INVALID_TASK_HANDLE );

	// Make task available to others.
	task_handle.m_task->Clear();

	// Calculate pool marker as difference between pointers:
	size_t pool_marker = task_handle.m_task - m_taskPool;
	m_poolMarkers[ pool_marker ].Store( 0, MemoryOrder::Release ); 

	// Invalidate pointer to avoid using released task!
	task_handle.Invalidate();
}

////////////////////////////////////////////////////
void TaskAllocator::ReleaseAllTasks()
{
	for( unsigned i = 0; i < TASK_POOL_SIZE; ++i )
	{
		m_taskPool[ i ].Clear();
		m_poolMarkers[ i ].Store( 0, MemoryOrder::Relaxed );
	}
}

////////////////////////////////////////////////////
bool TaskAllocator::AreAllTasksReleased() const
{
	for( unsigned i = 0; i < TASK_POOL_SIZE; ++i )
	{
		if( m_poolMarkers[ i ].Load( MemoryOrder::Relaxed ) == 1 )
			return false;
	}

	return true;
}

////////////////////////////////////////////////////
unsigned TaskAllocator::GetTaskPoolSize()
{
	return TASK_POOL_SIZE;
}

////////////////////////////////////////////////////////
bool TaskAllocator::Debug_TryToReleaseTask( unsigned index )
{
	// [NOTE]: i am not clearing task here!
	unsigned expected = 1;
	return m_poolMarkers[ index ].CompareExchange( expected, 0 );
}

////////////////////////////////////////////////////////
bool TaskAllocator::Debug_IsTaskOccupied( const TaskHandle& handle )
{
	size_t pool_marker = handle.m_task - m_taskPool;
	return m_poolMarkers[ pool_marker ] == 1;
}


NAMESPACE_STS_END