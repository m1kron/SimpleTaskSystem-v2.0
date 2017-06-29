#pragma once
#include "..\..\include\iTaskHandle.h"
#include "..\..\..\basicThreadingLib\include\atomic\atomic.h"

NAMESPACE_STS_BEGIN

class ITaskContext;

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

	// Marks this task as a child of parent task. Parent task will be
	// executed after all child tasks are done.
	void AddParent( Task* parentTask );

	// Set main task function.
	void SetTaskFunction( TTaskFunctionPtr function );

	// Returns storage pointer.
	void* GetStoragePtr();

	// Updates dependencies( parent_task ). Returns parent task if it is ready to
	// be executed, nullptr otherwise.
	Task* UpdateDependecies();

	// Clears task.
	void Clear();

	// Returns task id.
	uint32_t GetTaskID() const;

	// Max size of data that can be stored by task instance.
	static const size_t STORAGE_SIZE = ( BTL_CACHE_LINE_SIZE - sizeof( TTaskFunctionPtr ) - sizeof( Task* ) - sizeof( btl::Atomic< uint32_t > ) );

private:
	// Returns parent task.
	Task* GetParentTask() const;

	TTaskFunctionPtr m_functionPtr;
	Task* m_parentTask;
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
inline void Task::AddParent( Task* parentTask )
{
	ASSERT( parentTask != nullptr );
	ASSERT( m_parentTask == nullptr );

	m_parentTask = parentTask;
	m_parentTask->m_numberOfChildTasks.Increment();
}

////////////////////////////////////////////////////////
inline Task* Task::GetParentTask() const
{
	return m_parentTask;
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
	m_parentTask = nullptr;
	m_numberOfChildTasks.Store( 0, btl::MemoryOrder::Release );
}

////////////////////////////////////////////////////////
inline uint32_t Task::GetTaskID() const
{
	// Just use memory address.
	uint64_t id = reinterpret_cast< uint64_t > ( this );
	return (uint32_t)id; 
}

NAMESPACE_STS_END