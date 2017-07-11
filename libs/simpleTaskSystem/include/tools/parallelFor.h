#pragma once
#include "..\iTaskSystem.h"
#include "taskBatch.h"
#include "lambdaTask.h"

namespace sts
{
namespace tools
{

// Calls functor parallely for each element between begin and end.
// Tries to balance work load between available logical cores.
// Function blocks until it is done.
// Returns true when everything went ok ( and tasks are done ) or false when error and tasks are not done.
// [NOTE]: this is very simple implementation of parallelFor ( it assumes it takes similar time to process each iterator ).
// Better implementation would keep spliting range until very small one and execute them. 
template< class Iterator, typename Functor >
bool ParallelForEach(	const Iterator& begin,				//< Begin iterator
						const Iterator& end,				//< End iterator
						const Functor& functor,				//< functor will called on every iterator between begin and end.
						ITaskSystem* system_interface );	//< task system interface instance that will be used to deliver task functionality.

//////////////////////////////////////////////////////////////////////////
//
// IMPLEMENTATION:
//
//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
template< class Iterator, typename Functor >
bool ParallelForEach( const Iterator& begin, const Iterator& end, const Functor& functor, ITaskSystem* task_system_interface )
{
	uint32_t max_num_of_threads = task_system_interface->GetWorkersCount() + 1;
	auto con_size = std::distance( begin, end );
	auto batch_size = ( con_size / max_num_of_threads );
	Iterator last_it = end;

	static const unsigned MAX_WORKERS = 64;

	if( MAX_WORKERS < max_num_of_threads )
		return false; // or use some dynamic memory implementation.

	TaskBatch< MAX_WORKERS > batch( task_system_interface );
	
	// WARNING!
	// This is needed only in debug mode, cuz in debug stl iterators are so big,
	// that task's internal storage cannot hold them..
#ifdef _DEBUG
	std::vector< Iterator > iterators;
	iterators.reserve( 2 * ( max_num_of_threads - 1 ) );
#endif

	// Setup tasks.
	for( uint32_t i = 0; i < max_num_of_threads - 1; ++i )
	{
		Iterator start_it = begin;
		std::advance( start_it, i * batch_size );

		Iterator end_it = start_it;
		std::advance( end_it, batch_size );

		last_it = end_it;

#ifdef _DEBUG
		iterators.push_back( start_it ); 
		iterators.push_back( end_it );
		Iterator& dbg_start_it = iterators[ iterators.size() - 2 ];
		Iterator& dbg_end_it = iterators[ iterators.size() - 1 ];
#endif

#ifdef _DEBUG
		// Debug version:
		auto func = [ &functor, &dbg_start_it, &dbg_end_it ]( const ITaskContext* )
		{
			for( Iterator it = dbg_start_it; it != dbg_end_it; ++it )
				functor( it );
			return true;
		};
#else
		// Release version:
		auto func = [ &functor, start_it, end_it ]( const ITaskContext* )
		{
			for( auto it = start_it; it != end_it; ++it )
				functor( it );
			return true;
		};
#endif

		if( auto handle = LambdaTaskMaker( func, task_system_interface, nullptr ) )
			batch.Add( handle );
		else
			return false;
	}

	if( !batch.SubmitAll() )
		return false;

	// Do the rest of job using this thread:
	for( auto it = last_it; it != end; ++it )
		functor( it );

	// wait for rest to finish.
	return task_system_interface->RunTasksUsingThisThreadUntil( [ &batch ] { return batch.AreAllTaskFinished(); } );
}

}
}