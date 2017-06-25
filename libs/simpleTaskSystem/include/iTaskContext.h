#pragma once

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
	virtual size_t GetThisTaskStorageSize() const = 0;

	// Returns pointer to the storage - see information on TaskHandle::GetTaskStorage().
	virtual void* GetThisTaskStorage() const = 0;

	// Wait until given condition is satisfied, blocks exeution of this task.
	template< class TCondtion > void WaitFor( const TCondtion& condition ) const;

protected:
	// Called when condition is not yet satisfied.
	virtual void SuspendExecution() const = 0;

	virtual ~ITaskContext() = 0;
};

///////////////////////////////////////////////////////////////////////////
//
// INLINES:
//
///////////////////////////////////////////////////////////////////////////

template<class TCondtion>
inline void ITaskContext::WaitFor( const TCondtion& condition ) const
{
	while( !condition() )
		SuspendExecution();
}

}