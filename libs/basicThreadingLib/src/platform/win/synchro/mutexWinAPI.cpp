#include "mutexWinAPI.h"

NAMESPACE_BTL_BEGIN
NAMESPACE_PLATFORM_API_BEGIN

/////////////////////////////////////////////////////////
MutexImpl::MutexImpl()
{
	::InitializeCriticalSection( &m_criticalSection );
}

/////////////////////////////////////////////////////////
MutexImpl::~MutexImpl()
{
	::DeleteCriticalSection( &m_criticalSection );
}

/////////////////////////////////////////////////////////
void MutexImpl::Lock()
{
	::EnterCriticalSection( &m_criticalSection );
}

/////////////////////////////////////////////////////////
bool MutexImpl::TryLock()
{
	return ::TryEnterCriticalSection( &m_criticalSection ) != 0;
}

/////////////////////////////////////////////////////////
void MutexImpl::Unlock()
{
	::LeaveCriticalSection( &m_criticalSection );
}

NAMESPACE_PLATFORM_API_END
NAMESPACE_BTL_END
