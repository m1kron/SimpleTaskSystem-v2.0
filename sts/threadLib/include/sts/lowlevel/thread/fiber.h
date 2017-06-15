#pragma once
#include <utility>
#include "thisFiberHelpers.h"

NAMESPACE_STS_BEGIN

////////////////////////////////////////////////////////
// Base class for all fibers in the system. To create new fiber, one has to
// derive from this class and implement FiberFunction().
class FiberBase : private PlatformAPI::FiberImpl
{
	BASE_CLASS( PlatformAPI::FiberImpl );

public:
	// Returns fiber id.
	FIBER_ID GetFiberID() const;

	// Function that will be called by fiber.
	virtual void FiberFunction() = 0;

protected:
	// If you want system default stack size, set it to 0.
	FiberBase( unsigned stackSize );
	FiberBase( FiberBase&& other );

	~FiberBase();

	FiberBase( const FiberBase& ) = delete;
	FiberBase& operator=( const FiberBase& ) = delete;
};

//////////////////////////////////////////////////////////////
//
// INLINES:
//
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
inline FiberBase::FiberBase( unsigned stackSize )
{
	__base::CreateFiber( this, stackSize );
}

//////////////////////////////////////////////////////////////
inline FiberBase::FiberBase( FiberBase&& other )
	: __base( std::forward< __base >( other ) )
{
}

//////////////////////////////////////////////////////////////
inline FiberBase::~FiberBase()
{
	__base::DeleteFiber();
}

//////////////////////////////////////////////////////////////
inline FIBER_ID FiberBase::GetFiberID() const
{
	return __base::GetFiberID();
}

NAMESPACE_STS_END