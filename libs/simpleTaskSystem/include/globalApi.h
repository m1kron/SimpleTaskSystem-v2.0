#pragma once
#include "iTaskManager.h"
#include "stsExportApi.h"

extern "C" STS_API sts::ITaskManager* CreateTaskSystem();

extern "C" STS_API void DestroyTaskSystem( sts::ITaskManager* system );