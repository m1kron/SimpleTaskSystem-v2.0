#include "precompiledHeader.h"
#include "taskContext.h"
#include "task.h"

NAMESPACE_STS_BEGIN

// Implementation of dtor of ITaskContext.
ITaskContext::~ITaskContext() {}

//////////////////////////////////////////////////////
ITaskManager* TaskContext::GetTaskManager() const
{
	return m_taskManager;
}

//////////////////////////////////////////////////////
size_t TaskContext::GetThisTaskStorageSize() const
{
	return m_task->GetStorageSize();
}

//////////////////////////////////////////////////////
void* TaskContext::GetThisTaskStorage() const
{
	return m_task->GetStoragePtr();
}

NAMESPACE_STS_END