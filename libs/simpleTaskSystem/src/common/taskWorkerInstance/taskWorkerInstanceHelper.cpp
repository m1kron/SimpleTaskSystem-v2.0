#include "precompiledHeader.h"
#include "taskWorkerInstanceHelper.h"
#include "..\backend\taskFiber\taskFiber.h"

NAMESPACE_STS_BEGIN
NAMESPACE_COMMON_BEGIN

//////////////////////////////////////////////////////////////////////////////////
thread_local backend::TaskFiber* s_executingFiberThreadLocal = nullptr;

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
		ASSERT( s_executingFiberThreadLocal->GetCurrentState() != backend::TaskFiberState::Idle );
		s_executingFiberThreadLocal->SuspendExecution();
	}
}

//////////////////////////////////////////////////////////////////////////////////
void TaskWorkerInstanceHelper::SetCurrentlyExecutedTask( backend::TaskFiber* fiber )
{
	s_executingFiberThreadLocal = fiber;
}

NAMESPACE_COMMON_END
NAMESPACE_STS_END