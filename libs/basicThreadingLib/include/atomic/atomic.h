#pragma once
#include "..\btlApi.h"
#include "..\..\src\platform\atomicPlatform.h"

NAMESPACE_BTL_BEGIN

/////////////////////////////////////////////
// Base template for atomics.
template < class T, class AtomicImpl >
class AtomicBase : private AtomicImpl
{
public:
	// Returns contained value.
	T Load( MemoryOrder order = MemoryOrder::SeqCst ) const;

	// Set value to contained value. Returns old value.
	void Store( T value, MemoryOrder order = MemoryOrder::SeqCst );

	// CAS operation. Returns true if exchange succeded. If false, expected_val will contain current
	// value of atomic ( WILL BE CHANGED ).
	bool CompareExchange( T& expected_val, T value_to_set, MemoryOrder order = MemoryOrder::SeqCst );

	// Exchange operation: sets desired value and returns old one ( just before desired was set ).
	T Exchange( T desired, MemoryOrder order = MemoryOrder::SeqCst );

	// Adds value to contained value. Returns RESULT of the operation.
	T FetchAdd( T value );

	// Substract value from contained value. Returns RESULT of the operation.
	T FetchSub( T value );

	// Increment operation, returns contained value after operation.
	T Increment();

	// Decrement operation, returns contained value after operation.
	T Decrement();

	// Performs logical AND. Returns contained value BEFORE the operation.
	T FetchAnd( T value );

	// Performs logical OR. Returns contained value BEFORE the operation. 
	T FetchOr( T value );

	//Operators:
	operator T() const;       // Conversion operator.
	T operator=( T val );     // Assign operator.
	T operator--( );          // PreIncrement operator, returns contained value after operation.
	T operator++( );          // PreDecrement operator, returns contained value after operation.
	T operator+=( T val );    // Add-Assign operator.
	T operator-=( T val );    // Subtruct-Assign operator.
	T operator&=( T val );    // And-Assign operator.
	T operator|=( T val );    // Or-Assign operator.
};

// Main template atomic implementation, 
// selects base implementation for given size and platform.
// Only specializations compiles.
template < class T, uint32_t size = sizeof( T )>
class Atomic
{
};

// Specialization for 32 bit atomics.
template < class T >
class Atomic< T, 4 > : public AtomicBase< T, PlatformAPI::Atomic32Impl >
{
};

///////////////////////////////////////////////////////////////
//
// IMLINES:
//
///////////////////////////////////////////////////////////////

//////////////////////////////////////////////////
template < class T, class AtomicImpl > inline T AtomicBase< T, AtomicImpl >::Load( MemoryOrder order ) const
{
	return static_cast< T >( AtomicImpl::Load( order ) );
}

//////////////////////////////////////////////////
template < class T, class AtomicImpl > inline void AtomicBase< T, AtomicImpl >::Store( T value, MemoryOrder order )
{
	AtomicImpl::Store( ( AtomicImpl::TAtomicType ) value, order );
}

//////////////////////////////////////////////////
template < class T, class AtomicImpl > inline bool AtomicBase< T, AtomicImpl >::CompareExchange( T& expected_val, T value_to_set, MemoryOrder order )
{
	return AtomicImpl::CompareExchange( ( AtomicImpl::TAtomicType& ) expected_val, ( AtomicImpl::TAtomicType ) value_to_set, order );
}

//////////////////////////////////////////////////
template < class T, class AtomicImpl > inline T AtomicBase< T, AtomicImpl >::Exchange( T value, MemoryOrder order )
{
	return AtomicImpl::Exchange( ( AtomicImpl::TAtomicType ) value, order );
}

//////////////////////////////////////////////////
template < class T, class AtomicImpl > inline T AtomicBase< T, AtomicImpl >::FetchAdd( T value )
{
	return static_cast< T >( AtomicImpl::FetchAdd( ( AtomicImpl::TAtomicType ) value ) );
}

//////////////////////////////////////////////////
template < class T, class AtomicImpl > inline T AtomicBase< T, AtomicImpl >::FetchSub( T value )
{
	return static_cast< T >( AtomicImpl::FetchSub( ( AtomicImpl::TAtomicType ) value ) );
}

//////////////////////////////////////////////////
template < class T, class AtomicImpl > inline T AtomicBase< T, AtomicImpl >::Increment()
{
	return static_cast< T >( AtomicImpl::Increment() );
}

//////////////////////////////////////////////////
template < class T, class AtomicImpl > inline T AtomicBase< T, AtomicImpl >::Decrement()
{
	return static_cast< T >( AtomicImpl::Decrement() );
}

//////////////////////////////////////////////////
template < class T, class AtomicImpl > inline T AtomicBase< T, AtomicImpl >::FetchAnd( T value )
{
	return static_cast< T >( AtomicImpl::FetchAnd( ( AtomicImpl::TAtomicType ) value ) );
}

//////////////////////////////////////////////////
template < class T, class AtomicImpl > inline T AtomicBase< T, AtomicImpl >::FetchOr( T value )
{
	return static_cast< T >( AtomicImpl::FetchOr( ( AtomicImpl::TAtomicType ) value ) );
}

//////////////////////////////////////////////////
template < class T, class AtomicImpl > inline AtomicBase< T, AtomicImpl >::operator T() const
{
	return Load();
}

//////////////////////////////////////////////////
template < class T, class AtomicImpl > inline T AtomicBase< T, AtomicImpl >::operator=( T value )
{
	Store( value );
}

//////////////////////////////////////////////////
template < class T, class AtomicImpl > inline T AtomicBase< T, AtomicImpl >::operator++( )
{
	return Increment();
}

//////////////////////////////////////////////////
template < class T, class AtomicImpl > inline T AtomicBase< T, AtomicImpl >::operator--( )
{
	return Decrement();
}

//////////////////////////////////////////////////
template < class T, class AtomicImpl > inline T AtomicBase< T, AtomicImpl >::operator+=( T value )
{
	return FetchAdd( value );
}

//////////////////////////////////////////////////
template < class T, class AtomicImpl > inline T AtomicBase< T, AtomicImpl >::operator-=( T value )
{
	return FetchSub( value );
}

//////////////////////////////////////////////////
template < class T, class AtomicImpl > inline T AtomicBase< T, AtomicImpl >::operator&=( T value )
{
	return FetchAnd( value );
}

//////////////////////////////////////////////////
template < class T, class AtomicImpl > inline T AtomicBase< T, AtomicImpl >::operator|=( T value )
{
	return FetchOr( value );
}

NAMESPACE_BTL_END
