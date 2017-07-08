#pragma once
#include "..\..\..\include\iTaskHandle.h"
#include "..\..\..\..\basicThreadingLib\include\atomic\atomic.h"

NAMESPACE_STS_BEGIN

class ITaskContext;
class TaskAllocator;

typedef uint16_t TASK_ID;
#define INVALID_TASK_ID 0xFFFF

typedef std::array< class Task*, 3 > TReadyToBeExecutedArray;

/////////////////////////////////////////////////////////
// Task respresent basic unit of execution in the system.
BTL_ALIGNED( BTL_CACHE_LINE_SIZE ) class Task
{
public:

	// Default ctor.
	Task();

	// Returns size of the storage of the task.
	// TODO: constexpr..
	static const uint32_t GetStorageSize();

	// Main task function called by task worker.
	void Run( ITaskContext* context );

	// Returns true if task is finished ( execution of task function is over ).
	bool IsFinished() const; 

	// Returns true if task can be executed right now.
	bool IsReadyToBeExecuted() const;

	// Adds dependant tasks, all will be executed after this task is done.
	void AddDependantTasks( Task* dependant1, Task* dependant2, Task* dependant3 );

	// Set main task function.
	void SetTaskFunction( TTaskFunctionPtr function );

	// Returns storage pointer.
	void* GetStoragePtr();

	// Updates dependencies( parent_task ). Returns parent task if it is ready to
	// be executed, nullptr otherwise.
	bool UpdateDependecies( TaskAllocator* allocator, TReadyToBeExecutedArray& out_ready_to_be_executed );

	// Clears task.
	void Clear();

	// Returns task id.
	TASK_ID GetTaskID() const;

	// Max size of data that can be stored by task instance.
	static const size_t STORAGE_SIZE = ( BTL_CACHE_LINE_SIZE - sizeof( TTaskFunctionPtr ) - sizeof( TASK_ID ) * 4 - sizeof( btl::Atomic< uint32_t > ) );

private:
	typedef TASK_ID TDependantTasksArray[ 3 ];
	friend class TaskAllocator;

	// Setups this task.
	void Setup( TASK_ID id );

	TTaskFunctionPtr m_functionPtr;
	TDependantTasksArray m_dependants;
	TASK_ID m_id;
	btl::Atomic< uint32_t > m_numberOfChildTasks; //< When 0, task is considered as finished.

	char m_storage[ STORAGE_SIZE ];
};

///////////////////////////////////////////////////////////////
//
// INLINES:
//
///////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
inline bool Task::IsFinished() const
{
	return m_numberOfChildTasks.Load( btl::MemoryOrder::Acquire ) == 0;
}

////////////////////////////////////////////////////////
inline bool Task::IsReadyToBeExecuted() const
{
	return m_numberOfChildTasks.Load( btl::MemoryOrder::Acquire ) == 1;
}

////////////////////////////////////////////////////////
inline void Task::SetTaskFunction( TTaskFunctionPtr function )
{
	ASSERT( function != nullptr );
	ASSERT( m_functionPtr == nullptr );

	m_functionPtr = function;
	m_numberOfChildTasks.Increment(); //< this task is dependent task.
}

////////////////////////////////////////////////////////
inline void Task::Setup( TASK_ID id )
{
	ASSERT( id != INVALID_TASK_ID );
	m_id = id;
}

////////////////////////////////////////////////////////
inline void* Task::GetStoragePtr()
{
	return m_storage;
}

////////////////////////////////////////////////////////
inline void Task::Clear()
{
	ASSERT( m_numberOfChildTasks.Load( btl::MemoryOrder::Relaxed ) == 0 );

	m_functionPtr = nullptr;
	m_dependants[ 0 ] = INVALID_TASK_ID;
	m_dependants[ 1 ] = INVALID_TASK_ID;
	m_dependants[ 2 ] = INVALID_TASK_ID;
	m_numberOfChildTasks.Store( 0, btl::MemoryOrder::Release );
}

////////////////////////////////////////////////////////
inline TASK_ID Task::GetTaskID() const
{
	return m_id; 
}

NAMESPACE_STS_END