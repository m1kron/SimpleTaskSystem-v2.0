#pragma once
#include "..\..\include\iTaskSystem.h"
#include "..\common\taskWorkerInstance\taskWorkerInstance.h"

NAMESPACE_STS_BEGIN

namespace backend
{
	class BackendTaskSystem;
}

NAMESPACE_FRONTEND_BEGIN

// This is the frontend part of the task system: mainly implements ITaskSystem
// and servs as a worker instance when needed.
class FrontendTaskSystem : public ITaskSystem
{
public:
	FrontendTaskSystem();
	~FrontendTaskSystem();

	// ITaskSystem interface:
	uint32_t GetWorkersCount() const override;
	const ITaskHandle* CreateNewTask() override;
	bool SubmitTask( const ITaskHandle* task_handle ) override;
	void ReleaseTask( const ITaskHandle* task_handle ) override;
	void WaitOnConvertedMainThread() override;
	bool ConvertMainThreadToWorkerInstance() override;
	void ConvertWorkerInstanceToMainThread() override;
	bool IsOnWorkerInstance() const override;
	void WaitOnWorkerInstance() const override;
	// ---

	// Initilalizes backend system and this frontend. Returns true if success.
	bool Initialize( backend::BackendTaskSystem* backend_system );

	// Deinitializes backend system and this frontend. Returns backend system.
	backend::BackendTaskSystem* Deinitialize();

private:
	common::TaskWorkerInstance m_helperInstanceWorker;
	backend::BackendTaskSystem* m_backend;
};

NAMESPACE_FRONTEND_END
NAMESPACE_STS_END