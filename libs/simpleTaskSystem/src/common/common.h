#pragma once

#define NAMESPACE_STS_BEGIN namespace sts { // Simple Task System
#define NAMESPACE_STS_END }

#define NAMESPACE_COMMON_BEGIN namespace common {
#define NAMESPACE_COMMON_END }

#define NAMESPACE_FRONTEND_BEGIN namespace frontend {
#define NAMESPACE_FRONTEND_END }

#define NAMESPACE_BACKEND_BEGIN namespace backend {
#define NAMESPACE_BACKEND_END }

NAMESPACE_STS_BEGIN
NAMESPACE_COMMON_BEGIN

static const uint32_t TASK_POOL_SIZE = 2048;
static const uint32_t TASK_FIBER_POOL_SIZE = 128;
static const uint32_t TASK_FIBER_STACK_SIZE = 256*1024;

NAMESPACE_COMMON_END
NAMESPACE_STS_END