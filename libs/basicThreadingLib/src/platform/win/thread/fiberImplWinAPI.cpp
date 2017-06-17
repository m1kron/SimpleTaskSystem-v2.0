#include "fiberImplWinAPI.h"
#include "..\..\..\..\include\thread\fiber.h"
#include "..\..\..\..\..\commonLib\include\macros.h"

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

	if( !ret )
	{
		auto error = ::GetLastError();
		ASSERT( false );
	}

	return ret;
}

//////////////////////////////////////////////////////
FIBER_ID ConvertThreadToFiber( void* params )
{
	auto id = ::ConvertThreadToFiber( params );

	if( id == INVALID_FIBER_ID )
	{
		auto error = ::GetLastError();
		ASSERT( false );
	}

	return id;
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
void FiberImpl::CreateFiber( FiberBase* fiber, unsigned stackSize = 200 * 1024 )
{
	m_id = ::CreateFiber( stackSize, FiberFunction, fiber );
	ASSERT( m_id != INVALID_FIBER_ID );
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