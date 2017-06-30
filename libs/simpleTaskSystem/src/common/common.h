#pragma once

#define NAMESPACE_STS_BEGIN namespace sts { // Simple Task System
#define NAMESPACE_STS_END }

NAMESPACE_STS_BEGIN

static const uint32_t TASK_POOL_SIZE = 2048;
static const uint32_t TASK_FIBER_POOL_SIZE = 128;
static const uint32_t TASK_FIBER_STACK_SIZE = 256*1024;

NAMESPACE_STS_END