#include <sts\private_headers\winAPI\ManualResetEventWinAPI.h>
#include <commonlib\Macros.h>

NAMESPACE_STS_BEGIN
NAMESPACE_WINAPI_BEGIN

//////////////////////////////////////////////////////////
ManualResetEventImpl::ManualResetEventImpl()
    : m_eventHandle( NULL )
	, m_isEventSet( false )
{
	m_eventHandle = ::CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		NULL                // object name
		); 

	ASSERT( m_eventHandle != NULL );
}

//////////////////////////////////////////////////////////
ManualResetEventImpl::~ManualResetEventImpl()
{
	::CloseHandle( m_eventHandle );
	m_eventHandle = NULL;
}

//////////////////////////////////////////////////////////
ManualResetEventImpl::ManualResetEventImpl( ManualResetEventImpl&& oth_event )
	: m_eventHandle( oth_event.m_eventHandle )
	, m_isEventSet( oth_event.m_isEventSet )
{
	oth_event.m_eventHandle = NULL;
	oth_event.m_isEventSet = false;
}

//////////////////////////////////////////////////////////
void ManualResetEventImpl::SetEvent()
{
	BOOL ret = ::SetEvent( m_eventHandle );
	ASSERT( ret != 0 );
	m_isEventSet = true;
}

//////////////////////////////////////////////////////////
void ManualResetEventImpl::ResetEvent()
{
	BOOL ret = ::ResetEvent( m_eventHandle );
	ASSERT( ret != 0 );
	m_isEventSet = false;
}

//////////////////////////////////////////////////////////
bool ManualResetEventImpl::IsEventSet()
{
	return m_isEventSet;
}

//////////////////////////////////////////////////////////
void ManualResetEventImpl::Wait()
{
	DWORD ret = ::WaitForSingleObject( m_eventHandle, INFINITE );
	ASSERT( ret == WAIT_OBJECT_0 );
}

//////////////////////////////////////////////////////////
void ManualResetEventImpl::WaitFor( unsigned miliseconds )
{
	DWORD ret = ::WaitForSingleObject( m_eventHandle, miliseconds );
	ASSERT( ret != WAIT_FAILED );
}

//////////////////////////////////////////////////////////
ManualResetEventImpl::EVENT_NATIVE_HANDLE ManualResetEventImpl::GetNativeHandle()
{
	return m_eventHandle;
}

NAMESPACE_WINAPI_END
NAMESPACE_STS_END