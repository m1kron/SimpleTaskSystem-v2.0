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
void TaskHandle::AddDependants( const ITaskHandle* dependant1, const ITaskHandle* dependant2, const ITaskHandle* dependant3 ) const
{
	// TODO: if this get bigger either use variadic templates or simple array of ptrs.

	Task* dependant_task1 = dependant1 != nullptr ? AsTaskHandle( dependant1 )->GetTask() : nullptr;
	Task* dependant_task2 = dependant2 != nullptr ? AsTaskHandle( dependant2 )->GetTask() : nullptr;
	Task* dependant_task3 = dependant3 != nullptr ? AsTaskHandle( dependant3 )->GetTask() : nullptr;

	m_task->AddDependantTasks( dependant_task1, dependant_task2, dependant_task3 );
}

///////////////////////////////////////////////////////////////////
bool TaskHandle::IsFinished() const
{
	return m_task->IsFinished();
}

///////////////////////////////////////////////////////////////////
uint32_t TaskHandle::GetTaskStorageSize() const
{
	return m_task->GetStorageSize();
}

///////////////////////////////////////////////////////////////////
void * TaskHandle::GetTaskStorage() const
{
	return m_task->GetStoragePtr();
}

NAMESPACE_STS_END