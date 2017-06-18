#include "..\..\include\tools\assertHandler.h"
#include <windows.h>
#include <stdio.h>

void HandleAssert( const char* info, const char* file, int line )
{
	char buffer[ 2048 ];
	sprintf_s( buffer, "Condition: %s\nLine: %i\nFile: %s", info, line, file );

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
