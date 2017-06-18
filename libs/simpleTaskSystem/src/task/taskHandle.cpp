#include "precompiledHeader.h"
#include "taskHandle.h"
#include "task.h"

NAMESPACE_STS_BEGIN

// Implementation of dtor of ITaskHandle.
ITaskHandle::~ITaskHandle() {}

///////////////////////////////////////////////////////////////////
void TaskHandle::SetTaskFunction( TTaskFunctionPtr function ) const
{
	m_task->SetTaskFunction( function );
}

///////////////////////////////////////////////////////////////////
void TaskHandle::AddParent( const ITaskHandle* parentTaskHandle ) const
{
	m_task->AddParent( static_cast< const TaskHandle* >( parentTaskHandle )->m_task );
}

///////////////////////////////////////////////////////////////////
bool TaskHandle::IsFinished() const
{
	return m_task->IsFinished();
}

///////////////////////////////////////////////////////////////////
size_t TaskHandle::GetTaskStorageSize() const
{
	return m_task->GetStorageSize();
}

///////////////////////////////////////////////////////////////////
void * TaskHandle::GetTaskStorage() const
{
	return m_task->GetStoragePtr();
}

NAMESPACE_STS_END