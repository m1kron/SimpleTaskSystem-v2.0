#pragma once
#include <sts\private_headers\common\NamespaceMacros.h>

NAMESPACE_STS_BEGIN

enum MemoryOrder
{
	Relaxed,
	Acquire,
	Release,
	SeqCst,
};

NAMESPACE_STS_END