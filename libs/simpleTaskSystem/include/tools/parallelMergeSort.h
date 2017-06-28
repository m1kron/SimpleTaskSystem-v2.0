#pragma once
#include <algorithm>
#include "..\iTaskSystem.h"
#include "taskStorageBuffer.h"

namespace sts
{
namespace tools
{
// Implementation of parallel merge sort.
// Returns true if succes. Needs O(n) additional memory, where n - size of container.
template< class TContainer >
bool ParallelMergeSort( TContainer& container, ITaskSystem* system_interface );

//////////////////////////////////////////////////////////////////////////
//
// IMPLEMENTATION:
//
//////////////////////////////////////////////////////////////////////////

namespace sts_helpers
{
/////////////////////////////////////////////////////////////////////////////
// Helper struct for passing information between tasks.
template< class TContainer >
struct MergeSortRange
{
	TContainer* m_container;
	TContainer* m_tempContainer;
	size_t m_startIdx;
	size_t m_range;
	union
	{
		size_t m_maxRange;	//< When going down ( splitting ) last field contains maxRange.
		size_t m_levels;	//< When going up ( merging ) last field contains levels.
	};

	typename TContainer::iterator GetBeginIt() const
	{
		auto begin_range_it = m_container->begin();
		std::advance( begin_range_it, m_startIdx );
		return begin_range_it;
	}

	typename TContainer::iterator GetEndIt() const
	{
		auto end_range_it = GetBeginIt();
		std::advance( end_range_it, m_range );
		return end_range_it;
	}

	typename TContainer::iterator GetInserter() const
	{
		auto begin_range_it = m_tempContainer->begin();
		std::advance( begin_range_it, m_startIdx );
		return begin_range_it;
	}
};

/////////////////////////////////////////////////////////////////////////////
template< typename TContainer >
void MergeSortTaskFunction( const sts::ITaskContext* context )
{
	typedef MergeSortRange< TContainer > TSortRange;

	// 1. Read our range.
	auto range = TaskStorageReader( context ).Read< TSortRange >();

	// 2. If current range is less then max_range, use sequential sorting algorithm and return.
	if( range.m_range <= range.m_maxRange )
	{
		std::sort( range.GetBeginIt(), range.GetEndIt(), std::less<int>() );
		range.m_levels++;
		TaskStorageWriter( context ).WriteSafe( range );
		return;
	}

	// 3. Otherwise we have to recursively split into two ranges:
	auto left_range_task = context->GetTaskSystem()->CreateNewTask( MergeSortTaskFunction< TContainer >, nullptr );
	auto right_range_task = context->GetTaskSystem()->CreateNewTask( MergeSortTaskFunction< TContainer >, nullptr );

	// 4. Calculate half range
	const auto HALF_RANGE = range.m_range / 2;
	{
		// 5. Setup ranges for tasks.
		auto left_range = range;
		left_range.m_range = HALF_RANGE;
		TaskStorageWriter( left_range_task ).WriteSafe( left_range );

		auto right_range = range;
		right_range.m_range = range.m_range - HALF_RANGE;
		right_range.m_startIdx = range.m_startIdx + HALF_RANGE;
		TaskStorageWriter( right_range_task ).WriteSafe( right_range );
	}

	// 6. Submit the tasks.
	context->GetTaskSystem()->SubmitTask( left_range_task );
	context->GetTaskSystem()->SubmitTask( right_range_task );

	// 7. Wait for tasks to finish.
	context->SuspendUntil( [ right_range_task, left_range_task ]() {return right_range_task->IsFinished() && left_range_task->IsFinished(); } );

	// 8. Get results from tasks - range.m_container contains sorted subrange.
	auto left_range = TaskStorageReader( left_range_task ).Read< TSortRange >();
	auto right_range = TaskStorageReader( right_range_task ).Read< TSortRange >();

	// 9. Release tasks.
	context->GetTaskSystem()->ReleaseTask( left_range_task );
	context->GetTaskSystem()->ReleaseTask( right_range_task );

	// 10. Merge two subranges into one. Subranges are always in child task's container and we are always merging them into child task's tempContainer.
	std::merge( left_range.GetBeginIt(), left_range.GetEndIt(), right_range.GetBeginIt(), right_range.GetEndIt(), left_range.GetInserter(), std::less<int>() );

	// 10. Write final result - swap containers.
	range.m_container = left_range.m_tempContainer;
	range.m_tempContainer = left_range.m_container;
	range.m_levels = left_range.m_levels + 1;

	TaskStorageWriter( context ).WriteSafe( range );
}
}

/////////////////////////////////////////////////////////////////////////////////////
template< class TContainer >
bool ParallelMergeSort( TContainer& container, ITaskSystem* system_interface )
{
	typedef sts_helpers::MergeSortRange< TContainer > TSortRange;

	// 1. Calculate batch size.
	const size_t MAX_RANGE = container.size() / ( size_t )( system_interface->GetWorkersCount() );

	// 2. Prepare working space.
	auto working_container = container;

	// 3. Setup task.
	TSortRange range{ &container, &working_container, 0, container.size(), MAX_RANGE };
	auto handle = system_interface->CreateNewTask( sts_helpers::MergeSortTaskFunction< TContainer >, nullptr );

	if( !handle )
		return false;

	TaskStorageWriter( handle ).WriteSafe( range );

	// 4. Submit and wait until done.
	if( !system_interface->SubmitTask( handle ) )
		return false;
	if( !system_interface->RunTasksUsingThisThreadUntil( [ handle ]() { return handle->IsFinished(); } ) )
		return false;

	auto final_range = TaskStorageReader( handle ).Read< TSortRange >();
	system_interface->ReleaseTask( handle );

	// 5. If working_container contains result, copy it to container.
	if( ( final_range.m_levels & 1 ) == 0 )
		std::copy( working_container.begin(), working_container.end(), container.begin() );

	return true;
}

}
}