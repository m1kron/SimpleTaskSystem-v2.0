#pragma once
#include "iTaskContext.h"

namespace sts
{

// Archetype of the function that task can run.
typedef void( *TTaskFunctionPtr ) ( const ITaskContext* task_context );

// This class represent handle to real task that will be run by the system.
// User can setup the task only via this interface.
class ITaskHandle
{
public:
	// Set main task function.
	virtual void SetTaskFunction( TTaskFunctionPtr function ) const = 0;

	// Specifies dependant task handles of this task handle. All dependant tasks will be executed after this tasks finish execution.
	virtual void AddDependants( const ITaskHandle* dependant1, const ITaskHandle* dependant2 = nullptr, const ITaskHandle* dependant3 = nullptr ) const = 0;

	// Returns true if task is executed.
	virtual bool IsFinished() const = 0;

	// Returns size of task storage - user can put whatever he wants to that storage.
	// Example: user wants to calcualte some big number via the task, and wants to later get it 
	// from the task. He can do that by adding that big number to the storage via task execution 
	// and after the task is done - read it from that storage.
	// Adding to this storage does not required any synchronization.
	// If you need more memory then this storage, you have to allocated it on your own.
	virtual uint32_t GetTaskStorageSize() const = 0;

	// Returns pointer to the storage. WARNING! Do not put more data then GetTaskStorageSize()!
	virtual void* GetTaskStorage() const = 0;

protected:
	virtual ~ITaskHandle() = 0;
};

}