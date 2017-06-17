#pragma once
#include "..\..\src\platform\conditionVariablePlatform.h"

NAMESPACE_BTL_BEGIN

class Mutex;
class ConditionVariable : private PlatformAPI::ConditionVariableImpl
{
	BASE_CLASS( PlatformAPI::ConditionVariableImpl );

public:
	ConditionVariable() {}
	~ConditionVariable() {}

	ConditionVariable( const ConditionVariable& ) = delete;
	ConditionVariable& operator= ( const ConditionVariable& ) = delete;

	// Blocks until predicate is true. When waiting mutex is unlocked, so
	// other thread can lock it, but function automatically re-acquires mutex when 
	// returns.
	template < typename Predicate > void Wait( Mutex& mutex, Predicate& predicate );

	// Notifies one thread that condition should be satisfied now.
	void NotifyOne();

	// Notifies all threads that condition should be satisfied now.
	void NotifyAll();
};

///////////////////////////////////////////////////////////
//
// INLINES:
//
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
template< typename Predicate >
inline void ConditionVariable::Wait( Mutex& mutex, Predicate& predicate )
{
	__base::Wait( mutex.NativeHandle(), predicate );
}

///////////////////////////////////////////////////////////
inline void ConditionVariable::NotifyOne()
{
	__base::NotifyOne();
}

///////////////////////////////////////////////////////////
inline void ConditionVariable::NotifyAll()
{
	__base::NotifyAll();
}

NAMESPACE_BTL_END
