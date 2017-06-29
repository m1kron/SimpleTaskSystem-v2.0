#include "..\..\include\tools\assertHandler.h"
#include "..\..\include\tools\print.h"
#include <windows.h>
#include <stdio.h>

void HandleAssert( const char* condition, const char* info, const char* file, int line )
{
	char buffer[ 2048 ];
	PrintToBuffer( buffer, 2048, "Info: %s\nCondition: %s\nLine: %i\nFile: %s", info, condition, line, file );

	int msgboxID = MessageBoxA(
		NULL,
		buffer,
		"ASSERTION FAILED!",
		MB_ABORTRETRYIGNORE | MB_ICONHAND
	);

	switch( msgboxID )
	{
	case IDABORT:
		ExitProcess( -1 );
		break;
	case IDTRYAGAIN:
		DebugBreak();
		break;
	case IDIGNORE:
		// TODO: add code
		break;
	}
}
