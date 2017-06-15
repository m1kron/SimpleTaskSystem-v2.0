#include <sts\tasking\TaskBatch.h>
#include <sts\tasking\Task.h>
#include <sts\tasking\TaskManager.h>

NAMESPACE_STS_BEGIN

///////////////////////////////////////////////////////////
bool TaskBatch::AreAllTaskFinished() const
{
	for( const TaskHandle& handle : m_taskBatch )
	{
		if( !handle->IsFinished() )
			return false;
	}

	return true;
}

////////////////////////////////////////////////////////////
TaskBatch_AutoRelease::~TaskBatch_AutoRelease()
{
	for( TaskHandle& handle : m_taskBatch )
	{
		ASSERT( handle->IsFinished() );
		m_taskManager.ReleaseTask( handle );
	}
}


NAMESPACE_STS_END