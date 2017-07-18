#pragma once
#include "..\..\common\taskWorkerInstance\taskWorkerInstance.h"
#include "..\..\..\..\basicThreadingLib\include\synchro\manualResetEvent.h"
#include "..\..\..\..\basicThreadingLib\include\thread\Thread.h"

NAMESPACE_STS_BEGIN

//////////////////////////////////////////////////////////
// Task worker thread. This is a nest for single task worker instance.
class TaskWorkerThread : public btl::ThreadBase
{
	friend class TaskWorkerFiber;
public:
	TaskWorkerThread();

	TaskWorkerThread( TaskWorkerThread&& other ) = delete;
	TaskWorkerThread( const TaskWorkerThread& ) = delete;
	TaskWorkerThread& operator=( const TaskWorkerThread& ) = delete;

	// Starts thread ( and initializes worker instance ). Returns true if success.
	bool Start( const common::TaskWorkerInstanceContext& context );

	// Signals to stop work.
	void FinishWork();

	// Returns true if thread has finish it's thread function.
	bool HasFinishedWork() const;

	// Wake ups thread;
	void WakeUp();

	// Returns worker instance.
	common::TaskWorkerInstance* GetWorkerInstance();

private:
	// Main thread function.
	void ThreadFunction() override;
	void MainWorkerThreadLoop();

	common::TaskWorkerInstance m_workerInstance;
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
inline TaskWorkerThread::TaskWorkerThread()
	: m_shouldFinishWork( false )
	, m_hasFinishWork( false )
{
}

////////////////////////////////////////////////////////
inline bool TaskWorkerThread::Start( const common::TaskWorkerInstanceContext& context )
{
	if( m_workerInstance.Initalize( context ) )
	{
		StartThread();
		return true;
	}
	return false;
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
inline common::TaskWorkerInstance* TaskWorkerThread::GetWorkerInstance()
{
	return &m_workerInstance;
}

NAMESPACE_STS_END