#pragma once
#include "iTaskManager.h"

extern "C" sts::ITaskManager* CreateTaskSystem();

extern "C" void DestroyTaskSystem( sts::ITaskManager* system );