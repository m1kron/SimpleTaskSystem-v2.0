#pragma once

NAMESPACE_STS_BEGIN

class TaskWorkerInstance;

// Class represents worker instances hub, which holds registered worker instances in the system.
class TaskWorkerInstancesHub
{
public:
	// Registers instance. Returns index at which one will find this instance.
	uint32_t Register( TaskWorkerInstance* instance );

	// Unregisters all instances.
	void UnregisterAll();

	// Returns number of registered instances.
	uint32_t GetRegisteredInstancesCount() const;

	// Returns instance at given index.
	TaskWorkerInstance* GetRegisteredInstanceAt( uint32_t index );

private:
	std::vector< TaskWorkerInstance* > m_registeredInstances;
};

//////////////////////////////////////////////////////////////////////
//
// INLINES:
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
inline uint32_t TaskWorkerInstancesHub::Register( TaskWorkerInstance* instance )
{
	m_registeredInstances.push_back( instance );
	return GetRegisteredInstancesCount() - 1;
}

//////////////////////////////////////////////////////////////////////
inline void TaskWorkerInstancesHub::UnregisterAll()
{
	m_registeredInstances.clear();
}

//////////////////////////////////////////////////////////////////////
inline uint32_t TaskWorkerInstancesHub::GetRegisteredInstancesCount() const
{
	return (uint32_t)m_registeredInstances.size();
}

//////////////////////////////////////////////////////////////////////
inline TaskWorkerInstance* TaskWorkerInstancesHub::GetRegisteredInstanceAt( uint32_t index )
{
	return m_registeredInstances[ index ];
}

NAMESPACE_STS_END