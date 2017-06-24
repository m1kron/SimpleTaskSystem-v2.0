#include "precompiledHeader.h"
#include "taskContext.h"
#include "task.h"
#include "..\taskFiber\taskFiber.h"

NAMESPACE_STS_BEGIN

// Implementation of dtor of ITaskContext.
ITaskContext::~ITaskContext() {}

//////////////////////////////////////////////////////
size_t TaskContext::GetThisTaskStorageSize() const
{
	return m_taskFiber->GetTask()->GetStorageSize();
}

//////////////////////////////////////////////////////
void* TaskContext::GetThisTaskStorage() const
{
	return m_taskFiber->GetTask()->GetStoragePtr();
}

NAMESPACE_STS_END