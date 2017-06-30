#pragma once
#include "..\..\..\include\iTaskContext.h"

NAMESPACE_STS_BEGIN

class ITaskSystem;
class TaskFiber;

// Implementation of ITaskContext.
class TaskContext : public ITaskContext
{
public:
	TaskContext( ITaskSystem* system_interface, TaskFiber* task_fiber );

	// ITaskContext interface:
	ITaskSystem* GetTaskSystem() const override;
	uint32_t GetThisTaskStorageSize() const override;
	void* GetThisTaskStorage() const override;
	void SuspendExecution() const override;
	// ---

private:
	ITaskSystem* m_taskSystem;
	TaskFiber* m_taskFiber;
};

///////////////////////////////////////////////////////////
//
// INLINES:
//
///////////////////////////////////////////////////////////

/////////////////////////////////////////////////////
inline TaskContext::TaskContext( ITaskSystem* system_interface, TaskFiber* task_fiber )
	: m_taskSystem( system_interface )
	, m_taskFiber( task_fiber )
{
}

//////////////////////////////////////////////////////
inline ITaskSystem* TaskContext::GetTaskSystem() const
{
	return m_taskSystem;
}

NAMESPACE_STS_END