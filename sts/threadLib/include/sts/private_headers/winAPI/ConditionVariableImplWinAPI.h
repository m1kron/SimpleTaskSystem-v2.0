#pragma once

#include <windows.h>
#include <sts\private_headers\common\NamespaceMacros.h>

NAMESPACE_STS_BEGIN
NAMESPACE_WINAPI_BEGIN

class ConditionVariableImpl
{
protected:
	ConditionVariableImpl();
	~ConditionVariableImpl();

	// ConditionVariable cannot be moved or copied.
	ConditionVariableImpl( ConditionVariableImpl&& other ) = delete;
	ConditionVariableImpl( const ConditionVariableImpl& ) = delete;
	ConditionVariableImpl& operator= ( const ConditionVariableImpl& ) = delete;

	template< typename Predicate > void Wait( PCRITICAL_SECTION mutex, Predicate& p );
	void NotifyOne();
	void NotifyAll();

private:
	CONDITION_VARIABLE m_ConditionVariable;
};

///////////////////////////////////////////////////////////
//
// INLINES:
//
///////////////////////////////////////////////////////////

template< typename Predicate >
void ConditionVariableImpl::Wait( PCRITICAL_SECTION mutex, Predicate& predicate )
{
	while( !predicate() )
	{
		::SleepConditionVariableCS( &m_ConditionVariable, mutex, INFINITE );
	}
}

NAMESPACE_WINAPI_END
NAMESPACE_STS_END