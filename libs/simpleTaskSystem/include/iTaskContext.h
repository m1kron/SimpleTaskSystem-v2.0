#pragma once

namespace sts
{

class ITaskManager;

// Represents context of given task DURING ITS EXECUTION.
class ITaskContext
{
public:
	// Returns task manager.
	virtual ITaskManager* GetTaskManager() const = 0;

	// Returns size of task storage - see information on TaskHandle::GetTaskStorageSize().
	virtual size_t GetThisTaskStorageSize() const = 0;

	// Returns pointer to the storage - see information on TaskHandle::GetTaskStorage().
	virtual void* GetThisTaskStorage() const = 0;

	// Wait until given condition is satisfied, blocks exeution of this task.
	//template< class TCondtion > void WaitFor( const TCondtion& condition ) const;

protected:
	virtual ~ITaskContext() = 0;
};

}