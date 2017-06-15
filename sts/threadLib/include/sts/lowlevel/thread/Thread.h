#pragma once
#include <utility>
#include "thisThreadHelpers.h"

NAMESPACE_STS_BEGIN

////////////////////////////////////////////////////////
// Base class for all thread in the system. To create new thread, one has to
// derive from this class and implement ThreadFunction().
class ThreadBase : private PlatformAPI::ThreadImpl
{
	BASE_CLASS( PlatformAPI::ThreadImpl );

public:
	// Joins thread.
	void Join();

	// Detaches thread.
	void Detach();

	// Set debug thread name.
	void SetThreadName( const char* thread_name );

	// Starts the thread.
	void StartThread();

	// Returns thread id.
	THREAD_ID GetThreadID() const;

	// Function that will be called by thread.
	virtual void ThreadFunction() = 0;

protected:
	// To start thread instantly pass 'true'.
	ThreadBase( bool start_thread = false );
	ThreadBase( ThreadBase&& other );

	ThreadBase( const ThreadBase& ) = delete;
	ThreadBase& operator=( const ThreadBase& ) = delete;
};

///////////////////////////////////////////////////////////
//
// INLINES:
//
///////////////////////////////////////////////////////////

inline ThreadBase::ThreadBase( bool start_thread )
{
	if( start_thread )
	    StartThread();
}

///////////////////////////////////////////////////////////
inline ThreadBase::ThreadBase( ThreadBase&& other )
	: __base( std::forward< __base >( other ) ) 
{
}


///////////////////////////////////////////////////////////
inline void ThreadBase::StartThread()
{
	__base::StartThread( this );
}

///////////////////////////////////////////////////////////
inline void ThreadBase::Join()
{
	__base::Join();
}

///////////////////////////////////////////////////////////
inline void ThreadBase::Detach()
{
	__base::Detach();
}

///////////////////////////////////////////////////////////
inline void ThreadBase::SetThreadName( const char* thread_name )
{
	__base::SetThreadName( thread_name );
}

///////////////////////////////////////////////////////////
inline THREAD_ID ThreadBase::GetThreadID() const
{
	return __base::GetThreadID();
}

NAMESPACE_STS_END