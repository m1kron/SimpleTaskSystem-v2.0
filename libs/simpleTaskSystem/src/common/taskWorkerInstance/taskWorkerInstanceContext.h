#pragma once

NAMESPACE_STS_BEGIN

class ITaskSystem;

namespace backend
{
	class Dispatcher;
	class TaskFiberAllocator;
	class TaskRegistry;
}

NAMESPACE_COMMON_BEGIN

// Contains necessary context for task worker instance.
struct TaskWorkerInstanceContext
{
	const backend::TaskRegistry* m_registry;
	ITaskSystem* m_taskSystem;
	backend::Dispatcher* m_dispatcher;
	backend::TaskFiberAllocator* m_fiberAllocator;
	uint32_t m_id;
};

NAMESPACE_COMMON_END
NAMESPACE_STS_END