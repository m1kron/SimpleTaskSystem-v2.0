#pragma once
#include <commonlib\compile_time_tools\IsPowerOf2.h>
#include <commonlib\Macros.h>

///////////////////////////////////////////////////////////
// Returns true if ptr has specified alignment.
template< unsigned Alignment > bool IsAligned( const void* ptr );


////////////////////////////////////////////////////////////////
//
// IMPLEMENTATIONS:
//
////////////////////////////////////////////////////////////////

template< unsigned Alignment > inline bool IsAligned( const void* ptr )
{
	STATIC_ASSERT( IsPowerOf2< Alignment >::value, "Alignment has to be power of 2" );

	return ( ( ( uintptr_t ) ptr ) & ( Alignment - 1 ) ) == 0;
}