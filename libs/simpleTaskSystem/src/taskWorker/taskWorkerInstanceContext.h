#pragma once

NAMESPACE_STS_BEGIN

class TaskWorkerInstancesHub;
class TaskManager;
class TaskFiberAllocator;
class TaskFiber;

// Contains necessary info for task worker instance.
struct TaskWorkerInstanceContext
{
	TaskManager* m_taskManager;
	TaskWorkerInstancesHub* m_taskWorkerInstancesHub;
	TaskFiberAllocator* m_fiberAllocator;
	uint32_t m_id;
};

NAMESPACE_STS_END