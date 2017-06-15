#include <commonlib\prints\Prints.h>
#include <windows.h>
#include <stdio.h>

void Print( const char *format, ... )
{
	va_list args;
	va_start( args, format );
	char message[ 2048 ];
	vsprintf_s( message, format, args );
	va_end( args );
	OutputDebugString( message );
}