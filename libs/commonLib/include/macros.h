#pragma once
#include "tools\assertHandler.h"
#include "tools\print.h"

#ifdef _DEBUG 
#define DEBUG_MODE
#endif // _DEBUG

#ifdef DEBUG_MODE
#define DBG_ONLY_LINE( ... ) __VA_ARGS__
#else
#define DBG_ONLY_LINE( ... )
#endif // DEBUG_MODE

///////////////////////////////////////////////////////////////////////////////////////
// LOG
#if defined( LOGS_ENABLED )
#define LOG( ... ) PrintToDebugOutput( __VA_ARGS__ );
#else
#define LOG( ... );
#endif

///////////////////////////////////////////////////////////////////////////////////////
// ASSERTS
#define ASSERT( condition, ... )																				\
	if( !( condition ) )																						\
	{																											\
		LOG( "ASSERTION FAILED!\n Condition: %s\n File: %s\n Line %i", #condition, __FILE__, __LINE__ );		\
		char buffer[ 2048 ];																					\
		PrintToBuffer( buffer, 2048, "" __VA_ARGS__ );															\
		HandleAssert( #condition, buffer, __FILE__, __LINE__ );													\
	}

#define STATIC_ASSERT( condition, message ) static_assert( condition, message );

///////////////////////////////////////////////////////////////////////////////////////
// UTILITIES
#define BASE_CLASS( base_class ) typedef base_class __base;
#define RELEASE( object ) delete object; object = nullptr;
#define SAFE_RELEASE( object ) if ( object ) delete object; object = nullptr;
#define VERIFY_SUCCESS( expr ) { bool val = expr; ASSERT( val ); }