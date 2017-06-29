#pragma once
#include "..\macros.h"
#include "..\types.h"
#include "..\compileTimeTools\isPowerOf2.h"


// Returns true if ptr has specified alignment.
template< uint32_t Alignment > bool IsAligned( const void* ptr );

////////////////////////////////////////////////////////////////
//
// IMPLEMENTATIONS:
//
////////////////////////////////////////////////////////////////

template< uint32_t Alignment > inline bool IsAligned( const void* ptr )
{
	STATIC_ASSERT( IsPowerOf2< Alignment >::value, "Alignment has to be power of 2" );

	return ( ( ( uintptr_t ) ptr ) & ( Alignment - 1 ) ) == 0;
}