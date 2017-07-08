#pragma once
#include "..\..\..\include\iTaskHandle.h"

NAMESPACE_STS_BEGIN

class Task;

// Implementation of ITaskHandle interface.
class TaskHandle : public ITaskHandle
{
	friend class TaskAllocator;
public:
	TaskHandle();

	static const TaskHandle* AsTaskHandle( const ITaskHandle* handle );

	// Move ctor:
	TaskHandle( TaskHandle&& other_task ) = delete;
	TaskHandle& operator=( TaskHandle&& other ) = delete;

	// Cannot copy:
	TaskHandle( TaskHandle& ) = delete;
	TaskHandle& operator=( TaskHandle& other ) = delete;

	// Get associated task.
	Task* GetTask() const;

	// ITaskHandle interface:
	void SetTaskFunction( TTaskFunctionPtr function ) const override;
	void AddDependants( const ITaskHandle* dependant1, const ITaskHandle* dependant2, const ITaskHandle* dependant3 ) const override;
	bool IsFinished() const override;
	uint32_t GetTaskStorageSize() const override;
	void* GetTaskStorage() const override;
	// ---
private:
	// Associates task with this handle.
	void AssociateTask( Task* task );

	Task* m_task;
};

////////////////////////////////////////////////////////
//
// INLINES:
//
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
inline TaskHandle::TaskHandle()
	: m_task( nullptr )
{
}

////////////////////////////////////////////////////////
inline const TaskHandle* TaskHandle::AsTaskHandle( const ITaskHandle* handle )
{
	return static_cast< const TaskHandle* >( handle );
}

////////////////////////////////////////////////////////
inline void TaskHandle::AssociateTask( Task* task )
{
	m_task = task;
}

////////////////////////////////////////////////////////
inline Task* TaskHandle::GetTask() const
{
	return m_task;
}

NAMESPACE_STS_END