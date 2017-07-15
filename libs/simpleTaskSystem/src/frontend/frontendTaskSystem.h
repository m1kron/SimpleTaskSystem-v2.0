#pragma once
#include "..\..\include\iTaskSystem.h"
#include "..\common\taskWorkerInstance\taskWorkerInstance.h"

NAMESPACE_STS_BEGIN

class BackendTaskSystem;

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
	bool Initialize( BackendTaskSystem* backend_system );

	// Deinitializes backend system and this frontend. Returns backend system.
	BackendTaskSystem* Deinitialize();

private:
	TaskWorkerInstance m_helperInstanceWorker;
	BackendTaskSystem* m_backend;
};

NAMESPACE_STS_END