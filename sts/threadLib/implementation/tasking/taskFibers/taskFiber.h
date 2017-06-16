#pragma once
#include <sts\private_headers\common\NamespaceMacros.h>
#include <sts\lowlevel\thread\Fiber.h>

NAMESPACE_STS_BEGIN

class Task;
class TaskManager;

// Describes TaskWorkerFiver
enum class TaskFiberState
{
	Running,
	Suspended,
	Idle
};

STS_ALIGNED( STS_CACHE_LINE_SIZE ) class TaskFiber : public FiberBase
{
	BASE_CLASS( FiberBase );
public:
	TaskFiber();

	// Setups fiber. Manager is needed to run a task. When task is done, 
	// fiber will switch to prevFiberID.
	void Setup( sts::FIBER_ID prevFiberID, sts::TaskManager* manager );

	// Sets task to execute by fiber.
	void SetTaskToExecute( Task* task );

	// Returns current state of the fiber.
	TaskFiberState GetCurrentState() const;

	// Resets fiber state.
	void Reset();

private:
	// Main fiber function.
	void FiberFunction();

	TaskFiberState m_state;
	sts::Task* m_taskToExecute;
	sts::TaskManager* m_taskManager;
	sts::FIBER_ID m_prevFiberID;
};

////////////////////////////////////////////////////////////
//
// INLINES:
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
inline void TaskFiber::Setup( sts::FIBER_ID prevFiberID, sts::TaskManager* manager )
{
	m_prevFiberID = prevFiberID;
	m_taskManager = manager;
}

////////////////////////////////////////////////////////////
inline void TaskFiber::SetTaskToExecute( Task* task )
{
	m_taskToExecute = task;
}

////////////////////////////////////////////////////////////
inline TaskFiberState TaskFiber::GetCurrentState() const
{
	return m_state;
}

NAMESPACE_STS_END