#include "manualResetEventWinAPI.h"

NAMESPACE_BTL_BEGIN
NAMESPACE_PLATFORM_API_BEGIN

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

	if( m_eventHandle == NULL ) { WIN_ERROR_HANDLER(); }
}

//////////////////////////////////////////////////////////
ManualResetEventImpl::~ManualResetEventImpl()
{
	if( !::CloseHandle( m_eventHandle ) ) { WIN_ERROR_HANDLER(); }
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
	if ( !::SetEvent( m_eventHandle ) ) { WIN_ERROR_HANDLER(); }
	m_isEventSet = true;
}

//////////////////////////////////////////////////////////
void ManualResetEventImpl::ResetEvent()
{
	if( !::ResetEvent( m_eventHandle ) ) { WIN_ERROR_HANDLER(); }
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
	if( ret == WAIT_FAILED ) { WIN_ERROR_HANDLER(); }
	ASSERT( ret == WAIT_OBJECT_0 );
}

//////////////////////////////////////////////////////////
void ManualResetEventImpl::WaitFor( unsigned miliseconds )
{
	DWORD ret = ::WaitForSingleObject( m_eventHandle, miliseconds );
	if( ret == WAIT_FAILED ) { WIN_ERROR_HANDLER(); }
}

//////////////////////////////////////////////////////////
ManualResetEventImpl::EVENT_NATIVE_HANDLE ManualResetEventImpl::GetNativeHandle()
{
	return m_eventHandle;
}

NAMESPACE_PLATFORM_API_END
NAMESPACE_BTL_END
