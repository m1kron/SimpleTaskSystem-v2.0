#pragma once
#include "..\btlApi.h"

NAMESPACE_BTL_BEGIN

enum MemoryOrder
{
	Relaxed,
	Acquire,
	Release,
	SeqCst,
};

NAMESPACE_BTL_END
