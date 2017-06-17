#pragma once
#include "taskContext.h"
#include "..\..\..\commonLib\include\tools\existingBufferWrapper.h"
#include "..\..\..\basicThreadingLib\include\atomic\atomic.h"

NAMESPACE_STS_BEGIN

class TaskManager;

/////////////////////////////////////////////////////////
// Task respresent basic unit of execution in the system.
BTL_ALIGNED( BTL_CACHE_LINE_SIZE ) class Task
{
public:
	// Task function archetype.
	typedef void( *TFunctionPtr ) ( TaskContext& task_context );

	// Default ctor.
	Task();

	// Return max size of raw data, that task can hold. If you need more, you have to allocated it on your own.
	// TODO: constexpr..
	static const size_t GetDataSize();

	// Main task function called by task worker.
	void Run( TaskManager* task_manager );

	// Returns true if task is finished ( execution of task function is over ).
	bool IsFinished() const; 

	// Returns true if task can be executed right now.
	bool IsReadyToBeExecuted() const;

	// Marks this task as a child of parent task. Parent task will be
	// executed after all child tasks are done.
	void AddParent( const TaskHandle& parentTask );

	// Set main task function.
	void SetTaskFunction( TFunctionPtr function );

	// Returns raw task data pointer.
	void* GetRawDataPtr();

	// Clears task.
	void Clear();

	// Max size of data that can be stored by task instance.
	static const size_t DATA_SIZE = ( BTL_CACHE_LINE_SIZE - sizeof( TFunctionPtr ) - sizeof( Task* ) - sizeof( btl::Atomic< uint32_t > ) );

private:
	TFunctionPtr m_functionPtr; 
	Task* m_parentTask;
	btl::Atomic< uint32_t > m_numberOfChildTasks; //< When 0, task is considered as finished.

	char m_data[ DATA_SIZE ];
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
inline void Task::SetTaskFunction( TFunctionPtr function )
{
	ASSERT( function != nullptr );
	ASSERT( m_functionPtr == nullptr );

	m_functionPtr = function;
	m_numberOfChildTasks.Increment(); //< this task is dependent task.
}

////////////////////////////////////////////////////////
inline void Task::AddParent( const TaskHandle& parentTask )
{
	ASSERT( parentTask != INVALID_TASK_HANDLE );
	ASSERT( m_parentTask == nullptr );

	m_parentTask = parentTask.m_task;
	m_parentTask->m_numberOfChildTasks.Increment();
}

////////////////////////////////////////////////////////
inline void* Task::GetRawDataPtr()
{
	return m_data;
}

////////////////////////////////////////////////////////
inline void Task::Clear()
{
	ASSERT( m_numberOfChildTasks.Load( btl::MemoryOrder::Relaxed ) == 0 );

	m_functionPtr = nullptr;
	m_parentTask = nullptr;
	m_numberOfChildTasks.Store( 0, btl::MemoryOrder::Release );
}

NAMESPACE_STS_END