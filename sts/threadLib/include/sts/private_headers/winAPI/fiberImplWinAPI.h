#pragma once

#include <windows.h>
#include <sts\private_headers\common\NamespaceMacros.h>

NAMESPACE_STS_BEGIN

class FiberBase;

NAMESPACE_WINAPI_BEGIN

typedef LPVOID FIBER_ID;
#define INVALID_FIBER_ID NULL

////////////////////////////////////////////////////
void SwitchToFiber( FIBER_ID );
FIBER_ID GetCurrentFiberID();
bool ConvertFiberToThread();
FIBER_ID ConvertThreadToFiber( void* params );

////////////////////////////////////////////////////
class FiberImpl
{
protected:
	FiberImpl();
	FiberImpl( FiberImpl&& other );

	FiberImpl( const FiberImpl& ) = delete;
	FiberImpl& operator=( const FiberImpl& ) = delete;

	~FiberImpl();

	void CreateFiber( FiberBase* fiber, unsigned stackSize );
	void DeleteFiber();
	FIBER_ID GetFiberID() const;

private:
	FIBER_ID m_id;
};

NAMESPACE_WINAPI_END
NAMESPACE_STS_END