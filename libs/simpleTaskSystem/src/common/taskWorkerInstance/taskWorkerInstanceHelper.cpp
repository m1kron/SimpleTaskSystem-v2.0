#include "precompiledHeader.h"
#include "taskWorkerInstanceHelper.h"
#include "..\backend\taskFiber\taskFiber.h"

NAMESPACE_STS_BEGIN

//////////////////////////////////////////////////////////////////////////////////
thread_local TaskFiber* s_executingFiberThreadLocal = nullptr;

//////////////////////////////////////////////////////////////////////////////////
bool TaskWorkerInstanceHelper::IsOnActiveTaskWorkerInstance()
{
	return s_executingFiberThreadLocal != nullptr;
}

//////////////////////////////////////////////////////////////////////////////////
void TaskWorkerInstanceHelper::SuspendExecutionOfCurrentTask()
{
	if( s_executingFiberThreadLocal != nullptr )
	{
		ASSERT( s_executingFiberThreadLocal->GetCurrentState() != TaskFiberState::Idle );
		s_executingFiberThreadLocal->SuspendExecution();
	}
}

//////////////////////////////////////////////////////////////////////////////////
void TaskWorkerInstanceHelper::SetCurrentlyExecutedTask( TaskFiber* fiber )
{
	s_executingFiberThreadLocal = fiber;
}

NAMESPACE_STS_END