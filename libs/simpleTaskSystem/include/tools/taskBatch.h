#pragma once
#include "..\iTaskManager.h"
#include <vector>

namespace sts
{
namespace tools
{

//////////////////////////////////////////////////////////////////////////////////
// Class represents batch of task - if you have a group of task to submit,
// task batch more optimal way to do it then submitting tem one by one.
// Rember to release tasks after execution! ( or use TaskBatch_AutoRelease ).
class TaskBatch
{
public:
	TaskBatch( ITaskManager* manager );
	~TaskBatch();

	// Adds task.
	void Add( const ITaskHandle* task );

	// Returns number of task in this batch.
	size_t GetSize() const;

	// Returns true if all tasks in batch are finished.
	bool AreAllTaskFinished() const;

	// Submits all tasks in batch to be executed.
	bool SubmitAll();

	// Releases all tasks.
	void ReleaseAllTasks();

	// Support for range based loops.
	std::vector< const ITaskHandle* >::iterator begin();
	std::vector< const ITaskHandle* >::iterator end();
	std::vector< const ITaskHandle* >::const_iterator begin() const;
	std::vector< const ITaskHandle* >::const_iterator end() const;

	// Support for normal for loops.
	const ITaskHandle* operator[]( size_t index );
	const ITaskHandle* operator[]( size_t index ) const;

protected:
	std::vector< const ITaskHandle* > m_taskBatch;
	ITaskManager* m_taskManager;
};

////////////////////////////////////////////////////////////////
// Same as task batch, but will release all task when goes out of scope.
// Convinient when adding tasks in coroutines.
class TaskBatch_AutoRelease : public TaskBatch
{
public:
	TaskBatch_AutoRelease( ITaskManager* manager );
	~TaskBatch_AutoRelease();
};

//////////////////////////////////////////////////////////////////////
//
// INLINES:
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
inline TaskBatch::TaskBatch( ITaskManager* manager )
	: m_taskManager( manager )
{
}

//////////////////////////////////////////////////////////////////////
inline TaskBatch::~TaskBatch()
{
	m_taskManager = nullptr;
}

///////////////////////////////////////////////////////////
inline void TaskBatch::Add( const ITaskHandle* task )
{
	m_taskBatch.push_back( task );
}

///////////////////////////////////////////////////////////
inline size_t TaskBatch::GetSize() const
{
	return m_taskBatch.size();
}

////////////////////////////////////////////////////////////
inline std::vector< const ITaskHandle* >::iterator TaskBatch::begin()
{
	return m_taskBatch.begin();
}

////////////////////////////////////////////////////////////
inline std::vector< const ITaskHandle* >::iterator TaskBatch::end()
{
	return m_taskBatch.end();
}

////////////////////////////////////////////////////////////
inline std::vector< const ITaskHandle* >::const_iterator TaskBatch::begin() const
{
	return m_taskBatch.begin();
}

////////////////////////////////////////////////////////////
inline std::vector< const ITaskHandle* >::const_iterator TaskBatch::end() const
{
	return m_taskBatch.end();
}

////////////////////////////////////////////////////////////
inline const ITaskHandle* TaskBatch::operator[]( size_t index )
{
	return m_taskBatch[ index ];
}

////////////////////////////////////////////////////////////
inline const ITaskHandle* TaskBatch::operator[]( size_t index ) const
{
	return m_taskBatch[ index ];
}

///////////////////////////////////////////////////////////
inline bool TaskBatch::AreAllTaskFinished() const
{
	for( auto handle : m_taskBatch )
		if( !handle->IsFinished() )
			return false;

	return true;
}

///////////////////////////////////////////////////////////
inline bool TaskBatch::SubmitAll()
{
	for( auto handle : m_taskBatch )
		if ( !m_taskManager->SubmitTask( handle ) )
			return false;

	return true;
}

inline void TaskBatch::ReleaseAllTasks()
{
	for( auto handle : m_taskBatch )
		m_taskManager->ReleaseTask( handle );

	m_taskBatch.clear();
}

////////////////////////////////////////////////////////////
inline TaskBatch_AutoRelease::TaskBatch_AutoRelease( ITaskManager* manager )
	: TaskBatch( manager )
{
}

////////////////////////////////////////////////////////////
inline TaskBatch_AutoRelease::~TaskBatch_AutoRelease()
{
	ReleaseAllTasks();
}

}
}