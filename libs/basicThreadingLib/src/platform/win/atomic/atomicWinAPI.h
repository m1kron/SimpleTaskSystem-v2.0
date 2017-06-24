#pragma once
#include "..\..\platformApi.h"
#include "..\..\..\..\include\atomic\memoryOrder.h"
#include "..\..\..\..\..\commonLib\include\tools\tools.h"
#include <windows.h>
#include <intrin.h>

NAMESPACE_BTL_BEGIN
NAMESPACE_PLATFORM_API_BEGIN

//////////////////////////////////////////////////
// Memory barrier
#define FULL_MEMORY_BARRIER MemoryBarrier()
#define COMPILER_BARRIER _ReadWriteBarrier()

//////////////////////////////////////////////////
//
// IMPLEMENTATION FOR 32 bit INTEGRAL TYPES:
//
//////////////////////////////////////////////////

class Atomic32Impl
{
public:
	typedef LONG TAtomicType;

	Atomic32Impl();

	TAtomicType Load( MemoryOrder order = MemoryOrder::SeqCst ) const;
	void Store( TAtomicType value, MemoryOrder order = MemoryOrder::SeqCst );
	bool CompareExchange( TAtomicType& expected_val, TAtomicType value_to_set, MemoryOrder order = MemoryOrder::SeqCst );
	TAtomicType Exchange( TAtomicType desired, MemoryOrder order = MemoryOrder::SeqCst );
	TAtomicType FetchAdd( TAtomicType value );
	TAtomicType FetchSub( TAtomicType value );
	TAtomicType Increment();
	TAtomicType Decrement();
	TAtomicType FetchAnd( TAtomicType value );
	TAtomicType FetchOr( TAtomicType value );

private:
	BTL_ALIGNED( 4 ) TAtomicType m_value;
};

//////////////////////////////////////////////////
//
// INLINES:
//
//////////////////////////////////////////////////

//////////////////////////////////////////////////
inline Atomic32Impl::Atomic32Impl() 
	: m_value() 
{
	ASSERT( IsAligned< 4 >( this ) );
}

inline Atomic32Impl::TAtomicType Atomic32Impl::Load( MemoryOrder order ) const
{
	TAtomicType val = m_value; //< Every load has acquire semantics on x86.
	COMPILER_BARRIER;

	if( order == MemoryOrder::SeqCst )
		FULL_MEMORY_BARRIER; //< Is this needed?

	return val;
}

//////////////////////////////////////////////////
inline void Atomic32Impl::Store( TAtomicType value, MemoryOrder order )
{
	if( order != MemoryOrder::SeqCst )
	{
		COMPILER_BARRIER;
		m_value = value; // Every store has release semantics on x86.
		COMPILER_BARRIER;
	}
	else
		_InterlockedExchange( &m_value, value );
}

//////////////////////////////////////////////////
inline bool Atomic32Impl::CompareExchange( TAtomicType& expected_val, TAtomicType value_to_set, MemoryOrder order )
{
	TAtomicType prev_val = expected_val;
	expected_val = _InterlockedCompareExchange( &m_value, value_to_set, expected_val );

	return prev_val == expected_val;
}

//////////////////////////////////////////////////
inline Atomic32Impl::TAtomicType Atomic32Impl::Exchange( TAtomicType desired, MemoryOrder order )
{
	return _InterlockedExchange( &m_value, desired );
}

//////////////////////////////////////////////////
inline Atomic32Impl::TAtomicType Atomic32Impl::FetchAdd( TAtomicType value )
{
	return _InterlockedAdd( &m_value, value );
}

//////////////////////////////////////////////////
inline Atomic32Impl::TAtomicType Atomic32Impl::FetchSub( TAtomicType value )
{
	return _InterlockedAdd( &m_value, -value );
}

//////////////////////////////////////////////////
inline Atomic32Impl::TAtomicType Atomic32Impl::Increment()
{
	return _InterlockedIncrement( &m_value );
}

//////////////////////////////////////////////////
inline Atomic32Impl::TAtomicType Atomic32Impl::Decrement()
{
	return _InterlockedDecrement( &m_value );
}

//////////////////////////////////////////////////
inline Atomic32Impl::TAtomicType Atomic32Impl::FetchAnd( TAtomicType value )
{
	return _InterlockedAnd( &m_value, value );
}

//////////////////////////////////////////////////
inline Atomic32Impl::TAtomicType Atomic32Impl::FetchOr( TAtomicType value )
{
	return _InterlockedOr( &m_value, value );
}

NAMESPACE_PLATFORM_API_END
NAMESPACE_BTL_END
