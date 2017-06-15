#pragma once

#ifdef _DEBUG 
#define DEBUG_MODE
#endif // _DEBUG

#ifdef DEBUG_MODE
#define DBG_ONLY_LINE( ... ) __VA_ARGS__
#else
#define DBG_ONLY_LINE( ... )
#endif // DEBUG_MODE


#define ASSERT( condition ) if ( !( condition ) ) { __debugbreak(); }
//#define ASSERT( ... )

#define STATIC_ASSERT( condition, message ) static_assert( condition, message );

#define BASE_CLASS( base_class ) typedef base_class __base;

#define RELEASE( object ) delete object; object = nullptr;
#define SAFE_RELEASE( object ) if ( object ) delete object; object = nullptr;

#define VERIFY_SUCCES( expr ) bool val = expr; ASSERT( val );