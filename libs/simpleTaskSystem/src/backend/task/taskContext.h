#pragma once
#include "..\..\..\include\iTaskContext.h"

NAMESPACE_STS_BEGIN

class ITaskSystem;
class Task;

// Implementation of ITaskContext.
class TaskContext : public ITaskContext
{
public:
	TaskContext( ITaskSystem* system_interface, Task* task );

	// ITaskContext interface:
	ITaskSystem* GetTaskSystem() const override;
	uint32_t GetThisTaskStorageSize() const override;
	void* GetThisTaskStorage() const override;
	// ---

private:
	ITaskSystem* m_taskSystem;
	Task* m_task;
};

///////////////////////////////////////////////////////////
//
// INLINES:
//
///////////////////////////////////////////////////////////

/////////////////////////////////////////////////////
inline TaskContext::TaskContext( ITaskSystem* system_interface, Task* task )
	: m_taskSystem( system_interface )
	, m_task( task )
{
}

//////////////////////////////////////////////////////
inline ITaskSystem* TaskContext::GetTaskSystem() const
{
	return m_taskSystem;
}

NAMESPACE_STS_END