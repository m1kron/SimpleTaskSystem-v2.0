#include "fiberImplWinAPI.h"
#include "..\..\..\..\include\thread\fiber.h"
#include "..\..\..\..\..\commonLib\include\macros.h"

#define HANDLE_WIN_ERROR()							\
		auto error = ::GetLastError();				\
		LOG( "[WIN_API_ERROR]: %i", error );		\
		ASSERT( false );

NAMESPACE_BTL_BEGIN
NAMESPACE_PLATFORM_API_BEGIN

//////////////////////////////////////////////////////
void SwitchToFiber( FIBER_ID id )
{
	::SwitchToFiber( id );
}

//////////////////////////////////////////////////////
FIBER_ID GetCurrentFiberID()
{
	return ::GetCurrentFiber();
}

//////////////////////////////////////////////////////
bool ConvertFiberToThread()
{
	auto ret = ::ConvertFiberToThread();

	if( !ret ) { HANDLE_WIN_ERROR(); }

	return ret;
}

//////////////////////////////////////////////////////
FIBER_ID ConvertThreadToFiber( void* params )
{
	auto id = ::ConvertThreadToFiber( params );

	if( id == INVALID_FIBER_ID ) { HANDLE_WIN_ERROR(); }
	return id;
}

//////////////////////////////////////////////////////
bool IsThreadConvertedToFiber()
{
	// This is not documented on MSDN, but seems to be working.
	// More info: http://crystalclearsoftware.com/soc/coroutine/coroutine/fibers.html
	static const FIBER_ID MAGIC_FIBER_CONSTANT = ( FIBER_ID )0x1E00;
	
	auto ret = ::GetCurrentFiber();
	return (ret != MAGIC_FIBER_CONSTANT) && (ret != INVALID_FIBER_ID);
}

//////////////////////////////////////////////////////
void WINAPI FiberFunction( LPVOID lpParam )
{
	FiberBase* fiber = static_cast< FiberBase* > ( lpParam );
	fiber->FiberFunction();
}

//////////////////////////////////////////////////
FiberImpl::FiberImpl()
	: m_id( INVALID_FIBER_ID )
{
}

///////////////////////////////////////////////////
FiberImpl::FiberImpl( FiberImpl&& other )
	: m_id( other.m_id )
{
	other.m_id = INVALID_FIBER_ID;
}

///////////////////////////////////////////////////
FiberImpl::~FiberImpl()
{
}

///////////////////////////////////////////////////
void FiberImpl::CreateFiber( FiberBase* fiber, uint32_t stackSize )
{
	m_id = ::CreateFiber( stackSize, FiberFunction, fiber );

	if( m_id == INVALID_FIBER_ID ) { HANDLE_WIN_ERROR(); }
}

///////////////////////////////////////////////////
void FiberImpl::DeleteFiber()
{
	::DeleteFiber( m_id );
	m_id = INVALID_FIBER_ID;
}

///////////////////////////////////////////////////
FIBER_ID FiberImpl::GetFiberID() const
{
	return m_id;
}

NAMESPACE_PLATFORM_API_END
NAMESPACE_BTL_END
