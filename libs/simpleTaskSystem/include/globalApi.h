#pragma once
#include "iTaskSystem.h"
#include "stsExportApi.h"

extern "C" STS_API sts::ITaskSystem* CreateTaskSystem();

extern "C" STS_API void DestroyTaskSystem( sts::ITaskSystem* system );
