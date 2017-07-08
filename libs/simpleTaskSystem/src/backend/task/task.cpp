#include "precompiledHeader.h"
#include "task.h"
#include "taskAllocator.h"
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
bool Task::UpdateDependecies( TaskAllocator* allocator, TReadyToBeExecutedArray& out_ready_to_be_executed )
{
	bool return_val = false;
// ----------------------------------------------------------------------------------------------
// Marco updates dependant tasks. If any of the dependant task became ready to be executed,
// it will be added to out_ready_to_be_executed. Thanks to that, first  worker instance which makes dependant
// task ready to be executed, will be allowed to submit it to execution.
#define CHECK_DEPS( idx, dep_list, allocator, out_ret_val, out_ready_to_be_executed )			\
	if( dep_list[ idx ] != INVALID_TASK_ID )													\
	{																							\
		Task* dependant = allocator->TaskIDToTask( dep_list[ idx ] );							\
		TASK_LOG( "Updating dependencies of dependant task< %i >.", , dependant->GetTaskID() );	\
		uint32_t parent_dependencies = dependant->m_numberOfChildTasks.Decrement();				\
		if( parent_dependencies == 1 )															\
		{																						\
			out_ready_to_be_executed[ idx ] = dependant;										\
			out_ret_val = true;																	\
		}																						\
	}
// ----------------------------------------------------------------------------------------------

	CHECK_DEPS( 0, m_dependants, allocator, return_val, out_ready_to_be_executed );
	CHECK_DEPS( 1, m_dependants, allocator, return_val, out_ready_to_be_executed );
	CHECK_DEPS( 2, m_dependants, allocator, return_val, out_ready_to_be_executed );

	return return_val;

#undef CHECK_DEPS
}

////////////////////////////////////////////////////////
void Task::AddDependantTasks( Task* dependant1, Task* dependant2, Task* dependant3 )
{
// -----------------------------------------------------
#define SETUP_DEPS( dependant, idx, dep_array  )		\
	if( dependant )										\
	{													\
		ASSERT( dep_array[ idx ] == INVALID_TASK_ID );	\
		dep_array[ idx ] = dependant->GetTaskID();		\
		dependant->m_numberOfChildTasks.Increment();	\
	}
// -----------------------------------------------------

	SETUP_DEPS( dependant1, 0, m_dependants );
	SETUP_DEPS( dependant2, 1, m_dependants );
	SETUP_DEPS( dependant3, 2, m_dependants );

#undef SETUP_DEPS
}

NAMESPACE_STS_END