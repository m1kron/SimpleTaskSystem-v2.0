#pragma once
#include <sts\private_headers\common\NamespaceMacros.h>

NAMESPACE_STS_BEGIN

/////////////////////////////////////////////////////////////
// Handle, that holds entry in pool and allows to release slot.
class TaskHandle
{
	friend class TaskAllocator;
	friend class TaskManager;
	friend class Task;
	friend class TaskContext;
public:
	TaskHandle();

	// Move ctor:
	TaskHandle( TaskHandle&& other_task );
	TaskHandle& operator=( TaskHandle&& other );

	// Cannot copy:
	TaskHandle( TaskHandle& ) = delete;
	TaskHandle& operator=( TaskHandle& other ) = delete;

	// Comparsion operators:
	bool operator==( const TaskHandle& other ) const;
	bool operator!=( const TaskHandle& other ) const;

	// Class member access operator.
	Task* operator->( ) const;//rethink this.

	// Makes this handle invalid.
	void Invalidate();

private:
	TaskHandle( Task* task );

	Task* m_task;
};

#define INVALID_TASK_HANDLE TaskHandle()


////////////////////////////////////////////////////////
//
// INLINES:
//
////////////////////////////////////////////////////////
inline TaskHandle::TaskHandle( Task* task )
	: m_task( task )
{
}

////////////////////////////////////////////////////////
inline TaskHandle::TaskHandle()
	: m_task( nullptr )
{
}

////////////////////////////////////////////////////////
inline TaskHandle::TaskHandle( TaskHandle&& other_task )
	: m_task( other_task.m_task )
{
	other_task.Invalidate();
}

////////////////////////////////////////////////////////
inline Task* TaskHandle::operator->( ) const
{
	return m_task;
}

////////////////////////////////////////////////////////s
inline TaskHandle& TaskHandle::operator=( TaskHandle&& other )
{
	m_task = other.m_task;
	other.Invalidate();

	return *this;
}

////////////////////////////////////////////////////////s
inline bool TaskHandle::operator==( const TaskHandle& other ) const
{
	return m_task == other.m_task;
}

////////////////////////////////////////////////////////s
inline bool TaskHandle::operator!=( const TaskHandle& other ) const
{
	return m_task != other.m_task;
}

////////////////////////////////////////////////////////
inline void TaskHandle::Invalidate()
{
	m_task = nullptr;
}


NAMESPACE_STS_END