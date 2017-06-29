#pragma once
#include "iTaskSystem.h"
#include "stsExportApi.h"

// Creates task system. 
// [TODO]: add user-define alloc function.
extern "C" STS_API sts::ITaskSystem* CreateTaskSystem();

// Deallocates task system. All handles becames invalid.
extern "C" STS_API void DestroyTaskSystem( sts::ITaskSystem* system );
