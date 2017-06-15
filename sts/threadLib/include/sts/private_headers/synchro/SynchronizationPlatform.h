#pragma once

#include <sts\private_headers\common\NamespaceSelect.h>

#ifdef STS_PLATFORM_WINDOWS_64
#include <sts\private_headers\winAPI\MutexWinAPI.h>
#include <sts\private_headers\winAPI\ConditionVariableImplWinAPI.h>
#include <sts\private_headers\winAPI\ManualResetEventWinAPI.h>
#endif