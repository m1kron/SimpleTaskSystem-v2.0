#include "precompiledHeader.h"
#include "..\..\include\globalApi.h"
#include "..\manager\taskManager.h"

/////////////////////////////////////////////////////////////////
sts::ITaskManager* CreateTaskSystem()
{
	void* mem = _aligned_malloc( sizeof( sts::TaskManager ), BTL_CACHE_LINE_SIZE );

	sts::TaskManager* manager = new ( mem ) sts::TaskManager();
	manager->Initialize();

	return manager;
}

//////////////////////////////////////////////////////////////////
void DestroyTaskSystem( sts::ITaskManager* system )
{
	sts::TaskManager* manager = static_cast< sts::TaskManager* >( system );
	manager->Deinitialize();
	manager->~TaskManager();

	_aligned_free( manager );
}