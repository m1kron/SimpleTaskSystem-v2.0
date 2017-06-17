#pragma once
#include "..\manager\taskManager.h"
#include "..\..\..\basicThreadingLib\include\tools\tools.h"
#include "..\..\..\basicThreadingLib\include\thread\functorThread.h"

NAMESPACE_STS_BEGIN

// Calls functor parallely for each element between begin and end.
// Tries to balance work load between available logical cores.
// Function blocks until it is done. Function creates new Thread instances inside.
template< class Iterator, typename Functor >
void ParallelForEach( typename Iterator& begin,				///< Begin iterator
					  typename Iterator& end,				///< End iterator
					  const Functor& functor,				///< functor will called on every iterator between begin and end.
					  unsigned max_num_of_threads = 0 );	///< maximum number of threads, that implementation can use. O means that it is up to the implementation.

// The same as above, but uses task system instead of raw threads.
template< class Iterator, typename Functor >
void ParallelForEachUsingTasks( const Iterator& begin,			///< Begin iterator
								const Iterator& end,			///< End iterator
								const Functor& functor,			///< functor will called on every iterator between begin and end.
								TaskManager& task_manager );	///< task manager instance that will be used to deliver task functionality.

//////////////////////////////////////////////////////////////////////////
//
// IMPLEMENTATION:
//
//////////////////////////////////////////////////////////////////////////

template< class Iterator, typename Functor >
void ParallelForEach( const Iterator& begin, const Iterator& end, const Functor& functor, unsigned max_num_of_threads )
{
	if( max_num_of_threads == 0 )
		max_num_of_threads = tools::GetLogicalCoresSize();

	auto con_size = std::distance( begin, end );;
	auto batch_size = ( con_size / max_num_of_threads );
	Iterator last_it = end;

	std::vector< FunctorThread > workers;
	std::vector<std::function<void( void )> > functors;

	// Setup job.
	// Split the for amoung requested number of threads (inlcuding thread that called that function):
	for( unsigned i = 0; i < max_num_of_threads - 1; ++i )
	{
		Iterator start_it = begin;
		std::advance( start_it, i * batch_size );

		Iterator end_it = start_it;
		std::advance( end_it, batch_size );

		last_it = end_it;

		std::function< void( void ) > func = [ &functor, start_it, end_it ]()
		{
			for( auto it = start_it; it != end_it; ++it )
				functor( it );
		};

		functors.emplace_back( std::move( func ) );
	}

	// Start job. Start workers.
	for( auto& func : functors )
		workers.emplace_back( FunctorThread() );

	for( unsigned i = 0; i < max_num_of_threads - 1; ++i )
	{
		workers[ i ].SetFunctorAndStartThread( functors[ i ] );
		workers[ i ].SetThreadName( "ParallelFor_WorkerThread" );
	}

	// Do the rest job in this thread:
	for( auto it = last_it; it != end; ++it )
		functor( it );

	// Wait for thread to finish their jobs.
	for( auto& thread : workers )
		thread.Join();
}

/////////////////////////////////////////////////////////////////////////////////////
template< class Iterator, typename Functor >
void ParallelForEachUsingTasks( const Iterator& begin, const Iterator& end, const Functor& functor, TaskManager& task_manager )
{
	unsigned max_num_of_threads = task_manager.GetWorkersCount() + 1;
	auto con_size = std::distance( begin, end );
	auto batch_size = ( con_size / max_num_of_threads );
	Iterator last_it = end;

	sts::TaskBatch_AutoRelease batch( task_manager );
	
	// WARNING!
	// This is needed only in debug mode, cuz in debug stl iterators are so big,
	// that task's internal storage cannot hold them..
DBG_ONLY_LINE( std::vector< Iterator > iterators; )
DBG_ONLY_LINE( iterators.reserve( 2 * ( max_num_of_threads - 1 ) ); )

	// Setup tasks.
	for( unsigned i = 0; i < max_num_of_threads - 1; ++i )
	{
		Iterator start_it = begin;
		std::advance( start_it, i * batch_size );

		Iterator end_it = start_it;
		std::advance( end_it, batch_size );

		last_it = end_it;

DBG_ONLY_LINE( iterators.push_back( start_it ); )
DBG_ONLY_LINE( iterators.push_back( end_it ); )
DBG_ONLY_LINE( Iterator& dbg_start_it = iterators[ iterators.size() - 2 ]; )
DBG_ONLY_LINE( Iterator& dbg_end_it = iterators[ iterators.size() - 1 ]; )

#ifdef DEBUG_MODE
		// Debug version:
		auto func = [ &functor, &dbg_start_it, &dbg_end_it ]( TaskContext& )
		{
			for( Iterator it = dbg_start_it; it != dbg_end_it; ++it )
				functor( it );
		};
#else
		// Release version:
		auto func = [ &functor, start_it, end_it ]( TaskContext& )
		{
			for( auto it = start_it; it != end_it; ++it )
				functor( it );
		};
#endif

		sts::TaskHandle handle = task_manager.CreateNewTask( func );
		ASSERT( handle != sts::INVALID_TASK_HANDLE );
		batch.Add( std::move( handle ) );
	}

	task_manager.SubmitTaskBatch( batch );

	// Do the rest of job using this thread:
	for( auto it = last_it; it != end; ++it )
		functor( it );

	// wait for task to finish.
	task_manager.RunTasksUsingThisThreadUntil( [ &batch ] { return batch.AreAllTaskFinished(); } );
}

NAMESPACE_STS_END