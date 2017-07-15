#pragma once

NAMESPACE_STS_BEGIN

class TaskFiber;

class TaskWorkerInstanceHelper
{
public:
	// Returns true if this function is called from thread with active task worker instance.
	static bool IsOnActiveTaskWorkerInstance();

	// Suspends execution of task executed by worker instance on thread that this function is called.
	static void SuspendExecutionOfCurrentTask();

private:
	friend class TaskWorkerInstance;

	// Sets currently executed task fiber
	static void SetCurrentlyExecutedTask( TaskFiber* fiber );
};

NAMESPACE_STS_END