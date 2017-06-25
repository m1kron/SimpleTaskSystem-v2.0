#pragma once

NAMESPACE_STS_BEGIN

class Dispatcher;
class ITaskManager;
class TaskFiberAllocator;
class TaskFiber;

// Contains necessary context for task worker instance.
struct TaskWorkerInstanceContext
{
	ITaskManager* m_taskManager;
	Dispatcher* m_dispatcher;
	TaskFiberAllocator* m_fiberAllocator;
	uint32_t m_id;
};

NAMESPACE_STS_END