#include <sts\private_headers\winAPI\MutexWinAPI.h>

NAMESPACE_STS_BEGIN
NAMESPACE_WINAPI_BEGIN

/////////////////////////////////////////////////////////
MutexImpl::MutexImpl()
{
	::InitializeCriticalSection( &m_CriticalSection );
}

/////////////////////////////////////////////////////////
MutexImpl::~MutexImpl()
{
	::DeleteCriticalSection( &m_CriticalSection );
}

/////////////////////////////////////////////////////////
void MutexImpl::Lock()
{
	::EnterCriticalSection( &m_CriticalSection );
}

/////////////////////////////////////////////////////////
bool MutexImpl::TryLock()
{
	return ::TryEnterCriticalSection( &m_CriticalSection ) != 0;
}

/////////////////////////////////////////////////////////
void MutexImpl::Unlock()
{
	::LeaveCriticalSection( &m_CriticalSection );
}

NAMESPACE_WINAPI_END
NAMESPACE_STS_END