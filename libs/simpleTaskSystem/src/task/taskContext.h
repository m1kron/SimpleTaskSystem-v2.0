#pragma once
#include "..\..\include\iTaskContext.h"

NAMESPACE_STS_BEGIN

class ITaskManager;
class Task;

// Implementation of ITaskContext.
class TaskContext : public ITaskContext
{
public:
	TaskContext( ITaskManager* manager, Task* running_task );

	// ITaskContext interface:
	ITaskManager* GetTaskManager() const override;
	size_t GetThisTaskStorageSize() const override;
	void* GetThisTaskStorage() const override;
	// ---

	// Wait until given condition is satisfied, blocks exeution of this task.
	//template< class TCondtion > void WaitFor( const TCondtion& condition ) const;

private:
	ITaskManager* m_taskManager;
	Task* m_task;
};

///////////////////////////////////////////////////////////
//
// INLINES:
//
///////////////////////////////////////////////////////////

/////////////////////////////////////////////////////
inline TaskContext::TaskContext( ITaskManager* manager, Task* this_task )
	: m_taskManager( manager )
	, m_task( this_task )
{
}

///////////////////////////////////////////////////////
//template< class TCondtion >
//inline void TaskContext::WaitFor( const TCondtion& condition ) const
//{
//	m_taskManager.RunTasksUsingThisThreadUntil( condition );
//}


NAMESPACE_STS_END