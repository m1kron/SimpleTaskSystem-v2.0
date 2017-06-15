#include <sts\private_headers\winAPI\FiberImplWinAPI.h>
#include <commonlib\Macros.h>
#include <sts\lowlevel\thread\Fiber.h>

NAMESPACE_STS_BEGIN
NAMESPACE_WINAPI_BEGIN

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
	return ::ConvertFiberToThread();
}

//////////////////////////////////////////////////////
FIBER_ID ConvertThreadToFiber( void* params )
{
	return ::ConvertThreadToFiber( params );
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

NAMESPACE_WINAPI_END
NAMESPACE_STS_END