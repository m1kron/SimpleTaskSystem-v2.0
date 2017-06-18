#include "precompiledHeader.h"
#include "taskWorkerThread.h"

NAMESPACE_STS_BEGIN

#define THREAD_LOG( ... ) LOG( "[WORKER_THREAD]: " __VA_ARGS__ );

///////////////////////////////////////////////////////////
void TaskWorkerThread::ThreadFunction()
{
	VERIFY_SUCCESS( m_workerInstance.ConvertToFiber() );
	THREAD_LOG( "Starting main loop." );

	MainWorkerThreadLoop();

	THREAD_LOG( "Main loop ended." );
	VERIFY_SUCCESS( m_workerInstance.ConvertToThread() );
	VERIFY_SUCCESS( m_workerInstance.Deinitalize() );
}

///////////////////////////////////////////////////////////
void TaskWorkerThread::MainWorkerThreadLoop()
{
	while( true )
	{
		// Check if we have any new task to work on. If not, then wait for them.
		m_hasWorkToDoEvent.Wait();
		m_hasWorkToDoEvent.ResetEvent();

		THREAD_LOG( "Weaking up." );

		// Finish work if requested:
		if( m_shouldFinishWork )
		{
			THREAD_LOG( "Requested to finish work." );
			m_hasFinishWork = true;
			return;
		}

		// Do all the tasks:
		while( !m_shouldFinishWork )
		{
			bool executed = m_workerInstance.TryToExecuteSingleTask();

			if( !executed )
				break;
		}
	}
}

NAMESPACE_STS_END