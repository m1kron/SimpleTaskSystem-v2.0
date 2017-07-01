#pragma once
#include "..\iTaskSystem.h"

namespace sts
{
namespace tools
{

//////////////////////////////////////////////////////////////////////////////////
// Class represents batch of task - if you have a group of task to handle,
// this class is here to help you.
// Batch also allows to automatically release all of the tasks when it goes out
// of the scope.
// This is a 'raw' version of batch - user has to deliver buffer for storing const ITaskHandle*.
// Check out also TaskBatch, which delivers memory as well. 
class TaskBatchRaw
{
public:
	TaskBatchRaw( ITaskSystem* system_interface, const ITaskHandle** buffer_for_handles, size_t buffer_capacity, bool auto_release );
	~TaskBatchRaw();

	// Adds task. Retruns true if success.
	bool Add( const ITaskHandle* task );

	// Returns number of tasks in this batch.
	size_t GetSize() const;

	// Returns true if all tasks in batch are finished.
	bool AreAllTaskFinished() const;

	// Submits all tasks in batch to be executed.
	bool SubmitAll();

	// Releases all tasks.
	void ReleaseAllTasks();

	// Support for range based loops.
	const ITaskHandle** begin() const;
	const ITaskHandle** end() const;

	// Support for normal for loops.
	const ITaskHandle* operator[]( size_t index ) const;

protected:
	ITaskSystem* m_taskSystem;
	const ITaskHandle** m_buffer;
	size_t m_bufferCapacity;
	size_t m_currentSize;
	bool m_autoRelease;
};

//////////////////////////////////////////////////////////////////////
// This class basically only delivers a memory for TaskBatchRaw. User
// has to specify maximum number of tasks that this batch can handle.
template< unsigned BATCH_SIZE >
class TaskBatch : public TaskBatchRaw
{
public:
	TaskBatch( ITaskSystem* system_interface, bool auto_release = true );

private:
	const ITaskHandle* m_buffer[ BATCH_SIZE ];
};

//////////////////////////////////////////////////////////////////////
//
// INLINES:
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
inline TaskBatchRaw::TaskBatchRaw( ITaskSystem* system_interface, const ITaskHandle** buffer_for_handles, size_t buffer_capacity, bool auto_release )
	: m_taskSystem( system_interface )
	, m_buffer( buffer_for_handles )
	, m_bufferCapacity( buffer_capacity )
	, m_currentSize( 0 )
	, m_autoRelease( auto_release )
{
}

//////////////////////////////////////////////////////////////////////
inline TaskBatchRaw::~TaskBatchRaw()
{
	if( m_autoRelease )
		ReleaseAllTasks();
}

//////////////////////////////////////////////////////////////////////
inline bool TaskBatchRaw::Add( const ITaskHandle* task )
{
	if( task != nullptr && m_currentSize < m_bufferCapacity )
	{
		m_buffer[ m_currentSize++ ] = task;
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
inline size_t TaskBatchRaw::GetSize() const
{
	return m_currentSize;
}

//////////////////////////////////////////////////////////////////////
inline const ITaskHandle** TaskBatchRaw::begin() const
{
	return &m_buffer[ 0 ];
}

//////////////////////////////////////////////////////////////////////
inline const ITaskHandle** TaskBatchRaw::end() const
{
	return &m_buffer[ m_currentSize ];
}

//////////////////////////////////////////////////////////////////////
inline const ITaskHandle* TaskBatchRaw::operator[]( size_t index ) const
{
	return m_buffer[ index ];
}

//////////////////////////////////////////////////////////////////////
inline bool TaskBatchRaw::AreAllTaskFinished() const
{
	for( auto handle : *this )
		if( !handle->IsFinished() )
			return false;

	return true;
}

//////////////////////////////////////////////////////////////////////
inline bool TaskBatchRaw::SubmitAll()
{
	for( auto handle : *this )
		if ( !m_taskSystem->SubmitTask( handle ) )
			return false;

	return true;
}

//////////////////////////////////////////////////////////////////////
inline void TaskBatchRaw::ReleaseAllTasks()
{
	for( auto handle : *this )
		m_taskSystem->ReleaseTask( handle );
		
	m_currentSize = 0;
}

//////////////////////////////////////////////////////////////////////
template< unsigned BATCH_SIZE >
inline TaskBatch< BATCH_SIZE >::TaskBatch( ITaskSystem * system_interface, bool auto_release )
	: TaskBatchRaw( system_interface, m_buffer, BATCH_SIZE, auto_release )
{
}

}
}