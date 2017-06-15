#pragma once
#include <sts\private_headers\common\NamespaceMacros.h>
#include <sts\tasking\TaskHandle.h>

NAMESPACE_STS_BEGIN

class TaskManager;
class Task;

/////////////////////////////////////////////////////////
// Stores context of given task execution.
class TaskContext
{
public:
	TaskContext( TaskManager& manager, Task* this_task );

	// Returns this task.
	const TaskHandle& GetThisTask() const;

	// Returns task manager.
	TaskManager& GetTaskManager();

	// Wait until given condition is satisfied, blocks exeution of this task.
	template< class TCondtion > void WaitFor( const TCondtion& condition ) const;

private:
	TaskManager& m_taskManager;
	TaskHandle m_thisTaskHandle;
};

///////////////////////////////////////////////////////////
//
// INLINES:
//
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
inline TaskContext::TaskContext( TaskManager& manager, Task* this_task )
	: m_taskManager( manager )
	, m_thisTaskHandle( this_task )
{
}

///////////////////////////////////////////////////////
inline const TaskHandle& TaskContext::GetThisTask() const
{
	return m_thisTaskHandle;
}

///////////////////////////////////////////////////////
inline TaskManager& TaskContext::GetTaskManager()
{
	return m_taskManager;
}

///////////////////////////////////////////////////////
template< class TCondtion >
inline void TaskContext::WaitFor( const TCondtion& condition ) const
{
	m_taskManager.RunTasksUsingThisThreadUntil( condition );
}


NAMESPACE_STS_END