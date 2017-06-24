#pragma once
#include "..\..\..\basicThreadingLib\include\thread\fiber.h"

NAMESPACE_STS_BEGIN

class Task;
class ITaskManager;

// Describes TaskWorkerFiver
enum class TaskFiberState
{
	Running,
	Suspended,
	Idle
};

BTL_ALIGNED( BTL_CACHE_LINE_SIZE ) class TaskFiber : public btl::FiberBase
{
	BASE_CLASS( FiberBase );
public:
	TaskFiber();

	// Setups fiber. Manager is needed to run a task. When task is done, 
	// fiber will switch to parentFiberID.
	void Setup( btl::FIBER_ID parent_fiber_id, ITaskManager* manager );

	// Sets task to execute by fiber.
	void SetTaskToExecute( Task* task );

	// Returns task that this fiber is taking care of.
	Task* GetTask() const;

	// Returns current state of this fiber.
	TaskFiberState GetCurrentState() const;

	// Resets fiber state.
	void Reset();

	// Suspends execution of the task.
	void SuspendExecution();

private:
	// Performs switch to parentFiber
	void SwitchToParentFiber();

	// Main fiber function.
	void FiberFunction();

	TaskFiberState m_state;
	Task* m_taskToExecute;
	ITaskManager* m_taskManager;
	btl::FIBER_ID m_parentFiberID;
};

////////////////////////////////////////////////////////////
//
// INLINES:
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
inline void TaskFiber::Setup( btl::FIBER_ID parent_fiber_id, ITaskManager* manager )
{
	m_parentFiberID = parent_fiber_id;
	m_taskManager = manager;
}

////////////////////////////////////////////////////////////
inline void TaskFiber::SetTaskToExecute( Task* task )
{
	m_taskToExecute = task;
}

////////////////////////////////////////////////////////////
inline Task* TaskFiber::GetTask() const
{
	return m_taskToExecute;
}

////////////////////////////////////////////////////////////
inline TaskFiberState TaskFiber::GetCurrentState() const
{
	return m_state;
}

///////////////////////////////////////////////////////////
inline void TaskFiber::SwitchToParentFiber()
{
	btl::this_fiber::SwitchToFiber( m_parentFiberID );
}

NAMESPACE_STS_END