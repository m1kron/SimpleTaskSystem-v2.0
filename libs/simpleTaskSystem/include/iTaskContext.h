#pragma once
#include "sts_types.h"

namespace sts
{

class ITaskSystem;

// Represents context of given task DURING ITS EXECUTION.
class ITaskContext
{
public:
	// Returns task manager.
	virtual ITaskSystem* GetTaskSystem() const = 0;

	// Returns size of task storage - see information on TaskHandle::GetTaskStorageSize().
	virtual uint32_t GetThisTaskStorageSize() const = 0;

	// Returns pointer to the storage - see information on TaskHandle::GetTaskStorage().
	virtual void* GetThisTaskStorage() const = 0;

protected:
	virtual ~ITaskContext() = 0;
};

}