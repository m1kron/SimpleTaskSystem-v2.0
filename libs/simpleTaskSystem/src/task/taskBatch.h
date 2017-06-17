#pragma once
#include "taskHandle.h"

NAMESPACE_STS_BEGIN

class TaskManager;

/////////////////////////////////////////////////////
// Class represents batch of task - if you have a group of task to submit,
// task batch more optimal way to do it then submitting tem one by one.
class TaskBatch
{
public:
	TaskBatch() {}
	~TaskBatch();

	// Adds task.
	void Add( TaskHandle&& task );

	// Returns number of task in this batch.
	unsigned GetSize() const;

	// Returns true if all tasks in batch are finished.
	bool AreAllTaskFinished() const;

	// Support for range based loops.
	std::vector< TaskHandle >::iterator begin();
	std::vector< TaskHandle >::iterator end();
	std::vector< TaskHandle >::const_iterator begin() const;
	std::vector< TaskHandle >::const_iterator end() const;

	// Support for normal for loops.
	TaskHandle& operator[]( unsigned index );
	const TaskHandle& operator[]( unsigned index ) const;

protected:
	std::vector< TaskHandle > m_taskBatch;
};

////////////////////////////////////////////////////////////////
// Same as task batch, but will release all task when goes out of scope.
// Convinient when adding tasks in coroutines.
class TaskBatch_AutoRelease : public TaskBatch
{
public:
	TaskBatch_AutoRelease( TaskManager& manager );
	~TaskBatch_AutoRelease();

private:
	TaskManager& m_taskManager;
};

//////////////////////////////////////////////////////////////////////
//
// INLINES:
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
inline TaskBatch::~TaskBatch()
{
	m_taskBatch.clear();
}

///////////////////////////////////////////////////////////
inline void TaskBatch::Add( TaskHandle&& task )
{
	m_taskBatch.push_back( std::move( task ) );
}

///////////////////////////////////////////////////////////
inline unsigned TaskBatch::GetSize() const
{
	return (unsigned)m_taskBatch.size();
}

////////////////////////////////////////////////////////////
inline std::vector< TaskHandle >::iterator TaskBatch::begin()
{
	return m_taskBatch.begin();
}

////////////////////////////////////////////////////////////
inline std::vector< TaskHandle >::iterator TaskBatch::end()
{
	return m_taskBatch.end();
}

////////////////////////////////////////////////////////////
inline std::vector< TaskHandle >::const_iterator TaskBatch::begin() const
{
	return m_taskBatch.begin();
}

////////////////////////////////////////////////////////////
inline std::vector< TaskHandle >::const_iterator TaskBatch::end() const
{
	return m_taskBatch.end();
}

////////////////////////////////////////////////////////////
inline TaskHandle& TaskBatch::operator[]( unsigned index )
{
	return m_taskBatch[ index ];
}

////////////////////////////////////////////////////////////
inline const TaskHandle& TaskBatch::operator[]( unsigned index ) const
{
	return m_taskBatch[ index ];
}

////////////////////////////////////////////////////////////
inline TaskBatch_AutoRelease::TaskBatch_AutoRelease( TaskManager& manager )
	: m_taskManager( manager )
{
}


NAMESPACE_STS_END