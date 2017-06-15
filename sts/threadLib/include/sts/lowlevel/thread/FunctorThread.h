#pragma once

#include <functional>
#include <sts\private_headers\thread\ThreadPlatform.h>
#include <sts\lowlevel\thread\Thread.h>
#include <commonlib\Macros.h>

NAMESPACE_STS_BEGIN

/////////////////////////////////////////
// Thread that runs given functor.
class FunctorThread : public ThreadBase
{
	BASE_CLASS( ThreadBase );

public:
	FunctorThread() {}
	FunctorThread( const std::function< void( void ) >& functor );
	FunctorThread( FunctorThread&& other );

	// Sets functor and starts the thread.
	void SetFunctorAndStartThread( const std::function< void( void ) >& functor );

private:
	void ThreadFunction() override;

	std::function< void( void ) > m_functor;
};

///////////////////////////////////////////////////////////
//
// INLINES:
//
///////////////////////////////////////////////////////////

inline FunctorThread::FunctorThread( const std::function<void( void )>& functor )
	: m_functor( functor )
{
	__base::StartThread();
}

///////////////////////////////////////////////////////////
inline FunctorThread::FunctorThread( FunctorThread&& other )
	: __base( std::forward< __base >( other ) )
	, m_functor( std::forward< std::function<void( void )> >( other.m_functor ) )
{
}

///////////////////////////////////////////////////////////
inline void FunctorThread::SetFunctorAndStartThread( const std::function< void( void ) >& functor )
{
	m_functor = functor;
	StartThread();
}

///////////////////////////////////////////////////////////
inline void FunctorThread::ThreadFunction()
{
	m_functor();
}

NAMESPACE_STS_END