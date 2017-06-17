#pragma once
#include "..\structures\lockbased\lockBasedPtrQueue.h"
#include "..\..\..\basicThreadingLib\include\synchro\manualResetEvent.h"
#include "..\..\..\basicThreadingLib\include\thread\Thread.h"
#include "..\..\..\basicThreadingLib\include\thread\thisFiberHelpers.h"

NAMESPACE_STS_BEGIN

class Task;
class TaskWorkersPool;
class TaskManager;
class TaskFiberAllocator;
class TaskFiber;

// Contains necessary info for task worker thread.
struct TaskWorkerContext
{
	TaskWorkersPool* m_workersPool;
	TaskManager* m_taskManager;
	TaskFiberAllocator* m_fiberAllocator;
	unsigned m_poolIndex;
};

//////////////////////////////////////////////////////////
// Task worker thread. This class is responsible for scheduling fibers
// ( and tasks assigned to it ).
class TaskWorkerThread : public btl::ThreadBase
{
	friend class TaskWorkerFiber;
public:
	TaskWorkerThread( const TaskWorkerContext& context );

	TaskWorkerThread( TaskWorkerThread&& other ) = delete;
	TaskWorkerThread( const TaskWorkerThread& ) = delete;
	TaskWorkerThread& operator=( const TaskWorkerThread& ) = delete;

	// Adds task to worker's queue. Returns true if success.
	bool AddTask( Task* task );

	// Signals to stop work.
	void FinishWork();

	// Returns true if thread has finish it's thread function.
	bool HasFinishedWork() const;

	// Wake ups thread;
	void WakeUp();

	// Tries to steal task from this worker's queue. Returns nullptr if failed.
	Task* TryToStealTask();
private:
	// Main thread function.
	void ThreadFunction() override;
	void MainWorkerThreadLoop();

	// Loops through all other workers and tries to steal a task from them.
	Task* StealTaskFromOtherWorkers();

	LockBasedPtrQueue< Task, TASK_POOL_SIZE / 2 > m_pendingTaskQueue;	//< Using lock based queue, cuz has support to popBack() and have similar performance to lock free in avg case.
	TaskWorkerContext m_context;
	TaskFiber* m_currentFiber;
	btl::FIBER_ID m_thisWorkerFiberID;
	btl::ManualResetEvent m_hasWorkToDoEvent;
	bool m_shouldFinishWork;
	bool m_hasFinishWork;
};

////////////////////////////////////////////////////////////////
//
// INLINES:
//
////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
inline bool TaskWorkerThread::AddTask( Task* task )
{
	// Add new task to local queue.
	return m_pendingTaskQueue.PushBack( task );
}

////////////////////////////////////////////////////////
inline void TaskWorkerThread::FinishWork()
{
	m_shouldFinishWork = true;
	WakeUp();
}

////////////////////////////////////////////////////////
inline bool TaskWorkerThread::HasFinishedWork() const
{
	return m_hasFinishWork;
}

////////////////////////////////////////////////////////
inline void TaskWorkerThread::WakeUp()
{
	m_hasWorkToDoEvent.SetEvent();
}

////////////////////////////////////////////////////////
inline Task* TaskWorkerThread::TryToStealTask()
{
	return m_pendingTaskQueue.PopFront();
}

NAMESPACE_STS_END