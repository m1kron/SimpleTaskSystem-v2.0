#include <sts\private_headers\winAPI\ConditionVariableImplWinAPI.h>

NAMESPACE_STS_BEGIN
NAMESPACE_WINAPI_BEGIN

///////////////////////////////////////////////////////////
ConditionVariableImpl::ConditionVariableImpl()
{
	::InitializeConditionVariable( &m_ConditionVariable );
}

//////////////////////////////////////////////////////////
ConditionVariableImpl::~ConditionVariableImpl()
{
	// nothing to do (?)
}

//////////////////////////////////////////////////////////
void ConditionVariableImpl::NotifyOne()
{
	::WakeConditionVariable( &m_ConditionVariable );
}

//////////////////////////////////////////////////////////
void ConditionVariableImpl::NotifyAll()
{
	::WakeAllConditionVariable( &m_ConditionVariable );
}

NAMESPACE_WINAPI_END
NAMESPACE_STS_END