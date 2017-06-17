#pragma once
#include "..\..\..\commonLib\include\macros.h"
#include "..\bslApi.h"

NAMESPACE_BTL_BEGIN

///////////////////////////////////////////////
// RAII lockable wrapper.
template< class T > 
class LockGuard
{
public:
	explicit LockGuard( T& lockable );

	LockGuard( const LockGuard& ) = delete;
	LockGuard& operator= ( const LockGuard& ) = delete;

	~LockGuard();

private:
	T& m_lockable;
};

///////////////////////////////////////////////////////////
//
// IMPLEMENTATION:
//
///////////////////////////////////////////////////////////

template< class T >
inline LockGuard< T >::LockGuard( T& lockable )
	: m_lockable( lockable )
{
	m_lockable.Lock();
}

///////////////////////////////////////////////////////////
template< class T >
inline LockGuard< T >::~LockGuard()
{
	m_lockable.Unlock();
}

NAMESPACE_BTL_END
