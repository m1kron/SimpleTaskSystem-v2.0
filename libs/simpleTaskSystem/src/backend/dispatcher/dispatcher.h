#pragma once
#include "..\..\..\..\libs\basicThreadingLib\include\thread\thisThreadHelpers.h"

NAMESPACE_STS_BEGIN

namespace common { class TaskWorkerInstance; }

NAMESPACE_BACKEND_BEGIN

class Task;
class TaskFiber;

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

	// Registers instance. Returns index at which one will find this instance, primary_instance == false means that 
	// instance is a helper instance only, otherwise instance is a primary instance.
	// Helper instance is an instance that is not always available and has to be 
	// threated in special way in some scenarios.
	// Primary instance is alway available and dispatcher can count on that.
	uint32_t Register( common::TaskWorkerInstance* instance, bool primary_instance );

	// Sets function to weak up primary worker threads. OK, this is kind of hack:
	// my idea was that there is sth like worker instance and this dispatcher only bothers about that.
	// Worker instance can work on dedicated thread or on any thread that can be transfomed to worker instacnce.
	// Unfourtanetly, dedicted threads have to be weak up, otherwise they won't start process work. 
	void SetWakeUpAllPrimaryWorkersFunction( const std::function< void() >& wakeUpAll );

	// Unregisters all instances.
	void UnregisterAll();

	// Dispatches single task to worker instances.
	bool DispatchTask( Task* task );

	// Redispatches task from helper worker instance. Returns true if success.
	bool RedispatchTaskFromHelperWorkerInstance( Task* task );

	// Redispatches suspended fiber task. Returns true if success.
	bool RedispatchSuspendedTaskFiber( TaskFiber* suspended_task_fiber );

	// Returns number of registered instances.
	uint32_t GetRegisteredInstancesCount() const;

private:
	friend common::TaskWorkerInstance;

	// Wakes up all primary worker instances.
	void WakeUpAllPrimaryWorkerInstances();

	// Dispatches task to primary intances only.
	bool DispatchTaskToPrimaryInstances( Task* task );

	// Try to steal a task from other worker instances then the one with requesting_worker_instance_id.
	// Returns stealed task if success, nullptr otherwise.
	Task* TryToStealTaskFromOtherWorkerInstances( uint32_t requesting_worker_instance_id );

	// Finds registered worker instance that works on given thread id.
	common::TaskWorkerInstance* FindWorkerInstanceWithThreadID( btl::THREAD_ID id );

	std::vector< common::TaskWorkerInstance* > m_helpersInstances;
	std::vector< common::TaskWorkerInstance* > m_primaryInstances;
	std::vector< common::TaskWorkerInstance* > m_allRegisteredInstances;
	std::function< void() > m_wakeUpAllPrimaryWorkerInstancesFunc;
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

/////////////////////////////////////////////////////////////////////////////
inline void Dispatcher::SetWakeUpAllPrimaryWorkersFunction( const std::function<void()>& wakeUpAll )
{
	m_wakeUpAllPrimaryWorkerInstancesFunc = wakeUpAll;
}

/////////////////////////////////////////////////////////////////////////////
inline void Dispatcher::WakeUpAllPrimaryWorkerInstances()
{
	m_wakeUpAllPrimaryWorkerInstancesFunc();
}

NAMESPACE_BACKEND_END
NAMESPACE_STS_END