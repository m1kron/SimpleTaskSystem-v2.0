#include <sts\tasking\Task.h>
#include <sts\tasking\TaskManager.h>
#include <commonlib\tools\Tools.h>

NAMESPACE_STS_BEGIN

///////////////////////////////////////////////////////
//
// TASK:
//
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
Task::Task()
{
	STATIC_ASSERT( sizeof( Task ) == STS_CACHE_LINE_SIZE, "Task has to have size of cache line!" );
	ASSERT( IsAligned< STS_CACHE_LINE_SIZE >( this ) );

	Clear();
}

///////////////////////////////////////////////////////
const size_t Task::GetDataSize()
{
	return DATA_SIZE;
}

///////////////////////////////////////////////////////
void Task::Run( TaskManager* task_manager )
{
	ASSERT( IsReadyToBeExecuted() );

	TaskContext taskContext( *task_manager, this );

	// Execute task function:
	m_functionPtr( taskContext );

	// We are finished, so we can't have any dependant tasks now.
	unsigned dependent_num = m_numberOfChildTasks.Decrement();
	ASSERT( dependent_num == 0 );

	if( m_parentTask )
	{
		// Inform parent that we are finished.
		unsigned parent_dependant_task = m_parentTask->m_numberOfChildTasks.Decrement();
		ASSERT( parent_dependant_task > 0 );

		// Parent is ready to be executed, so add it to our thread.
		if( parent_dependant_task == 1 )
		{
			bool submitted = task_manager->SubmitTask( TaskHandle( m_parentTask ) );
			ASSERT( submitted );
		}		
	}
}

NAMESPACE_STS_END