#pragma once

#define NAMESPACE_STS_BEGIN namespace sts { // Simple Task System
#define NAMESPACE_STS_END }

#define NAMESPACE_PROTOTYPE_BEGIN namespace Prototype {
#define NAMESPACE_PROTOTYPE_END }

NAMESPACE_STS_BEGIN

static const uint32_t TASK_POOL_SIZE = 2048;
static const uint32_t TASK_FIBER_POOL_SIZE = 512;

NAMESPACE_STS_END