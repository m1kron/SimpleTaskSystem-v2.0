#pragma once

NAMESPACE_STS_BEGIN

class Dispatcher;
class ITaskSystem;
class TaskFiberAllocator;
class TaskRegistry;

// Contains necessary context for task worker instance.
struct TaskWorkerInstanceContext
{
	const TaskRegistry* m_registry;
	ITaskSystem* m_taskSystem;
	Dispatcher* m_dispatcher;
	TaskFiberAllocator* m_fiberAllocator;
	uint32_t m_id;
};

NAMESPACE_STS_END