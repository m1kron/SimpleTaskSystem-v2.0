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
						ITaskSystem* system_interface,		//< task system interface instance that will be used to deliver task functionality
						size_t cut_off );					//< when to cut off recursion and process given batch sequentailly

//////////////////////////////////////////////////////////////////////////
//
// IMPLEMENTATION:
//
//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
template< class Iterator, typename Functor >
bool ParallelForEach( const Iterator& begin, const Iterator& end, const Functor& functor, ITaskSystem* task_system_interface, size_t cut_off )
{
	auto con_size = std::distance( begin, end );
	auto half_size = ( con_size / 2 );

	if( (size_t)con_size < cut_off )
	{
		// Just do it sequentually:
		for( auto it = begin; it != end; ++it )
			functor( it );

		return true;
	}

	// Batch is to big to calculate it sequentially, so spread it across threads.
	Iterator first_half_begin = begin;
	Iterator first_half_end = first_half_begin;
	std::advance( first_half_end, half_size + 1 );

	Iterator second_half_begin = first_half_end;
	Iterator second_half_end = end;

	// Crete a task to execute second half:
	TaskBatch< 1 > batch( task_system_interface );
	auto second_half_lambda = [ &functor, &second_half_begin, &second_half_end, cut_off ]( const ITaskContext* context )
	{
		return ParallelForEach( second_half_begin, second_half_end, functor, context->GetTaskSystem(), cut_off );
	};

	if( !batch.Add( LambdaTaskMaker( second_half_lambda, task_system_interface, nullptr ) ) )
		return false;

	if( !batch.SubmitAll() )
		return false;

	// Execute ParallelFor for first half.
	ParallelForEach( first_half_begin, first_half_end, functor, task_system_interface, cut_off );

	// Wait for rest to finish.
	if( !task_system_interface->WaitUntil( [ &batch ] { return batch.AreAllTaskFinished(); } ) )
		return false;

	return !batch.HasExecutionError();
}

}
}