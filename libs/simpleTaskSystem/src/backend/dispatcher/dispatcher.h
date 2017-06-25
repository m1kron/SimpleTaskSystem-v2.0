#pragma once
#include "..\..\..\..\libs\basicThreadingLib\include\thread\thisThreadHelpers.h"


NAMESPACE_STS_BEGIN

class TaskWorkerInstance;
class Task;

// Class is responsbile for:
// 1) maintain list of available task worker instances
// 2) dispatch task to task worker instances
// 3) dispatch suspended task fibers to task worker instances
// Dispatchers does not owns TaskWorkerInstance(s).
class Dispatcher
{
public:
	Dispatcher();
	~Dispatcher();

	// Registers instance. Returns index at which one will find this instance.
	uint32_t Register( TaskWorkerInstance* instance, bool always_converted );

	// Unregisters all instances.
	void UnregisterAll();

	// Dispatches single task to worker instances.
	bool DispatchTask( Task* task );

	// Returns number of registered instances.
	uint32_t GetRegisteredInstancesCount() const;

private:
	friend class TaskWorkerInstance;

	// Try to steal a task from other worker instances then the one with requesting_worker_instance_id.
	// Returns stealed task if success, nullptr otherwise.
	Task* TryToStealTaskFromOtherWorkerInstances( uint32_t requesting_worker_instance_id );

	// Finds registered worker instance that works on given thread id.
	TaskWorkerInstance* FindWorkerInstanceWithThreadID( btl::THREAD_ID id );

	std::vector< TaskWorkerInstance* > m_notAlwaysConvertedInstances;
	std::vector< TaskWorkerInstance* > m_alwaysConvertedInstances;
	std::vector< TaskWorkerInstance* > m_allRegisteredInstances;
};

////////////////////////////////////////////////////////////////////////
//
// INLINES:
//
////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
inline Dispatcher::Dispatcher()
{
}

/////////////////////////////////////////////////////////////////////////////
inline Dispatcher::~Dispatcher()
{
}

/////////////////////////////////////////////////////////////////////////////
inline uint32_t Dispatcher::GetRegisteredInstancesCount() const
{
	return ( uint32_t )m_allRegisteredInstances.size();
}

NAMESPACE_STS_END