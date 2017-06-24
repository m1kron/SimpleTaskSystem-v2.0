#pragma once
#include "..\..\include\iTaskContext.h"

NAMESPACE_STS_BEGIN

class ITaskManager;
class TaskFiber;

// Implementation of ITaskContext.
class TaskContext : public ITaskContext
{
public:
	TaskContext( ITaskManager* manager, TaskFiber* task_fiber );

	// ITaskContext interface:
	ITaskManager* GetTaskManager() const override;
	size_t GetThisTaskStorageSize() const override;
	void* GetThisTaskStorage() const override;
	void SuspendExecution() const override;
	// ---

private:
	ITaskManager* m_taskManager;
	TaskFiber* m_taskFiber;
};

///////////////////////////////////////////////////////////
//
// INLINES:
//
///////////////////////////////////////////////////////////

/////////////////////////////////////////////////////
inline TaskContext::TaskContext( ITaskManager* manager, TaskFiber* task_fiber )
	: m_taskManager( manager )
	, m_taskFiber( task_fiber )
{
}

//////////////////////////////////////////////////////
inline ITaskManager* TaskContext::GetTaskManager() const
{
	return m_taskManager;
}

NAMESPACE_STS_END