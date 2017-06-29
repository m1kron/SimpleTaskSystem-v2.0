#pragma once
#include <windows.h>
#include "..\platformApi.h"
#include "..\..\..\..\..\commonLib\include\macros.h"

#define WIN_ERROR_HANDLER()										\
		auto error = ::GetLastError();							\
		ASSERT( false, "[WIN_ERROR]: error code: %i", error );
