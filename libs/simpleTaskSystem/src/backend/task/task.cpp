#include "precompiledHeader.h"
#include "task.h"
#include "..\..\..\..\commonLib\include\tools\tools.h"

NAMESPACE_STS_BEGIN

#define TASK_LOG( txt, ... ) LOG("[TASK< %i >]: " txt, GetTaskID() __VA_ARGS__ );

///////////////////////////////////////////////////////
Task::Task()
{
	STATIC_ASSERT( sizeof( Task ) == BTL_CACHE_LINE_SIZE, "Task has to have size of cache line!" );
	ASSERT( IsAligned< BTL_CACHE_LINE_SIZE >( this ) );

	Clear();
}

///////////////////////////////////////////////////////
const uint32_t Task::GetStorageSize()
{
	return STORAGE_SIZE;
}

///////////////////////////////////////////////////////
void Task::Run( ITaskContext* context )
{
	ASSERT( IsReadyToBeExecuted() );

	TASK_LOG( "Started execution." );

	// Execute task function:
	m_functionPtr( context );

	TASK_LOG( "Execution finished." );

	// We are finished, so we can't have any dependant tasks now.
	uint32_t dependent_num = m_numberOfChildTasks.Decrement();
	ASSERT( dependent_num == 0 );
}

///////////////////////////////////////////////////////
Task* Task::UpdateDependecies()
{
	if( m_parentTask )
	{
		TASK_LOG( "Updating dependencies of parent task< %i >.", , m_parentTask->GetTaskID() );

		// Inform parent that we are finished.
		uint32_t parent_dependencies = m_parentTask->m_numberOfChildTasks.Decrement();
		if( parent_dependencies == 1 )
			return m_parentTask; // First thread, which makes parent task ready to be executed, will be allowed to submit it to execution.
	}

	return nullptr;
}

NAMESPACE_STS_END