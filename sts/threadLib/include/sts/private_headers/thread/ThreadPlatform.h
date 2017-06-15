#pragma once

#include <sts\private_headers\common\NamespaceSelect.h>

#ifdef STS_PLATFORM_WINDOWS_64
#include <sts\private_headers\winAPI\ThreadImplWinAPI.h>
#include <sts\private_headers\winAPI\FiberImplWinAPI.h>
#endif