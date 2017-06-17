#include "conditionVariableImplWinAPI.h"

NAMESPACE_BTL_BEGIN
NAMESPACE_PLATFORM_API_BEGIN

///////////////////////////////////////////////////////////
ConditionVariableImpl::ConditionVariableImpl()
{
	::InitializeConditionVariable( &m_conditionVariable );
}

//////////////////////////////////////////////////////////
ConditionVariableImpl::~ConditionVariableImpl()
{
	// nothing to do (?)
}

//////////////////////////////////////////////////////////
void ConditionVariableImpl::NotifyOne()
{
	::WakeConditionVariable( &m_conditionVariable );
}

//////////////////////////////////////////////////////////
void ConditionVariableImpl::NotifyAll()
{
	::WakeAllConditionVariable( &m_conditionVariable );
}

NAMESPACE_PLATFORM_API_END
NAMESPACE_BTL_END
