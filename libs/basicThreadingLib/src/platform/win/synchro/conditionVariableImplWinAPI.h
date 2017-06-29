#pragma once
#include "..\winPlatformCommon.h"

NAMESPACE_BTL_BEGIN
NAMESPACE_PLATFORM_API_BEGIN

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
	CONDITION_VARIABLE m_conditionVariable;
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
		if( !::SleepConditionVariableCS( &m_conditionVariable, mutex, INFINITE ) )
		{
			WIN_ERROR_HANDLER();
		}
	}
}

NAMESPACE_PLATFORM_API_END
NAMESPACE_BTL_END
