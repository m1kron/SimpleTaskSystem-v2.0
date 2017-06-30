#include "precompiledHeader.h"
#include "taskContext.h"
#include "task.h"
#include "..\taskFiber\taskFiber.h"

NAMESPACE_STS_BEGIN

// Implementation of dtor of ITaskContext.
ITaskContext::~ITaskContext() {}

//////////////////////////////////////////////////////
uint32_t TaskContext::GetThisTaskStorageSize() const
{
	return m_taskFiber->GetTask()->GetStorageSize();
}

//////////////////////////////////////////////////////
void* TaskContext::GetThisTaskStorage() const
{
	return m_taskFiber->GetTask()->GetStoragePtr();
}

//////////////////////////////////////////////////////
void TaskContext::SuspendExecution() const
{
	m_taskFiber->SuspendExecution();
}

NAMESPACE_STS_END