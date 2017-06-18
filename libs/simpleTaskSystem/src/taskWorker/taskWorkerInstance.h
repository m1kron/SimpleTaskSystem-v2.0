#pragma once
#include "taskWorkerInstanceContext.h"
#include "..\structures\lockbased\lockBasedPtrQueue.h"
#include "..\..\..\basicThreadingLib\include\thread\thisFiberHelpers.h"

NAMESPACE_STS_BEGIN

class Task;

// Class represents TaskWorkerInstance, which is responsible for logic related to running task.
// Implements every thing that is connected to that ( including stealing, fiber managment etc ).
// Note that this class is used by task worker thread, as well as task manager to allow running 
// tasks from main thread.
class TaskWorkerInstance
{
public:
	TaskWorkerInstance();
	~TaskWorkerInstance();

	// Initlalizes. Returns true if success.
	bool Initalize( const TaskWorkerInstanceContext& context );

	// Initlalizes. Returns true if success.
	bool Deinitalize();

	// Converts thread to fiber. Returns true if success.
	bool ConvertToFiber();

	// Returns true if thread is already converted to fiber.
	bool IsConvertedToFiber() const;

	// Converts fiber back to thread. Returns true if success.
	bool ConvertToThread();

	// Setups given fiber.
	void SetupFiber( TaskFiber* fiber );

	// Releases fiber.
	void ReleaseFiber( TaskFiber* fiber );

	// Adds task to pending execution queue. Returns true if success.
	bool AddTask( Task* task );

	// Tries to execute one task - first from pending queue. If queue is empty, then tries to steal task
	// from other worker instances. Returns true if any task was executed.
	bool TryToExecuteSingleTask();

	// Tries to steal a task from this worker instance.
	Task* TryToStealTaskFromThisInstance();

private:
	// Tries to steal a task from other worker instances.
	Task* TryToStealTaskFromOtherInstances();

	LockBasedPtrQueue< Task, TASK_POOL_SIZE / 2 > m_pendingTaskQueue;	//< Using lock based queue, cuz has support to popBack() and have similar performance to lock free in avg case.
	TaskWorkerInstanceContext m_context;
	TaskFiber* m_currentFiber;
	btl::FIBER_ID m_thisFiberID;
};

//////////////////////////////////////////////////////////////////////////////
//
// INLINES:
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
inline TaskWorkerInstance::TaskWorkerInstance()
	: m_currentFiber( nullptr )
	, m_thisFiberID( INVALID_FIBER_ID )
{
}

//////////////////////////////////////////////////////////////////////////////////
inline TaskWorkerInstance::~TaskWorkerInstance()
{
}

//////////////////////////////////////////////////////////////////////////////////
inline bool TaskWorkerInstance::Deinitalize()
{
	ReleaseFiber( m_currentFiber );
	return true;
}

//////////////////////////////////////////////////////////////////////////////////
inline bool TaskWorkerInstance::IsConvertedToFiber() const
{
	return m_thisFiberID != INVALID_FIBER_ID;
}

//////////////////////////////////////////////////////////////////////////////////
inline bool TaskWorkerInstance::AddTask( Task* task )
{
	return m_pendingTaskQueue.PushBack( task );
}

//////////////////////////////////////////////////////////////////////////////////
inline Task* TaskWorkerInstance::TryToStealTaskFromThisInstance()
{
	return m_pendingTaskQueue.PopFront();
}

NAMESPACE_STS_END