#include "precompiledHeader.h"
#include "task.h"
#include "..\..\..\commonLib\include\tools\tools.h"

NAMESPACE_STS_BEGIN

///////////////////////////////////////////////////////
//
// TASK:
//
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
Task::Task()
{
	STATIC_ASSERT( sizeof( Task ) == BTL_CACHE_LINE_SIZE, "Task has to have size of cache line!" );
	ASSERT( IsAligned< BTL_CACHE_LINE_SIZE >( this ) );

	Clear();
}

///////////////////////////////////////////////////////
const size_t Task::GetStorageSize()
{
	return STORAGE_SIZE;
}

///////////////////////////////////////////////////////
void Task::Run( ITaskContext* context )
{
	ASSERT( IsReadyToBeExecuted() );

	// Execute task function:
	m_functionPtr( context );

	// We are finished, so we can't have any dependant tasks now.
	uint32_t dependent_num = m_numberOfChildTasks.Decrement();
	ASSERT( dependent_num == 0 );

	if( m_parentTask )
	{
		// Inform parent that we are finished.
		uint32_t parent_dependant_task = m_parentTask->m_numberOfChildTasks.Decrement();
		ASSERT( parent_dependant_task > 0 );		
	}
}

NAMESPACE_STS_END