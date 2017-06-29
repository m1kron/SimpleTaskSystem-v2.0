#include "..\..\include\tools\print.h"
#include <windows.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////////////
void PrintToBuffer( char* buffer, size_t buffer_size, const char* format, ... )
{
	va_list args;
	va_start( args, format );
	vsprintf_s( buffer, buffer_size, format, args );
	va_end( args );
}

//////////////////////////////////////////////////////////////////////////////////
void PrintToDebugOutput( const char *format, ... )
{
	va_list args;
	va_start( args, format );
	char message[ 2048 ];
	vsprintf_s( message, format, args );
	va_end( args );
	OutputDebugStringA( message );
}
